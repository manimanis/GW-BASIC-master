#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <string_view>

namespace gwbasic {

// GW-BASIC Error Codes (from BINTRP.H / OEM.H)
enum class ErrorCode {
    None = 0,
    SyntaxError = 2,              // SNERR
    ReturnWithoutGosub = 3,       // RGERR
    OutOfData = 4,                // ODERR
    IllegalFunctionCall = 5,      // FCERR
    Overflow = 6,                 // OVERR
    OutOfMemory = 7,              // OMERR
    UndefinedStatement = 8,       // USERR
    SubscriptOutOfRange = 9,      // BSERR
    DuplicateDefinition = 10,     // DDERR
    DivisionByZero = 11,          // DVOER
    IllegalDirect = 12,           // IDERR
    TypeMismatch = 13,            // TMERR
    OutOfStringSpace = 14,        // OSERR
    StringTooLong = 15,           // ERRLS ("String formula too complex" / "String too long")
    StringFormulaTooComplex = 16, // STERR
    CantContinue = 17,            // CNERR
    UndefinedUserFunction = 18,   // UFERR
    BadFileNumber = 52,           // BFNERR
    FileNotFound = 53,            // FFNERR
    BadFileName = 64              // BFMERR
};

// Custom exceptions for BASIC runtime
class BasicException : public std::runtime_error {
public:
    ErrorCode code;
    explicit BasicException(ErrorCode c, const std::string& msg) 
        : std::runtime_error(msg), code(c) {}
};

class IllegalFunctionCallException : public BasicException {
public:
    IllegalFunctionCallException() 
        : BasicException(ErrorCode::IllegalFunctionCall, "Illegal function call") {}
};

class StringTooLongException : public BasicException {
public:
    StringTooLongException() 
        : BasicException(ErrorCode::StringTooLong, "String too long (max 255 characters)") {}
};

class TypeMismatchException : public BasicException {
public:
    TypeMismatchException() 
        : BasicException(ErrorCode::TypeMismatch, "Type mismatch") {}
};

class OutOfStringSpaceException : public BasicException {
public:
    OutOfStringSpaceException() 
        : BasicException(ErrorCode::OutOfStringSpace, "Out of string space") {}
};

// Maximum length of a GW-BASIC string descriptor (1 byte length: 0 to 255)
constexpr size_t MAX_STRING_LENGTH = 255;

} // namespace gwbasic
