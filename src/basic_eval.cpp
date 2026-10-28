#include "basic_eval.h"
#include "basic_graphics.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <ctime>

namespace gwbasic {

BasicEvaluator::BasicEvaluator(MemoryManager& mem, FileManager& files, ConsoleIO& console, BasicGraphics* gfx)
    : mem_(mem), files_(files), console_(console), gfx_(gfx) {}

double BasicEvaluator::toDouble(const VariableValue& val) {
    if (std::holds_alternative<int16_t>(val)) return static_cast<double>(std::get<int16_t>(val));
    if (std::holds_alternative<float>(val)) return static_cast<double>(std::get<float>(val));
    if (std::holds_alternative<double>(val)) return std::get<double>(val);
    throw TypeMismatchException();
}

int64_t BasicEvaluator::toInt64(const VariableValue& val) {
    return static_cast<int64_t>(toDouble(val));
}

std::string BasicEvaluator::toString(const VariableValue& val) {
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    }
    return BasicStrings::str(toDouble(val));
}

bool BasicEvaluator::isString(const VariableValue& val) {
    return std::holds_alternative<std::string>(val);
}

void BasicEvaluator::skipWhitespace(std::string_view& expr) {
    while (!expr.empty() && std::isspace(static_cast<unsigned char>(expr.front()))) {
        expr.remove_prefix(1);
    }
}

char BasicEvaluator::peekChar(std::string_view& expr) {
    skipWhitespace(expr);
    if (expr.empty()) return '\0';
    return expr.front();
}

char BasicEvaluator::getChar(std::string_view& expr) {
    skipWhitespace(expr);
    if (expr.empty()) return '\0';
    char c = expr.front();
    expr.remove_prefix(1);
    return c;
}

bool BasicEvaluator::matchChar(std::string_view& expr, char expected) {
    if (peekChar(expr) == expected) {
        expr.remove_prefix(1);
        return true;
    }
    return false;
}

bool BasicEvaluator::matchKeyword(std::string_view& expr, const std::string& kw) {
    skipWhitespace(expr);
    if (expr.size() < kw.size()) return false;
    for (size_t i = 0; i < kw.size(); ++i) {
        if (std::toupper(static_cast<unsigned char>(expr[i])) != std::toupper(static_cast<unsigned char>(kw[i]))) {
            return false;
        }
    }
    // Check keyword boundary only if keyword ends with alphanumeric or type suffix
    if (!kw.empty() && (std::isalnum(static_cast<unsigned char>(kw.back())) || kw.back() == '$' || kw.back() == '%' || kw.back() == '#' || kw.back() == '!')) {
        if (expr.size() > kw.size()) {
            char next = expr[kw.size()];
            if (std::isalnum(static_cast<unsigned char>(next)) || next == '$' || next == '%' || next == '#' || next == '!') {
                return false;
            }
        }
    }
    expr.remove_prefix(kw.size());
    return true;
}

VariableValue BasicEvaluator::evaluate(const std::string& expr_str) {
    std::string_view view = expr_str;
    return evaluate(view);
}

VariableValue BasicEvaluator::evaluate(std::string_view& expr) {
    return parseLogicalOr(expr);
}

VariableValue BasicEvaluator::parseLogicalOr(std::string_view& expr) {
    VariableValue left = parseLogicalXor(expr);
    while (matchKeyword(expr, "OR")) {
        VariableValue right = parseLogicalXor(expr);
        int64_t l = toInt64(left);
        int64_t r = toInt64(right);
        left = static_cast<double>(l | r);
    }
    return left;
}

VariableValue BasicEvaluator::parseLogicalXor(std::string_view& expr) {
    VariableValue left = parseLogicalAnd(expr);
    while (matchKeyword(expr, "XOR")) {
        VariableValue right = parseLogicalAnd(expr);
        int64_t l = toInt64(left);
        int64_t r = toInt64(right);
        left = static_cast<double>(l ^ r);
    }
    return left;
}

VariableValue BasicEvaluator::parseLogicalAnd(std::string_view& expr) {
    VariableValue left = parseLogicalNot(expr);
    while (matchKeyword(expr, "AND")) {
        VariableValue right = parseLogicalNot(expr);
        int64_t l = toInt64(left);
        int64_t r = toInt64(right);
        left = static_cast<double>(l & r);
    }
    return left;
}

VariableValue BasicEvaluator::parseLogicalNot(std::string_view& expr) {
    if (matchKeyword(expr, "NOT")) {
        VariableValue val = parseLogicalNot(expr);
        int64_t n = toInt64(val);
        // Bitwise NOT in 16-bit 8086 BASIC
        return static_cast<double>(~n);
    }
    return parseRelational(expr);
}

