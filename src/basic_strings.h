#pragma once

#include "basic_types.h"
#include <string>
#include <string_view>
#include <cstdint>

namespace gwbasic {

/**
 * C++ implementation of GW-BASIC string functions (originally in BISTRS.ASM)
 * All operations conform to 8086 GW-BASIC semantics (1-indexed, max length 255, FCERR on bounds).
 */
class BasicStrings {
public:
    // LEN(s$): Returns string length (0..255)
    static int len(const std::string& s);

    // ASC(s$): Returns ASCII code of the first character. Throws IllegalFunctionCallException if empty.
    static int asc(const std::string& s);

    // CHR$(n): Returns 1-character string from ASCII code (0..255).
    static std::string chr(int code);

    // LEFT$(s$, n): Returns the leftmost n characters of s$.
    static std::string left(const std::string& s, int n);

    // RIGHT$(s$, n): Returns the rightmost n characters of s$.
    static std::string right(const std::string& s, int n);

    // MID$(s$, start [, length]): Returns substring starting at 1-based index `start`.
    // If length is omitted, default is up to 255 characters (rest of string).
    static std::string mid(const std::string& s, int start, int length = 255);

    // LHS MID$: Statement MID$(s$, start [, length]) = replacement$
    // Replaces in-place characters inside target string. Length of target string is NEVER changed.
    static void lhs_mid(std::string& target, int start, int length, const std::string& replacement);
    static void lhs_mid(std::string& target, int start, const std::string& replacement);

    // Concatenation: s1$ + s2$. Throws StringTooLongException if result > 255 characters.
    static std::string concat(const std::string& s1, const std::string& s2);

    // STRCMP: Compares two strings. Returns:
    //  0 if equal
    // -1 if s1 < s2
    // +1 if s1 > s2
    static int compare(const std::string& s1, const std::string& s2);

    // INSTR([start,] s1$, s2$): Searches for s2$ inside s1$ starting from 1-based position `start`.
    // Returns 1-based index of first match, or 0 if not found.
    static int instr(int start, const std::string& s1, const std::string& s2);
    static int instr(const std::string& s1, const std::string& s2);

    // STR$(n): Converts number to string with a leading space for non-negative numbers, or '-' for negative.
    static std::string str(double num);

    // VAL(s$): Parses floating-point or integer value from string.
    static double val(const std::string& s);

    // HEX$(n): Returns uppercase hexadecimal string representation.
    static std::string hex(int64_t num);

    // OCT$(n): Returns octal string representation.
    static std::string oct(int64_t num);

    // SPACE$(n): Returns string with n spaces.
    static std::string space(int n);

    // STRING$(n, char_code): Returns string with character repeated n times.
    static std::string string_repeat(int n, int char_code);
    static std::string string_repeat(int n, const std::string& s);
};

} // namespace gwbasic
