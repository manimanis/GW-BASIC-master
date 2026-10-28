#pragma once

#include "basic_types.h"
#include "basic_memory.h"
#include "basic_eval.h"
#include "basic_graphics.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <functional>

namespace gwbasic {

struct ProgramPosition {
    int line = 0;
    size_t stmt_idx = 0;
};

struct ForLoopState {
    std::string var_name;
    double target;
    double step;
    int body_line;
    size_t body_stmt_idx;
};

class BasicInterpreter {
public:
    BasicInterpreter();

    // Program line management
    void addLine(int line_number, const std::string& text);
    void deleteLine(int line_number);
    void newProgram();
    std::string listProgram() const;
    std::string listProgram(int start_line, int end_line) const;

    // Execution
    void run();
    void run(int start_line);
    void executeStatement(const std::string& statement);
    void executeProgram();

    // File I/O commands (LOAD / SAVE)
    void loadProgram(const std::string& filename, bool run_after = false);
    void saveProgram(const std::string& filename, bool ascii_format = true);

    // Output capture (for tests and headless running)
    std::string getOutput() const { return output_.str(); }
    void clearOutput() { output_.str(""); output_.clear(); }
    int getCurrentLine() const { return current_line_; }
    void stop() { is_running_ = false; }
    bool isRunning() const { return is_running_; }

    using StepHook = std::function<void()>;
    void setStepHook(StepHook hook) { step_hook_ = std::move(hook); }

    MemoryManager& memory() { return mem_; }
    BasicGraphics& graphics() { return gfx_; }
    FileManager& files() { return files_; }
    ConsoleIO& console() { return console_; }

private:
    StepHook step_hook_;
    MemoryManager mem_;
    BasicGraphics gfx_;
    FileManager files_;
    ConsoleIO console_;
    BasicEvaluator eval_;
    std::ostringstream output_;

    // Stored program lines ordered by line number
    std::map<int, std::string> program_;
    int current_line_ = 0;
    size_t current_stmt_idx_ = 0;
    bool is_running_ = false;

    // Call stack for GOSUB / RETURN
    std::vector<ProgramPosition> gosub_stack_;

    // Loop stack for FOR / NEXT
    std::vector<ForLoopState> for_stack_;

    // Loop stack for WHILE / WEND
    std::vector<ProgramPosition> while_stack_;

    // DATA / READ / RESTORE pool (line_number, item string)
    std::vector<std::pair<int, std::string>> data_items_;
    size_t data_ptr_ = 0;

    void rebuildDataPool();

    void executeLine(const std::string& line_text);
    void executeSingleStatement(std::string_view stmt, bool& branch_taken);
    std::vector<std::string> splitStatements(std::string_view line);

    void handlePrint(std::string_view& stmt);
    void handleLet(std::string_view& stmt);
    void handleGoto(std::string_view& stmt);
    void handleGosub(std::string_view& stmt);
    void handleReturn();
    void handleIf(std::string_view& stmt, bool& branch_taken);
    void handleFor(std::string_view& stmt);
    void handleNext(std::string_view& stmt);

    // Module 4 I/O statements
    void handleOpen(std::string_view& stmt);
    void handleClose(std::string_view& stmt);
    void handleInput(std::string_view& stmt);
    void handleLineInput(std::string_view& stmt);
    void handleLocate(std::string_view& stmt);
    void handleColor(std::string_view& stmt);
    void handleWidth(std::string_view& stmt);
    void handleSaveScreen(std::string_view& stmt);

    // Graphics statements (Module 3 & ADVGRP integration)
    void handlePset(std::string_view& stmt, bool preset = false);
    void handleLine(std::string_view& stmt);
    void handleCircle(std::string_view& stmt);
    void handlePaint(std::string_view& stmt);
    void handleDraw(std::string_view& stmt);
    void handleGraphicsGet(std::string_view& stmt);
    void handleGraphicsPut(std::string_view& stmt);

    // Additional Core Statements
    void handleRead(std::string_view& stmt);
    void handleRestore(std::string_view& stmt);
    void handleWhile(std::string_view& stmt, bool& branch_taken);
    void handleWend();
    void handleSwap(std::string_view& stmt);
    void handleRandomize(std::string_view& stmt);
    void handlePoke(std::string_view& stmt);
    void handleBeep();
    void handleSound(std::string_view& stmt);
    void handleOn(std::string_view& stmt);
    void handleDim(std::string_view& stmt);

    // Error handler state
    int error_handler_line_ = 0;

    // Helpers
    std::pair<int, int> parseCoordinates(std::string_view& stmt);
    void skipToMatchingNext(const std::string& var_name);
    void skipToMatchingWend();
};

} // namespace gwbasic