VariableValue BasicEvaluator::parseRelational(std::string_view& expr) {
    VariableValue left = parseAddSub(expr);

    while (true) {
        skipWhitespace(expr);
        if (expr.empty()) break;

        // In BASIC: True = -1, False = 0
        if (matchKeyword(expr, "<=")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) <= 0)
                                      : (toDouble(left) <= toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else if (matchKeyword(expr, ">=")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) >= 0)
                                      : (toDouble(left) >= toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else if (matchKeyword(expr, "<>") || matchKeyword(expr, "><")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) != 0)
                                      : (toDouble(left) != toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else if (matchKeyword(expr, "=")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) == 0)
                                      : (toDouble(left) == toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else if (matchKeyword(expr, "<")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) < 0)
                                      : (toDouble(left) < toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else if (matchKeyword(expr, ">")) {
            VariableValue right = parseAddSub(expr);
            bool res = isString(left) ? (BasicStrings::compare(toString(left), toString(right)) > 0)
                                      : (toDouble(left) > toDouble(right));
            left = static_cast<double>(res ? -1.0 : 0.0);
        } else {
            break;
        }
    }
    return left;
}

VariableValue BasicEvaluator::parseAddSub(std::string_view& expr) {
    VariableValue left = parseMulDiv(expr);

    while (true) {
        if (matchChar(expr, '+')) {
            VariableValue right = parseMulDiv(expr);
            if (isString(left) && isString(right)) {
                left = BasicStrings::concat(toString(left), toString(right));
            } else if (!isString(left) && !isString(right)) {
                left = toDouble(left) + toDouble(right);
            } else {
                throw TypeMismatchException();
            }
        } else if (matchChar(expr, '-')) {
            VariableValue right = parseMulDiv(expr);
            if (isString(left) || isString(right)) {
                throw TypeMismatchException();
            }
            left = toDouble(left) - toDouble(right);
        } else {
            break;
        }
    }
    return left;
}

VariableValue BasicEvaluator::parseMulDiv(std::string_view& expr) {
    VariableValue left = parsePower(expr);

    while (true) {
        if (matchChar(expr, '*')) {
            VariableValue right = parsePower(expr);
            left = toDouble(left) * toDouble(right);
        } else if (matchChar(expr, '/')) {
            VariableValue right = parsePower(expr);
            double r = toDouble(right);
            if (r == 0.0) throw BasicException(ErrorCode::DivisionByZero, "Division by zero");
            left = toDouble(left) / r;
        } else if (matchChar(expr, '\\')) {
            VariableValue right = parsePower(expr);
            left = static_cast<double>(BasicMath::idiv_fn(toInt64(left), toInt64(right)));
        } else if (matchKeyword(expr, "MOD")) {
            VariableValue right = parsePower(expr);
            left = static_cast<double>(BasicMath::imod_fn(toInt64(left), toInt64(right)));
        } else {
            break;
        }
    }
    return left;
}

VariableValue BasicEvaluator::parsePower(std::string_view& expr) {
    VariableValue left = parseUnary(expr);
    if (matchChar(expr, '^')) {
        VariableValue right = parsePower(expr); // Right-associative
        left = BasicMath::pow_fn(toDouble(left), toDouble(right));
    }
    return left;
}

VariableValue BasicEvaluator::parseUnary(std::string_view& expr) {
    skipWhitespace(expr);
    if (matchChar(expr, '+')) {
        return parseUnary(expr);
    }
    if (matchChar(expr, '-')) {
        VariableValue val = parseUnary(expr);
        return -toDouble(val);
    }
    return parsePrimary(expr);
}

