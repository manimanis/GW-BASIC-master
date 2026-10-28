#include "../src/basic_strings.h"
#include "../src/basic_memory.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace gwbasic;

void test_len() {
    assert(BasicStrings::len("") == 0);
    assert(BasicStrings::len("HELLO") == 5);
    std::cout << "[PASS] test_len\n";
}

void test_asc_and_chr() {
    assert(BasicStrings::asc("A") == 65);
    assert(BasicStrings::asc("Hello") == 72);
    
    bool caught = false;
    try {
        BasicStrings::asc("");
    } catch (const IllegalFunctionCallException&) {
        caught = true;
    }
    assert(caught);

    assert(BasicStrings::chr(65) == "A");
    assert(BasicStrings::chr(32) == " ");
    
    caught = false;
    try {
        BasicStrings::chr(300);
    } catch (const IllegalFunctionCallException&) {
        caught = true;
    }
    assert(caught);
    std::cout << "[PASS] test_asc_and_chr\n";
}

void test_left_right_mid() {
    std::string s = "MICROSOFT GW-BASIC";
    assert(BasicStrings::left(s, 9) == "MICROSOFT");
    assert(BasicStrings::left(s, 0) == "");
    assert(BasicStrings::left(s, 50) == s);

    assert(BasicStrings::right(s, 8) == "GW-BASIC");
    assert(BasicStrings::right(s, 0) == "");
    assert(BasicStrings::right(s, 50) == s);

    assert(BasicStrings::mid(s, 11, 2) == "GW");
    assert(BasicStrings::mid(s, 11) == "GW-BASIC");
    assert(BasicStrings::mid(s, 30) == "");

    // Test LHS MID$ assignment: MID$(s, 11, 2) = "MS"
    std::string target = "MICROSOFT GW-BASIC";
    BasicStrings::lhs_mid(target, 11, 2, "MS");
    assert(target == "MICROSOFT MS-BASIC");

    // LHS MID$ does not extend length
    std::string target2 = "HELLO";
    BasicStrings::lhs_mid(target2, 4, 10, "12345");
    assert(target2 == "HEL12"); // Only 2 chars available (positions 4 and 5)
    assert(target2.size() == 5);

    std::cout << "[PASS] test_left_right_mid\n";
}

void test_concat_and_compare() {
    assert(BasicStrings::concat("GW-", "BASIC") == "GW-BASIC");
    
    // Test string too long (> 255 chars)
    std::string long1(200, 'A');
    std::string long2(100, 'B');
    bool caught = false;
    try {
        BasicStrings::concat(long1, long2);
    } catch (const StringTooLongException&) {
        caught = true;
    }
    assert(caught);

    assert(BasicStrings::compare("ABC", "ABC") == 0);
    assert(BasicStrings::compare("ABC", "ABD") == -1);
    assert(BasicStrings::compare("ABD", "ABC") == 1);
    std::cout << "[PASS] test_concat_and_compare\n";
}

void test_instr() {
    std::string s1 = "HELLO WORLD";
    assert(BasicStrings::instr(s1, "WORLD") == 7);
    assert(BasicStrings::instr(s1, "XYZ") == 0);
    assert(BasicStrings::instr(8, s1, "O") == 8); // "WORLD" has 'O' at position 8
    assert(BasicStrings::instr(9, s1, "O") == 0);
    assert(BasicStrings::instr(s1, "") == 1);
    std::cout << "[PASS] test_instr\n";
}

void test_val_and_str() {
    assert(BasicStrings::str(42) == " 42");
    assert(BasicStrings::str(-42) == "-42");
    assert(BasicStrings::str(3.14) == " 3.14");

    assert(BasicStrings::val("  123.45") == 123.45);
    assert(BasicStrings::val(" -50") == -50.0);
    assert(BasicStrings::val("&HFF") == 255.0);
    assert(BasicStrings::val("&O10") == 8.0);
    assert(BasicStrings::val("ABC") == 0.0);

    assert(BasicStrings::hex(255) == "FF");
    assert(BasicStrings::oct(8) == "10");
    std::cout << "[PASS] test_val_and_str\n";
}

void test_space_and_string_repeat() {
    assert(BasicStrings::space(4) == "    ");
    assert(BasicStrings::string_repeat(5, '*') == "*****");
    assert(BasicStrings::string_repeat(3, "XYZ") == "XXX");
    std::cout << "[PASS] test_space_and_string_repeat\n";
}

void test_memory_and_softkeys() {
    MemoryManager mem;
    // Check GWRAM.ASM default function keys
    assert(mem.softKeys().getKey(0) == "LIST ");
    assert(mem.softKeys().getKey(1) == "RUN\r");
    assert(mem.softKeys().getKey(9) == "SCREEN 0,0,0\r");

    // Case insensitivity
    mem.setVariable("a$", std::string("test string"));
    assert(std::get<std::string>(mem.getVariable("A$")) == "test string");

    mem.setVariable("count%", static_cast<int16_t>(100));
    assert(std::get<int16_t>(mem.getVariable("COUNT%")) == 100);

    assert(mem.getFreeNumericMemory() > 0);
    assert(mem.getFreeStringMemory() > 0);
    std::cout << "[PASS] test_memory_and_softkeys\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Module 1 Tests (BISTRS & GWRAM) ===\n";
    test_len();
    test_asc_and_chr();
    test_left_right_mid();
    test_concat_and_compare();
    test_instr();
    test_val_and_str();
    test_space_and_string_repeat();
    test_memory_and_softkeys();
    std::cout << "=== ALL MODULE 1 TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
