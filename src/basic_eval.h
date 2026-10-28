#pragma once

#include "basic_types.h"
#include "basic_memory.h"
#include "basic_math.h"
#include "basic_strings.h"
#include <string>
#include <string_view>
#include <variant>

#include "basic_io.h"

namespace gwbasic {

class BasicGraphics;

class BasicEvaluator {
public:
    BasicEvaluator(MemoryManager& mem, FileManager& files, ConsoleIO& console, BasicGraphics* gfx = nullptr);

    // Evaluates a full expression from a string view, advancing the cursor
    VariableValue evaluate(std::string_view& expr);
    VariableValue evaluate(const std::string& expr_str);

    // Helper conversions
    static double toDouble(const VariableValue& val);
    static int64_t toInt64(const VariableValue& val);
    static std::string toString(const VariableValue& val);
    static bool isString(const VariableValue& val);

private:
    MemoryManager& mem_;
    FileManager& files_;
    ConsoleIO& console_;
    BasicGraphics* gfx_;

    void skipWhitespace(std::string_view& expr);
    char peekChar(std::string_view& expr);
    char getChar(std::string_view& expr);
    bool matchChar(std::string_view& expr, char expected);
    bool matchKeyword(std::string_view& expr, const std::string& kw);

    // Recursive Descent Parser according to GW-BASIC precedence
    VariableValue parseLogicalOr(std::string_view& expr);
    VariableValue parseLogicalXor(std::string_view& expr);
    VariableValue parseLogicalAnd(std::string_view& expr);
    VariableValue parseLogicalNot(std::string_view& expr);
    VariableValue parseRelational(std::string_view& expr);
    VariableValue parseAddSub(std::string_view& expr);
    VariableValue parseMulDiv(std::string_view& expr);
    VariableValue parsePower(std::string_view& expr);
    VariableValue parseUnary(std::string_view& expr);
    VariableValue parsePrimary(std::string_view& expr);

    VariableValue parseFunctionCall(const std::string& func_name, std::string_view& expr);
};

} // namespace gwbasic