VariableValue BasicEvaluator::parsePrimary(std::string_view& expr) {
    skipWhitespace(expr);
    if (expr.empty()) {
        throw BasicException(ErrorCode::SyntaxError, "Syntax error: unexpected end of expression");
    }

    // Parentheses
    if (matchChar(expr, '(')) {
        VariableValue result = evaluate(expr);
        if (!matchChar(expr, ')')) {
            throw BasicException(ErrorCode::SyntaxError, "Syntax error: missing closing parenthesis");
        }
        return result;
    }

    // String literal "..."
    if (expr.front() == '"') {
        expr.remove_prefix(1);
        std::string str_val;
        while (!expr.empty() && expr.front() != '"') {
            str_val += expr.front();
            expr.remove_prefix(1);
        }
        if (!expr.empty() && expr.front() == '"') {
            expr.remove_prefix(1);
        }
        return str_val;
    }

    // Number literal (supports 123, 3.14, &HFF, &O17, with optional type suffix %, !, #)
    if (std::isdigit(static_cast<unsigned char>(expr.front())) || expr.front() == '.' || expr.front() == '&') {
        size_t len = 0;
        if (expr.front() == '&') {
            len = 1;
            if (len < expr.size() && (std::toupper(static_cast<unsigned char>(expr[len])) == 'H' ||
                                      std::toupper(static_cast<unsigned char>(expr[len])) == 'O')) {
                len++;
            }
            while (len < expr.size() && std::isxdigit(static_cast<unsigned char>(expr[len]))) {
                len++;
            }
        } else {
            while (len < expr.size() && (std::isdigit(static_cast<unsigned char>(expr[len])) || expr[len] == '.' || 
                   expr[len] == 'e' || expr[len] == 'E' || expr[len] == 'd' || expr[len] == 'D')) {
                len++;
            }
        }
        if (len < expr.size() && (expr[len] == '%' || expr[len] == '!' || expr[len] == '#' || expr[len] == '&')) {
            len++;
        }
        std::string num_str(expr.substr(0, len));
        expr.remove_prefix(len);
        return BasicStrings::val(num_str);
    }

    // Identifiers (Keywords, Function calls, or Variables)
    if (std::isalpha(static_cast<unsigned char>(expr.front())) || expr.front() == '_') {
        size_t len = 0;
        while (len < expr.size() && (std::isalnum(static_cast<unsigned char>(expr[len])) || 
               expr[len] == '_' || expr[len] == '.' ||
               expr[len] == '$' || expr[len] == '%' || expr[len] == '#' || expr[len] == '!')) {
            len++;
            // Type suffixes ($ % # !) end identifier
            char last = expr[len - 1];
            if (last == '$' || last == '%' || last == '#' || last == '!') break;
        }

        std::string ident(expr.substr(0, len));
        expr.remove_prefix(len);

        std::string upper_ident = ident;
        std::transform(upper_ident.begin(), upper_ident.end(), upper_ident.begin(), ::toupper);

        // Standalone functions without arguments in BASIC
        if (upper_ident == "INKEY$") {
            return console_.inkey();
        }
        if (upper_ident == "CSRLIN") {
            return static_cast<double>(console_.getCsrlin());
        }
        if (upper_ident == "TIMER") {
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            // GW-BASIC TIMER returns seconds since midnight (with fractional seconds)
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm_struct;
#if defined(_WIN32)
            localtime_s(&tm_struct, &t);
#else
            localtime_r(&t, &tm_struct);
#endif
            double sec_of_day = tm_struct.tm_hour * 3600.0 + tm_struct.tm_min * 60.0 + tm_struct.tm_sec + (millis % 1000) / 1000.0;
            return sec_of_day;
        }

        // Check if followed by '(' -> Array access or Function call
        if (peekChar(expr) == '(') {
            // Check if this name is a known array
            if (mem_.hasArray(ident)) {
                // Parse array indices
                matchChar(expr, '(');
                std::vector<int> indices;
                do {
                    indices.push_back(static_cast<int>(toDouble(evaluate(expr))));
                } while (matchChar(expr, ','));
                matchChar(expr, ')');
                return mem_.getArrayElement(ident, indices);
            }
            return parseFunctionCall(upper_ident, expr);
        }

        // Variable lookup
        return mem_.getVariable(ident);
    }

    throw BasicException(ErrorCode::SyntaxError, "Syntax error in expression");
}

