#pragma once

#include "basic_types.h"
#include <cstdint>
#include <cmath>

namespace gwbasic {

/**
 * C++ implementation of GW-BASIC mathematical routines (originally in MATH1.ASM & MATH2.ASM)
 * Implements standard transcendentals, integer operators, and Knuth LCG RND generator.
 */
class BasicMath {
public:
    // Core Transcendentals
    static double abs_fn(double x);
    static int sgn_fn(double x);
    static double int_fn(double x);   // Floor: INT(-3.4) = -4
    static double fix_fn(double x);   // Trunc: FIX(-3.4) = -3
    static double sqr_fn(double x);   // Square root (x >= 0)
    static double log_fn(double x);   // Natural logarithm (x > 0)
    static double exp_fn(double x);   // Exponential e^x
    static double sin_fn(double x);   // Sine (radians)
    static double cos_fn(double x);   // Cosine (radians)
    static double tan_fn(double x);   // Tangent (radians)
    static double atn_fn(double x);   // Arctangent (radians)
    static double pow_fn(double base, double exp); // base ^ exp

    // Integer / Binary Operators
    static int16_t cint_fn(double x); // Round to nearest 16-bit integer
    static int64_t idiv_fn(int64_t a, int64_t b); // Integer division '\'
    static int64_t imod_fn(int64_t a, int64_t b); // Integer modulo 'MOD'

    // Pseudo-Random Number Generator (Knuth Vol 2 Ch 3 Linear Congruential Generator)
    // Formula: RND(N+1) = (RND(N) * 214013 + 2531011) MOD 2^24
    // RND(x):
    //  - x < 0: Seeds generator with x, generates next number
    //  - x > 0: Generates next random number in [0.0, 1.0)
    //  - x = 0: Returns previous random number
    static double rnd(double x = 1.0);
    static void randomize(uint32_t seed);
    static void reset_rnd();

private:
    static inline uint32_t rnd_state_ = 0x000001; // Initial state matching GW-BASIC $RND0
    static inline double last_rnd_value_ = 0.0;
    static constexpr uint32_t RND_A = 214013;
    static constexpr uint32_t RND_C = 2531011;
    static constexpr uint32_t RND_M = 16777216; // 2^24
};

} // namespace gwbasic
