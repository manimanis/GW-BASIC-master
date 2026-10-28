#pragma once

#include "basic_types.h"
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <variant>
#include <cstdint>

namespace gwbasic {

// Variable value variant representing GW-BASIC primitive types
using VariableValue = std::variant<int16_t, float, double, std::string>;

// Softkeys (F1 - F10) definitions from GWRAM.ASM
class SoftKeys {
public:
    static constexpr size_t NUM_KEYS = 10;
    SoftKeys();

    const std::string& getKey(size_t index) const;
    void setKey(size_t index, const std::string& text);
    void resetDefaults();

private:
    std::array<std::string, NUM_KEYS> keys_;
};

// Array dimension descriptor
struct ArrayDim {
    std::vector<int> dimensions; // sizes per dimension (1-indexed, size = max_index+1)
    std::vector<double> num_data;
    std::vector<std::string> str_data;
    bool is_string = false;

    size_t computeIndex(const std::vector<int>& indices) const {
        size_t idx = 0;
        size_t stride = 1;
        for (int d = static_cast<int>(dimensions.size()) - 1; d >= 0; --d) {
            idx += indices[static_cast<size_t>(d)] * stride;
            stride *= static_cast<size_t>(dimensions[static_cast<size_t>(d)]);
        }
        return idx;
    }

    size_t totalElements() const {
        size_t total = 1;
        for (int d : dimensions) total *= static_cast<size_t>(d);
        return total;
    }
};

// Memory model representing GWRAM.ASM and memory layout
class MemoryManager {
public:
    // Default 64KB memory space typical for 8086 GW-BASIC
    static constexpr size_t DEFAULT_MEMORY_SIZE = 65536;

    explicit MemoryManager(size_t total_memory = DEFAULT_MEMORY_SIZE);

    // Variable management
    void setVariable(const std::string& name, const VariableValue& val);
    VariableValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void clearVariables(); // CLEAR statement

    // Array management (DIM statement)
    void dimArray(const std::string& name, const std::vector<int>& dims);
    bool hasArray(const std::string& name) const;
    void setArrayElement(const std::string& name, const std::vector<int>& indices, const VariableValue& val);
    VariableValue getArrayElement(const std::string& name, const std::vector<int>& indices) const;

    // FRE(0) and FRE("") functions
    size_t getFreeNumericMemory() const;
    size_t getFreeStringMemory() const;
    void garbageCollectStrings();

    // Low-level memory access (PEEK / POKE statements from BIMISC.ASM / GWRAM.ASM)
    uint8_t peek(size_t addr) const;
    void poke(size_t addr, uint8_t val);

    // Memory boundaries
    size_t getTotalMemory() const { return total_memory_; }
    void setMemoryLimit(size_t bytes);

    SoftKeys& softKeys() { return soft_keys_; }
    const SoftKeys& softKeys() const { return soft_keys_; }

private:
    size_t total_memory_;
    SoftKeys soft_keys_;
    std::unordered_map<std::string, VariableValue> variables_;
    std::unordered_map<std::string, ArrayDim> arrays_;
    std::vector<uint8_t> ram_;
};

} // namespace gwbasic