VariableValue BasicEvaluator::parseFunctionCall(const std::string& func, std::string_view& expr) {
    matchChar(expr, '(');

    auto get_arg = [this, &expr]() -> VariableValue {
        return evaluate(expr);
    };

    VariableValue res = 0.0;

    // Transcendentals & Math (Module 2)
    if (func == "ABS") res = BasicMath::abs_fn(toDouble(get_arg()));
    else if (func == "SGN") res = static_cast<double>(BasicMath::sgn_fn(toDouble(get_arg())));
    else if (func == "INT") res = BasicMath::int_fn(toDouble(get_arg()));
    else if (func == "FIX") res = BasicMath::fix_fn(toDouble(get_arg()));
    else if (func == "SQR") res = BasicMath::sqr_fn(toDouble(get_arg()));
    else if (func == "LOG") res = BasicMath::log_fn(toDouble(get_arg()));
    else if (func == "EXP") res = BasicMath::exp_fn(toDouble(get_arg()));
    else if (func == "SIN") res = BasicMath::sin_fn(toDouble(get_arg()));
    else if (func == "COS") res = BasicMath::cos_fn(toDouble(get_arg()));
    else if (func == "TAN") res = BasicMath::tan_fn(toDouble(get_arg()));
    else if (func == "ATN") res = BasicMath::atn_fn(toDouble(get_arg()));
    else if (func == "CINT") res = static_cast<double>(BasicMath::cint_fn(toDouble(get_arg())));
    else if (func == "RND") {
        double arg = 1.0;
        if (peekChar(expr) != ')') arg = toDouble(get_arg());
        res = BasicMath::rnd(arg);
    }
    // String functions (Module 1)
    else if (func == "LEN") res = static_cast<double>(BasicStrings::len(toString(get_arg())));
    else if (func == "ASC") res = static_cast<double>(BasicStrings::asc(toString(get_arg())));
    else if (func == "CHR$") res = BasicStrings::chr(static_cast<int>(toDouble(get_arg())));
    else if (func == "STR$") res = BasicStrings::str(toDouble(get_arg()));
    else if (func == "VAL") res = BasicStrings::val(toString(get_arg()));
    else if (func == "HEX$") res = BasicStrings::hex(toInt64(get_arg()));
    else if (func == "OCT$") res = BasicStrings::oct(toInt64(get_arg()));
    else if (func == "SPACE$") res = BasicStrings::space(static_cast<int>(toDouble(get_arg())));
    else if (func == "LEFT$") {
        std::string s = toString(get_arg());
        matchChar(expr, ',');
        int n = static_cast<int>(toDouble(get_arg()));
        res = BasicStrings::left(s, n);
    }
    else if (func == "RIGHT$") {
        std::string s = toString(get_arg());
        matchChar(expr, ',');
        int n = static_cast<int>(toDouble(get_arg()));
        res = BasicStrings::right(s, n);
    }
    else if (func == "MID$") {
        std::string s = toString(get_arg());
        matchChar(expr, ',');
        int start = static_cast<int>(toDouble(get_arg()));
        int length = 255;
        if (matchChar(expr, ',')) {
            length = static_cast<int>(toDouble(get_arg()));
        }
        res = BasicStrings::mid(s, start, length);
    }
    else if (func == "STRING$") {
        int count = static_cast<int>(toDouble(get_arg()));
        matchChar(expr, ',');
        VariableValue arg2 = get_arg();
        if (isString(arg2)) res = BasicStrings::string_repeat(count, toString(arg2));
        else res = BasicStrings::string_repeat(count, static_cast<int>(toDouble(arg2)));
    }
    else if (func == "INSTR") {
        VariableValue arg1 = get_arg();
        matchChar(expr, ',');
        VariableValue arg2 = get_arg();
        if (matchChar(expr, ',')) {
            // INSTR(start, s1$, s2$)
            VariableValue arg3 = get_arg();
            res = static_cast<double>(BasicStrings::instr(static_cast<int>(toDouble(arg1)), toString(arg2), toString(arg3)));
        } else {
            // INSTR(s1$, s2$)
            res = static_cast<double>(BasicStrings::instr(toString(arg1), toString(arg2)));
        }
    }
    else if (func == "EOF") {
        int filenum = static_cast<int>(toDouble(get_arg()));
        res = static_cast<double>(files_.isEOF(filenum) ? -1.0 : 0.0);
    }
    else if (func == "LOF") {
        int filenum = static_cast<int>(toDouble(get_arg()));
        res = static_cast<double>(files_.getLOF(filenum));
    }
    else if (func == "LOC") {
        int filenum = static_cast<int>(toDouble(get_arg()));
        res = static_cast<double>(files_.getLOC(filenum));
    }
    else if (func == "POS") {
        get_arg(); // Dummy argument
        res = static_cast<double>(console_.getPos());
    }
    else if (func == "PEEK") {
        size_t addr = static_cast<size_t>(toInt64(get_arg()));
        res = static_cast<double>(mem_.peek(addr));
    }
    else if (func == "POINT") {
        int x = static_cast<int>(toDouble(get_arg()));
        matchChar(expr, ',');
        int y = static_cast<int>(toDouble(get_arg()));
        res = static_cast<double>(gfx_ ? gfx_->point(x, y) : 0);
    }
    else if (func == "TAB") {
        int col = static_cast<int>(toDouble(get_arg()));
        // TAB(n) → return enough spaces to reach column n (simplified: just n spaces)
        if (col < 1) col = 1;
        res = std::string(static_cast<size_t>(col - 1), ' ');
    }
    else if (func == "SPC") {
        int n = static_cast<int>(toDouble(get_arg()));
        if (n < 0) n = 0;
        res = std::string(static_cast<size_t>(n), ' ');
    }
    else if (func == "FRE") {
        VariableValue arg = get_arg();
        if (isString(arg)) res = static_cast<double>(mem_.getFreeStringMemory());
        else res = static_cast<double>(mem_.getFreeNumericMemory());
    }
    else {
        throw BasicException(ErrorCode::SyntaxError, "Unknown function: " + func);
    }

    if (!matchChar(expr, ')')) {
        throw BasicException(ErrorCode::SyntaxError, "Missing closing ')' in function call");
    }
    return res;
}

} // namespace gwbasic
