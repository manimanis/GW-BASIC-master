#include "basic_math.h"
#include <limits>

namespace gwbasic {

double BasicMath::abs_fn(double x) {
    return std::abs(x);
}

int BasicMath::sgn_fn(double x) {
    if (x > 0.0) return 1;
    if (x < 0.0) return -1;
    return 0;
}

double BasicMath::int_fn(double x) {
    return std::floor(x);
}

double BasicMath::fix_fn(double x) {
    return std::trunc(x);
}

double BasicMath::sqr_fn(double x) {
    if (x < 0.0) {
        throw IllegalFunctionCallException();
    }
    return std::sqrt(x);
}

double BasicMath::log_fn(double x) {
    if (x <= 0.0) {
        throw IllegalFunctionCallException();
    }
    return std::log(x);
}

double BasicMath::exp_fn(double x) {
    // 88.02969 is the single/double overflow threshold in 8086 BASIC
    if (x > 88.02969) {
        throw BasicException(ErrorCode::Overflow, "Overflow in EXP");
    }
    return std::exp(x);
}

double BasicMath::sin_fn(double x) {
    return std::sin(x);
}

double BasicMath::cos_fn(double x) {
    return std::cos(x);
}

double BasicMath::tan_fn(double x) {
    return std::tan(x);
}

double BasicMath::atn_fn(double x) {
    return std::atan(x);
}

double BasicMath::pow_fn(double base, double exp) {
    if (base == 0.0) {
        if (exp < 0.0) {
            throw BasicException(ErrorCode::DivisionByZero, "Division by zero");
        }
        if (exp == 0.0) return 1.0;
        return 0.0;
    }
    if (base < 0.0) {
        // In GW-BASIC, a negative base must have an integer exponent
        if (exp != std::floor(exp)) {
            throw IllegalFunctionCallException();
        }
    }
    double result = std::pow(base, exp);
    if (std::isinf(result)) {
        throw BasicException(ErrorCode::Overflow, "Overflow in power");
    }
    return result;
}

int16_t BasicMath::cint_fn(double x) {
    double r = std::round(x);
    if (r < -32768.0 || r > 32767.0) {
        throw BasicException(ErrorCode::Overflow, "Overflow: Integer out of range");
    }
    return static_cast<int16_t>(r);
}

int64_t BasicMath::idiv_fn(int64_t a, int64_t b) {
    if (b == 0) {
        throw BasicException(ErrorCode::DivisionByZero, "Division by zero");
    }
    return a / b;
}

int64_t BasicMath::imod_fn(int64_t a, int64_t b) {
    if (b == 0) {
        throw BasicException(ErrorCode::DivisionByZero, "Division by zero");
    }
    return a % b;
}

double BasicMath::rnd(double x) {
    if (x < 0.0) {
        // Reseed with value
        uint32_t seed = static_cast<uint32_t>(std::abs(x) * 16777216.0);
        rnd_state_ = seed % RND_M;
        if (rnd_state_ == 0) rnd_state_ = 1;
    } else if (x == 0.0) {
        // Return previous random value without advancing
        return last_rnd_value_;
    }

    // Advance LCG
    rnd_state_ = static_cast<uint32_t>((static_cast<uint64_t>(rnd_state_) * RND_A + RND_C) % RND_M);
    last_rnd_value_ = static_cast<double>(rnd_state_) / static_cast<double>(RND_M);
    return last_rnd_value_;
}

void BasicMath::randomize(uint32_t seed) {
    rnd_state_ = seed % RND_M;
    if (rnd_state_ == 0) rnd_state_ = 1;
    rnd(1.0);
}

void BasicMath::reset_rnd() {
    rnd_state_ = 0x000001;
    last_rnd_value_ = 0.0;
}

} // namespace gwbasic
