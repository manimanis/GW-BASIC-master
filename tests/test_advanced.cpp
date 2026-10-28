#include "../src/basic_interpreter.h"
#include <iostream>
#include <cassert>

using namespace gwbasic;

void test_multi_statements_and_swap() {
    BasicInterpreter basic;
    basic.executeStatement("A = 10 : B = 20 : SWAP A, B : PRINT A; B");
    assert(basic.getOutput() == " 20 10\n");
    basic.clearOutput();

    basic.executeStatement("S$ = \"HELLO:WORLD\" : PRINT S$");
    assert(basic.getOutput() == "HELLO:WORLD\n");
    basic.clearOutput();

    std::cout << "[PASS] test_multi_statements_and_swap\n";
}

void test_data_read_restore() {
    BasicInterpreter basic;
    basic.addLine(10, "DATA 100, 200, \"GW-BASIC\", 42");
    basic.addLine(20, "READ X, Y, S$, Z");
    basic.addLine(30, "PRINT X; Y; S$; Z");
    basic.addLine(40, "RESTORE");
    basic.addLine(50, "READ A");
    basic.addLine(60, "PRINT A");

    basic.run();
    std::string out = basic.getOutput();
    assert(out.find(" 100 200GW-BASIC 42") != std::string::npos);
    assert(out.find(" 100\n") != std::string::npos);
    basic.clearOutput();

    std::cout << "[PASS] test_data_read_restore\n";
}

void test_while_wend() {
    BasicInterpreter basic;
    basic.addLine(10, "I = 1 : SUM = 0");
    basic.addLine(20, "WHILE I <= 5");
    basic.addLine(30, "  SUM = SUM + I");
    basic.addLine(40, "  I = I + 1");
    basic.addLine(50, "WEND");
    basic.addLine(60, "PRINT \"SUM =\"; SUM");

    basic.run();
    std::string out = basic.getOutput();
    assert(out.find("SUM = 15") != std::string::npos);
    basic.clearOutput();

    std::cout << "[PASS] test_while_wend\n";
}

void test_if_then_else() {
    BasicInterpreter basic;
    basic.addLine(10, "X = 5");
    basic.addLine(20, "IF X > 10 THEN PRINT \"HIGH\" ELSE PRINT \"LOW\"");
    basic.addLine(30, "IF X = 5 THEN PRINT \"EXACT\" ELSE PRINT \"OTHER\"");

    basic.run();
    std::string out = basic.getOutput();
    assert(out.find("LOW\n") != std::string::npos);
    assert(out.find("EXACT\n") != std::string::npos);
    basic.clearOutput();

    std::cout << "[PASS] test_if_then_else\n";
}

void test_peek_and_poke() {
    BasicInterpreter basic;
    basic.executeStatement("POKE 1024, 255");
    basic.executeStatement("X = PEEK(1024)");
    basic.executeStatement("PRINT X");
    assert(basic.getOutput() == " 255\n");
    basic.clearOutput();

    std::cout << "[PASS] test_peek_and_poke\n";
}

void test_draw_macro_language() {
    BasicInterpreter basic;
    basic.graphics().cls(0);
    basic.graphics().setCursor(100, 100);

    // Draw a box of 20x20 in color 2: R20, D20, L20, U20
    basic.executeStatement("DRAW \"C2 R20 D20 L20 U20\"");

    assert(basic.graphics().point(110, 100) == 2); // Top edge
    assert(basic.graphics().point(120, 110) == 2); // Right edge
    assert(basic.graphics().point(110, 120) == 2); // Bottom edge
    assert(basic.graphics().point(100, 110) == 2); // Left edge
    assert(basic.graphics().point(110, 110) == 0); // Center untouched

    // Test POINT function in evaluator
    basic.executeStatement("COL = POINT(110, 100) : PRINT COL");
    assert(basic.getOutput() == " 2\n");
    basic.clearOutput();

    std::cout << "[PASS] test_draw_macro_language\n";
}

void test_print_using() {
    BasicInterpreter basic;
    basic.executeStatement("PRINT USING \"###.##\"; 12.3456");
    assert(basic.getOutput() == " 12.35\n");
    basic.clearOutput();

    basic.executeStatement("PRINT USING \"$$###.##\"; 49.9");
    assert(basic.getOutput() == "$49.90\n");
    basic.clearOutput();

    std::cout << "[PASS] test_print_using\n";
}

void test_on_goto() {
    BasicInterpreter basic;
    basic.addLine(10, "CHOICE = 2");
    basic.addLine(20, "ON CHOICE GOTO 100, 200, 300");
    basic.addLine(30, "PRINT \"ERROR\" : END");
    basic.addLine(100, "PRINT \"ONE\" : END");
    basic.addLine(200, "PRINT \"TWO\" : END");
    basic.addLine(300, "PRINT \"THREE\" : END");

    basic.run();
    std::string out = basic.getOutput();
    assert(out == "TWO\n");
    basic.clearOutput();

    std::cout << "[PASS] test_on_goto\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Advanced Statements & Macro Language Tests ===\n";
    test_multi_statements_and_swap();
    test_data_read_restore();
    test_while_wend();
    test_if_then_else();
    test_peek_and_poke();
    test_draw_macro_language();
    test_print_using();
    test_on_goto();
    std::cout << "=== ALL ADVANCED TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
