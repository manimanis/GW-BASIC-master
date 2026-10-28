#include "../src/basic_io.h"
#include "../src/basic_interpreter.h"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace gwbasic;

void test_file_manager_direct() {
    FileManager fm;
    std::string test_file = "test_io_direct.tmp";

    // 1. Output mode
    fm.openFile(1, test_file, FileMode::Output);
    fm.writeString(1, "GW-BASIC FILE I/O\n");
    fm.writeString(1, "100, 200, \"TEST STRING\"\n");
    fm.closeFile(1);

    // 2. Input mode
    fm.openFile(1, test_file, FileMode::Input);
    assert(!fm.isEOF(1));
    assert(fm.getLOF(1) > 0);

    std::string line1 = fm.readLine(1);
    assert(line1 == "GW-BASIC FILE I/O");

    std::string t1 = fm.readToken(1);
    std::string t2 = fm.readToken(1);
    std::string t3 = fm.readToken(1);

    assert(t1 == "100");
    assert(t2 == "200");
    assert(t3 == "TEST STRING");

    assert(fm.isEOF(1));
    fm.closeFile(1);

    // Cleanup
    std::remove(test_file.c_str());
    std::cout << "[PASS] test_file_manager_direct\n";
}

void test_console_and_keys() {
    ConsoleIO console;
    console.pushKey("A");
    console.pushKey("B");
    assert(console.inkey() == "A");
    assert(console.inkey() == "B");
    assert(console.inkey() == "");

    // Test digits and Escape (CHR$(27))
    console.pushKey("1");
    console.pushKey("\x1B");
    assert(console.inkey() == "1");
    assert(console.inkey() == "\x1B");
    assert(console.inkey() == "");

    // Test 2-byte extended keys (arrows, F-keys)
    console.pushKey(std::string("\0H", 2)); // Up arrow
    console.pushKey(std::string("\0;", 2)); // F1
    std::string up = console.inkey();
    assert(up.size() == 2 && up[0] == '\0' && up[1] == 'H');
    std::string f1 = console.inkey();
    assert(f1.size() == 2 && f1[0] == '\0' && f1[1] == ';');
    assert(console.inkey() == "");

    console.locate(10, 20);
    assert(console.getCsrlin() == 10);
    assert(console.getPos() == 20);

    console.setColor(14, 1, 0); // Yellow on Blue
    assert(console.getForeground() == 14);
    assert(console.getBackground() == 1);

    console.setWidth(40);
    assert(console.getWidth() == 40);

    std::cout << "[PASS] test_console_and_keys\n";
}

void test_screen_export_bmp_and_ppm() {
    BasicGraphics gfx(320, 200, ScreenMode::MediumRes);
    gfx.cls(1); // Blue background

    // Draw box and circle
    gfx.box(20, 20, 100, 100, 4, false); // Red box outline
    gfx.paint(50, 50, 14, 4);            // Yellow fill inside
    gfx.circle(200, 100, 40, 2);         // Green circle

    // Save as BMP
    std::string bmp_file = "test_screen.bmp";
    assert(ScreenExport::saveBMP(bmp_file, gfx));

    std::ifstream bmp_in(bmp_file, std::ios::binary);
    assert(bmp_in.is_open());
    char header[2];
    bmp_in.read(header, 2);
    assert(header[0] == 'B' && header[1] == 'M'); // BMP signature
    bmp_in.close();

    // Save as PPM
    std::string ppm_file = "test_screen.ppm";
    assert(ScreenExport::savePPM(ppm_file, gfx));
    std::ifstream ppm_in(ppm_file, std::ios::binary);
    assert(ppm_in.is_open());
    std::string magic;
    ppm_in >> magic;
    assert(magic == "P6"); // PPM binary format
    ppm_in.close();

    std::remove(bmp_file.c_str());
    std::remove(ppm_file.c_str());
    std::cout << "[PASS] test_screen_export_bmp_and_ppm\n";
}

void test_interpreter_file_io() {
    BasicInterpreter basic;
    std::string script_file = "test_script_output.txt";

    // Program writing to file and reading back:
    // 10 OPEN "test_script_output.txt" FOR OUTPUT AS #1
    // 20 PRINT #1, "GW-BASIC C++ HAL"
    // 30 PRINT #1, 123.45
    // 40 CLOSE #1
    // 50 OPEN "test_script_output.txt" FOR INPUT AS #1
    // 60 LINE INPUT #1, L$
    // 70 INPUT #1, NUM
    // 80 CLOSE #1
    // 90 PRINT "READBACK: "; L$; " | "; NUM

    basic.addLine(10, "OPEN \"" + script_file + "\" FOR OUTPUT AS #1");
    basic.addLine(20, "PRINT #1, \"GW-BASIC C++ HAL\"");
    basic.addLine(30, "PRINT #1, 123.45");
    basic.addLine(40, "CLOSE #1");
    basic.addLine(50, "OPEN \"" + script_file + "\" FOR INPUT AS #1");
    basic.addLine(60, "LINE INPUT #1, L$");
    basic.addLine(70, "INPUT #1, NUM");
    basic.addLine(80, "CLOSE #1");
    basic.addLine(90, "PRINT \"READBACK: \"; L$; \" | \"; NUM");

    basic.run();

    std::string out = basic.getOutput();
    assert(out.find("READBACK: GW-BASIC C++ HAL |  123.45") != std::string::npos);

    std::remove(script_file.c_str());
    std::cout << "[PASS] test_interpreter_file_io (OPEN, PRINT#, LINE INPUT#, INPUT#, CLOSE)\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Module 4 Tests (HAL: GIODSK, GIOKYB, SCNDRV) ===\n";
    test_file_manager_direct();
    test_console_and_keys();
    test_screen_export_bmp_and_ppm();
    test_interpreter_file_io();
    std::cout << "=== ALL MODULE 4 TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
