#include "basic_interpreter.h"
#include "sdl_window.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#endif

using namespace gwbasic;

void print_banner(const MemoryManager& mem) {
    std::cout << "GW-BASIC 3.23 (Modern C++ Native Port)\n"
              << "(C) Copyright Microsoft 1983 / C++ Re-implementation\n"
              << mem.getFreeNumericMemory() << " Bytes free\n"
              << "Ok\n";
}

void wait_for_key_and_close(SDLWindow& win) {
    if (!win.isOpen()) return;

    std::cout << "\nAppuyez sur une touche (dans la fenetre ou la console) pour fermer...\n";
    // Drain previous keys
    while (win.hasKey()) win.popKey();
#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        while (_kbhit()) _getch();
    }
#endif

    while (win.isOpen()) {
        win.pollEvents();
        if (!win.isOpen()) break;
        if (win.hasKey()) {
            win.popKey();
            win.close();
            break;
        }
#ifdef _WIN32
        if (_isatty(_fileno(stdin))) {
            if (_kbhit()) {
                _getch();
                win.close();
                break;
            }
        } else {
            // Non-interactive or piped stdin: close window automatically so tests/pipes don't block
            win.close();
            break;
        }
#endif
        SDL_Delay(16);
    }
}

int main(int argc, char* argv[]) {
    BasicInterpreter interpreter;

    if (argc > 1) {
        // Parse flags
        bool use_sdl = false;
        std::string script_file;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--gui" || arg == "-g") {
                use_sdl = true;
            } else {
                script_file = arg;
            }
        }

        if (script_file.empty()) {
            std::cerr << "Usage: gwbasic [--gui] <script.bas>\n";
            return 1;
        }

        std::ifstream file(script_file);
        if (!file.is_open()) {
            std::cerr << "File not found: " << script_file << "\n";
            return 1;
        }

        std::string line;
        while (std::getline(file, line)) {
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;

            if (std::isdigit(static_cast<unsigned char>(line[start]))) {
                size_t space_pos = line.find_first_of(" \t", start);
                int line_num = std::stoi(line.substr(start, space_pos - start));
                std::string code = (space_pos != std::string::npos) ? line.substr(space_pos + 1) : "";
                interpreter.addLine(line_num, code);
            } else {
                interpreter.executeStatement(line.substr(start));
            }
        }

        SDLWindow win("GW-BASIC 3.23 - CGA Display", 3);

        auto auto_open_sdl = [&]() {
            auto& gfx = interpreter.graphics();
            if (!gfx.isDirty()) return;
            gfx.clearDirty();

            if (!win.isOpen()) {
                if (!win.init(gfx.getWidth(), gfx.getHeight())) {
                    return;
                }
            }
            if (win.isOpen()) {
                win.render(gfx);
            }
        };

        auto step_hook = [&]() {
            auto_open_sdl();
            if (win.isOpen()) {
                win.pollEvents();
                while (win.hasKey()) {
                    interpreter.console().pushKey(win.popKey());
                }
                if (!win.isOpen() && interpreter.isRunning()) {
                    interpreter.stop();
                }
            }
        };

        interpreter.setStepHook(step_hook);

        try {
            interpreter.run();
            std::cout << interpreter.getOutput();
            auto_open_sdl();
            if (win.isOpen()) {
                wait_for_key_and_close(win);
            }
            return 0;
        } catch (const BasicException& e) {
            std::cerr << "Runtime Error at line " << interpreter.getCurrentLine()
                      << ": " << e.what() << "\n";
            std::cout << interpreter.getOutput();
            auto_open_sdl();
            if (win.isOpen()) {
                wait_for_key_and_close(win);
            }
            return 1;
        }
    }

    // ========== Interactive REPL Mode with SDL2 ==========
    print_banner(interpreter.memory());

    SDLWindow win("GW-BASIC 3.23 - CGA Monitor", 3);
    bool sdl_active = false;

    // Auto-open SDL window when any graphics routine touches the framebuffer
    auto auto_open_sdl = [&]() {
        auto& gfx = interpreter.graphics();
        if (!gfx.isDirty()) return;
        gfx.clearDirty();

        if (!win.isOpen()) {
            if (win.init(gfx.getWidth(), gfx.getHeight())) {
                sdl_active = true;
            } else {
                return;
            }
        }
        if (win.isOpen()) {
            win.render(gfx);
        }
    };

    auto step_hook = [&]() {
        auto_open_sdl();
        if (win.isOpen()) {
            win.pollEvents();
            while (win.hasKey()) {
                interpreter.console().pushKey(win.popKey());
            }
            if (!win.isOpen() && interpreter.isRunning()) {
                interpreter.stop();
            }
        }
    };

    interpreter.setStepHook(step_hook);

    std::string input;
    while (true) {
        // Poll SDL events if window is open
        if (win.isOpen()) {
            win.pollEvents();
            while (win.hasKey()) {
                interpreter.console().pushKey(win.popKey());
            }
        }

        std::cout << "";
        if (!std::getline(std::cin, input)) break;

        size_t start = input.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;

        std::string trimmed = input.substr(start);

        std::string upper = trimmed;
        for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

        if (upper == "SYSTEM" || upper == "EXIT" || upper == "QUIT") {
            break;
        }

        try {
            if (std::isdigit(static_cast<unsigned char>(trimmed[0]))) {
                size_t space_pos = trimmed.find_first_of(" \t");
                int line_num = std::stoi(trimmed.substr(0, space_pos));
                std::string code = (space_pos != std::string::npos) ? trimmed.substr(space_pos + 1) : "";
                if (code.empty()) {
                    interpreter.deleteLine(line_num);
                } else {
                    interpreter.addLine(line_num, code);
                }
            } else if (upper.substr(0, 3) == "RUN") {
                interpreter.clearOutput();
                std::string args = trimmed.substr(3);
                // Trim leading whitespace from args
                size_t astart = args.find_first_not_of(" \t");
                if (astart == std::string::npos) {
                    // Plain RUN
                    interpreter.run();
                } else {
                    args = args.substr(astart);
                    if (args.front() == '"') {
                        // RUN "filename.bas" — load and run
                        size_t end_quote = args.find('"', 1);
                        if (end_quote == std::string::npos) {
                            std::cout << "Syntax error\nOk\n";
                            continue;
                        }
                        std::string fname = args.substr(1, end_quote - 1);
                        interpreter.loadProgram(fname, true);
                    } else {
                        // RUN line_number
                        int start_line = std::stoi(args);
                        interpreter.run(start_line);
                    }
                }
                std::cout << interpreter.getOutput();
                auto_open_sdl();
                if (win.isOpen()) {
                    wait_for_key_and_close(win);
                }
                std::cout << "Ok\n";
            } else if (upper.substr(0, 4) == "LIST") {
                std::string args = trimmed.substr(4);
                size_t astart = args.find_first_not_of(" \t");
                if (astart == std::string::npos) {
                    // Plain LIST — show all lines
                    std::cout << interpreter.listProgram();
                } else {
                    args = args.substr(astart);
                    // Parse range: LIST n, LIST n-, LIST -n, LIST n-m
                    size_t dash = args.find('-');
                    if (dash == std::string::npos) {
                        // LIST n — show single line
                        int line = std::stoi(args);
                        std::cout << interpreter.listProgram(line, line);
                    } else if (dash == 0) {
                        // LIST -n — from beginning to n
                        int end_line = std::stoi(args.substr(1));
                        std::cout << interpreter.listProgram(0, end_line);
                    } else if (dash == args.size() - 1) {
                        // LIST n- — from n to end
                        int start_line = std::stoi(args.substr(0, dash));
                        std::cout << interpreter.listProgram(start_line, 65535);
                    } else {
                        // LIST n-m
                        int start_line = std::stoi(args.substr(0, dash));
                        int end_line = std::stoi(args.substr(dash + 1));
                        std::cout << interpreter.listProgram(start_line, end_line);
                    }
                }
                std::cout << "Ok\n";
            } else if (upper.substr(0, 4) == "LOAD") {
                std::string args = trimmed.substr(4);
                size_t astart = args.find_first_not_of(" \t");
                if (astart == std::string::npos || args[astart] != '"') {
                    std::cout << "Syntax error\nOk\n";
                    continue;
                }
                args = args.substr(astart);
                size_t end_quote = args.find('"', 1);
                if (end_quote == std::string::npos) {
                    std::cout << "Syntax error\nOk\n";
                    continue;
                }
                std::string fname = args.substr(1, end_quote - 1);
                // Check for ,R flag (run after loading)
                bool run_after = false;
                std::string rest = args.substr(end_quote + 1);
                size_t rstart = rest.find_first_not_of(" \t");
                if (rstart != std::string::npos && rest[rstart] == ',') {
                    std::string flag = rest.substr(rstart + 1);
                    size_t fstart = flag.find_first_not_of(" \t");
                    if (fstart != std::string::npos) {
                        char fc = std::toupper(static_cast<unsigned char>(flag[fstart]));
                        if (fc == 'R') run_after = true;
                    }
                }
                interpreter.loadProgram(fname, run_after);
                if (run_after) {
                    std::cout << interpreter.getOutput();
                    auto_open_sdl();
                    if (win.isOpen()) {
                        wait_for_key_and_close(win);
                    }
                }
                std::cout << "Ok\n";
            } else if (upper.substr(0, 4) == "SAVE") {
                std::string args = trimmed.substr(4);
                size_t astart = args.find_first_not_of(" \t");
                if (astart == std::string::npos || args[astart] != '"') {
                    std::cout << "Syntax error\nOk\n";
                    continue;
                }
                args = args.substr(astart);
                size_t end_quote = args.find('"', 1);
                if (end_quote == std::string::npos) {
                    std::cout << "Syntax error\nOk\n";
                    continue;
                }
                std::string fname = args.substr(1, end_quote - 1);
                // Check for ,A flag (ASCII format — always used since we don't do tokenized)
                bool ascii = true;
                std::string rest = args.substr(end_quote + 1);
                size_t rstart = rest.find_first_not_of(" \t");
                if (rstart != std::string::npos && rest[rstart] == ',') {
                    // ,A is accepted (and is the default); ,P (protected) not supported
                    std::string flag = rest.substr(rstart + 1);
                    size_t fstart = flag.find_first_not_of(" \t");
                    if (fstart != std::string::npos) {
                        char fc = std::toupper(static_cast<unsigned char>(flag[fstart]));
                        if (fc == 'P') {
                            std::cout << "Feature not supported\nOk\n";
                            continue;
                        }
                    }
                }
                interpreter.saveProgram(fname, ascii);
                std::cout << "Ok\n";
            } else if (upper == "NEW") {
                interpreter.newProgram();
                std::cout << "Ok\n";
            } else {
                interpreter.clearOutput();
                interpreter.executeStatement(trimmed);
                std::cout << interpreter.getOutput();
                auto_open_sdl();
                if (win.isOpen()) {
                    wait_for_key_and_close(win);
                }
                std::cout << "Ok\n";
            }
        } catch (const BasicException& e) {
            std::cout << e.what() << "\nOk\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\nOk\n";
        }
    }

    if (sdl_active) {
        win.close();
    }

    return 0;
}
