#include "../src/basic_math.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace gwbasic;

void test_core_math() {
    assert(BasicMath::abs_fn(-42.5) == 42.5);
    assert(BasicMath::abs_fn(42.5) == 42.5);

    assert(BasicMath::sgn_fn(15.2) == 1);
    assert(BasicMath::sgn_fn(-15.2) == -1);
    assert(BasicMath::sgn_fn(0.0) == 0);

    // INT vs FIX
    assert(BasicMath::int_fn(3.7) == 3.0);
    assert(BasicMath::int_fn(-3.7) == -4.0);
    assert(BasicMath::fix_fn(3.7) == 3.0);
    assert(BasicMath::fix_fn(-3.7) == -3.0);

    // SQR
    assert(BasicMath::sqr_fn(16.0) == 4.0);
    bool caught = false;
    try {
        BasicMath::sqr_fn(-1.0);
    } catch (const IllegalFunctionCallException&) {
        caught = true;
    }
    assert(caught);

    // LOG
    assert(std::abs(BasicMath::log_fn(std::exp(1.0)) - 1.0) < 1e-9);
    caught = false;
    try {
        BasicMath::log_fn(0.0);
    } catch (const IllegalFunctionCallException&) {
        caught = true;
    }
    assert(caught);

    // EXP overflow
    caught = false;
    try {
        BasicMath::exp_fn(100.0);
    } catch (const BasicException& e) {
        assert(e.code == ErrorCode::Overflow);
        caught = true;
    }
    assert(caught);

    // Trigonometry
    assert(std::abs(BasicMath::sin_fn(0.0)) < 1e-9);
    assert(std::abs(BasicMath::cos_fn(0.0) - 1.0) < 1e-9);
    assert(std::abs(BasicMath::atn_fn(1.0) - (std::atan(1.0))) < 1e-9);

    std::cout << "[PASS] test_core_math\n";
}

void test_powers_and_integers() {
    // Powers
    assert(BasicMath::pow_fn(2.0, 3.0) == 8.0);
    assert(BasicMath::pow_fn(-2.0, 3.0) == -8.0);
    assert(BasicMath::pow_fn(9.0, 0.5) == 3.0);

    // Negative base with non-integer exp should fail
    bool caught = false;
    try {
        BasicMath::pow_fn(-4.0, 0.5);
    } catch (const IllegalFunctionCallException&) {
        caught = true;
    }
    assert(caught);

    // CINT
    assert(BasicMath::cint_fn(3.6) == 4);
    assert(BasicMath::cint_fn(-3.6) == -4);
    caught = false;
    try {
        BasicMath::cint_fn(40000.0);
    } catch (const BasicException& e) {
        assert(e.code == ErrorCode::Overflow);
        caught = true;
    }
    assert(caught);

    // Integer division and modulo
    assert(BasicMath::idiv_fn(17, 5) == 3);
    assert(BasicMath::imod_fn(17, 5) == 2);

    caught = false;
    try {
        BasicMath::idiv_fn(10, 0);
    } catch (const BasicException& e) {
        assert(e.code == ErrorCode::DivisionByZero);
        caught = true;
    }
    assert(caught);

    std::cout << "[PASS] test_powers_and_integers\n";
}

void test_rnd() {
    BasicMath::reset_rnd();
    double r1 = BasicMath::rnd(1.0);
    double r2 = BasicMath::rnd(1.0);
    assert(r1 >= 0.0 && r1 < 1.0);
    assert(r2 >= 0.0 && r2 < 1.0);
    assert(r1 != r2);

    // RND(0) returns exact same value
    double r2_repeat = BasicMath::rnd(0.0);
    assert(r2 == r2_repeat);

    // Negative seed restarts deterministic sequence
    BasicMath::rnd(-10.0);
    double s1 = BasicMath::rnd(1.0);
    BasicMath::rnd(-10.0);
    double s2 = BasicMath::rnd(1.0);
    assert(s1 == s2);

    std::cout << "[PASS] test_rnd (Knuth LCG matching GW-BASIC $RND)\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Module 2 Tests (MATH1 & MATH2) ===\n";
    test_core_math();
    test_powers_and_integers();
    test_rnd();
    std::cout << "=== ALL MODULE 2 TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
