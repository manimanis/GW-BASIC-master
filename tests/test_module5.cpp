#include "../src/basic_interpreter.h"
#include <iostream>
#include <cassert>

using namespace gwbasic;

void test_direct_eval_and_print() {
    BasicInterpreter basic;
    basic.executeStatement("PRINT 10 + 20 * 3");
    assert(basic.getOutput() == " 70\n");
    basic.clearOutput();

    basic.executeStatement("PRINT \"HELLO \" + \"WORLD\"");
    assert(basic.getOutput() == "HELLO WORLD\n");
    basic.clearOutput();

    basic.executeStatement("PRINT SQR(25) + LEN(\"BASIC\")");
    assert(basic.getOutput() == " 10\n");
    basic.clearOutput();

    std::cout << "[PASS] test_direct_eval_and_print\n";
}

void test_variables_and_strings() {
    BasicInterpreter basic;
    basic.executeStatement("A = 100");
    basic.executeStatement("B$ = \"GW-BASIC\"");
    basic.executeStatement("PRINT A; B$");
    assert(basic.getOutput() == " 100GW-BASIC\n");
    basic.clearOutput();

    // In-place LHS MID$ statement test: MID$(B$, 1, 2) = "MS"
    basic.executeStatement("MID$(B$, 1, 2) = \"MS\"");
    basic.executeStatement("PRINT B$");
    assert(basic.getOutput() == "MS-BASIC\n");
    basic.clearOutput();

    std::cout << "[PASS] test_variables_and_strings\n";
}

void test_basic_program_loops_and_gosub() {
    BasicInterpreter basic;
    // Program:
    // 10 TOTAL = 0
    // 20 FOR I = 1 TO 3
    // 30 TOTAL = TOTAL + I
    // 40 NEXT I
    // 50 PRINT "TOTAL ="; TOTAL
    // 60 GOSUB 100
    // 70 END
    // 100 PRINT "SUBROUTINE CALLED"
    // 110 RETURN

    basic.addLine(10, "TOTAL = 0");
    basic.addLine(20, "FOR I = 1 TO 3");
    basic.addLine(30, "TOTAL = TOTAL + I");
    basic.addLine(40, "NEXT I");
    basic.addLine(50, "PRINT \"TOTAL =\"; TOTAL");
    basic.addLine(60, "GOSUB 100");
    basic.addLine(70, "END");
    basic.addLine(100, "PRINT \"SUBROUTINE CALLED\"");
    basic.addLine(110, "RETURN");

    basic.run();
    std::string out = basic.getOutput();
    assert(out.find("TOTAL = 6") != std::string::npos);
    assert(out.find("SUBROUTINE CALLED") != std::string::npos);
    std::cout << "[PASS] test_basic_program_loops_and_gosub\n";
}

void test_graphics_statements() {
    BasicInterpreter basic;
    // Execute graphics statements (linking Module 3 & Module 5)
    basic.executeStatement("CLS");
    basic.executeStatement("PSET (50, 60), 2");
    assert(basic.graphics().point(50, 60) == 2);

    // Box and PAINT (ADVGRP.ASM)
    basic.executeStatement("LINE (10, 10)-(40, 40), 1, B");
    assert(basic.graphics().point(20, 20) == 0); // empty inside

    // PAINT (20, 20), 2, 1
    basic.executeStatement("PAINT (20, 20), 2, 1");
    assert(basic.graphics().point(20, 20) == 2); // filled inside
    assert(basic.graphics().point(5, 5) == 0);   // outside untouched

    std::cout << "[PASS] test_graphics_statements (PSET, LINE, PAINT)\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Module 5 Tests (GWEVAL & GWMAIN) ===\n";
    test_direct_eval_and_print();
    test_variables_and_strings();
    test_basic_program_loops_and_gosub();
    test_graphics_statements();
    std::cout << "=== ALL MODULE 5 TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
