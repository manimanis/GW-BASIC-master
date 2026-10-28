#include "basic_strings.h"
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <cmath>

namespace gwbasic {

int BasicStrings::len(const std::string& s) {
    return static_cast<int>(s.size());
}

int BasicStrings::asc(const std::string& s) {
    if (s.empty()) {
        throw IllegalFunctionCallException();
    }
    return static_cast<uint8_t>(s[0]);
}

std::string BasicStrings::chr(int code) {
    if (code < 0 || code > 255) {
        throw IllegalFunctionCallException();
    }
    return std::string(1, static_cast<char>(static_cast<uint8_t>(code)));
}

std::string BasicStrings::left(const std::string& s, int n) {
    if (n < 0 || n > 255) {
        throw IllegalFunctionCallException();
    }
    if (n == 0) {
        return "";
    }
    if (n >= static_cast<int>(s.size())) {
        return s;
    }
    return s.substr(0, n);
}

std::string BasicStrings::right(const std::string& s, int n) {
    if (n < 0 || n > 255) {
        throw IllegalFunctionCallException();
    }
    if (n == 0) {
        return "";
    }
    if (n >= static_cast<int>(s.size())) {
        return s;
    }
    return s.substr(s.size() - n);
}

std::string BasicStrings::mid(const std::string& s, int start, int length) {
    if (start <= 0 || start > 255 || length < 0 || length > 255) {
        throw IllegalFunctionCallException();
    }
    if (start > static_cast<int>(s.size())) {
        return "";
    }
    size_t offset = static_cast<size_t>(start - 1);
    return s.substr(offset, static_cast<size_t>(length));
}

void BasicStrings::lhs_mid(std::string& target, int start, int length, const std::string& replacement) {
    if (start <= 0 || start > static_cast<int>(target.size()) || length < 0 || length > 255) {
        throw IllegalFunctionCallException();
    }

    size_t offset = static_cast<size_t>(start - 1);
    size_t available_in_target = target.size() - offset;
    size_t count = std::min({static_cast<size_t>(length), replacement.size(), available_in_target});

    for (size_t i = 0; i < count; ++i) {
        target[offset + i] = replacement[i];
    }
}

void BasicStrings::lhs_mid(std::string& target, int start, const std::string& replacement) {
    lhs_mid(target, start, 255, replacement);
}

std::string BasicStrings::concat(const std::string& s1, const std::string& s2) {
    if (s1.size() + s2.size() > MAX_STRING_LENGTH) {
        throw StringTooLongException();
    }
    return s1 + s2;
}

int BasicStrings::compare(const std::string& s1, const std::string& s2) {
    if (s1 == s2) return 0;
    return (s1 < s2) ? -1 : 1;
}

int BasicStrings::instr(int start, const std::string& s1, const std::string& s2) {
    if (start <= 0 || start > 255) {
        throw IllegalFunctionCallException();
    }
    if (start > static_cast<int>(s1.size())) {
        return 0;
    }
    if (s2.empty()) {
        return start;
    }
    if (s1.empty()) {
        return 0;
    }

    size_t pos = s1.find(s2, static_cast<size_t>(start - 1));
    if (pos == std::string::npos) {
        return 0;
    }
    return static_cast<int>(pos + 1);
}

int BasicStrings::instr(const std::string& s1, const std::string& s2) {
    return instr(1, s1, s2);
}

std::string BasicStrings::str(double num) {
    std::ostringstream oss;
    // Positive numbers start with a space in BASIC
    if (num >= 0.0) {
        oss << " ";
    }
    // If it's an integer value, print without trailing decimals
    if (num == std::floor(num) && !std::isinf(num) && std::abs(num) < 1e14) {
        oss << static_cast<int64_t>(num);
    } else {
        oss << num;
    }
    return oss.str();
}

double BasicStrings::val(const std::string& s) {
    // Skip leading whitespaces
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        i++;
    }
    if (i >= s.size()) {
        return 0.0;
    }

    // Check for &H (hex) or &O / & (octal)
    if (s[i] == '&' && i + 1 < s.size()) {
        char prefix = std::toupper(static_cast<unsigned char>(s[i + 1]));
        if (prefix == 'H') {
            i += 2;
            int64_t hex_val = 0;
            while (i < s.size() && std::isxdigit(static_cast<unsigned char>(s[i]))) {
                int digit = 0;
                char c = s[i];
                if (c >= '0' && c <= '9') digit = c - '0';
                else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
                else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
                hex_val = (hex_val << 4) | digit;
                i++;
            }
            return static_cast<double>(hex_val);
        } else if (prefix == 'O' || (s[i+1] >= '0' && s[i+1] <= '7')) {
            if (prefix == 'O') i += 2;
            else i += 1;
            int64_t oct_val = 0;
            while (i < s.size() && s[i] >= '0' && s[i] <= '7') {
                oct_val = (oct_val << 3) | (s[i] - '0');
                i++;
            }
            return static_cast<double>(oct_val);
        }
    }

    // Standard floating-point extraction
    char* end_ptr = nullptr;
    const char* start_ptr = s.c_str() + i;
    double result = std::strtod(start_ptr, &end_ptr);
    if (end_ptr == start_ptr) {
        return 0.0;
    }
    return result;
}

std::string BasicStrings::hex(int64_t num) {
    if (num == 0) return "0";
    // In GW-BASIC 16-bit, values are typically 16-bit signed/unsigned
    uint16_t val16 = static_cast<uint16_t>(num);
    std::ostringstream oss;
    oss << std::uppercase << std::hex << val16;
    return oss.str();
}

std::string BasicStrings::oct(int64_t num) {
    if (num == 0) return "0";
    uint16_t val16 = static_cast<uint16_t>(num);
    std::ostringstream oss;
    oss << std::oct << val16;
    return oss.str();
}

std::string BasicStrings::space(int n) {
    if (n < 0 || n > 255) {
        throw IllegalFunctionCallException();
    }
    return std::string(static_cast<size_t>(n), ' ');
}

std::string BasicStrings::string_repeat(int n, int char_code) {
    if (n < 0 || n > 255 || char_code < 0 || char_code > 255) {
        throw IllegalFunctionCallException();
    }
    return std::string(static_cast<size_t>(n), static_cast<char>(static_cast<uint8_t>(char_code)));
}

std::string BasicStrings::string_repeat(int n, const std::string& s) {
    if (s.empty()) {
        throw IllegalFunctionCallException();
    }
    return string_repeat(n, static_cast<uint8_t>(s[0]));
}

} // namespace gwbasic
