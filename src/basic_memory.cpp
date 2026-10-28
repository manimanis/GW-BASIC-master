#include "basic_memory.h"
#include <algorithm>

namespace gwbasic {

SoftKeys::SoftKeys() {
    resetDefaults();
}

void SoftKeys::resetDefaults() {
    // Exact defaults from GWRAM.ASM (lines 116-147)
    keys_[0] = "LIST ";
    keys_[1] = "RUN\r";
    keys_[2] = "LOAD\"";
    keys_[3] = "SAVE\"";
    keys_[4] = "CONT\r";
    keys_[5] = ",\"LPT1:\"";
    keys_[6] = "TRON\r";
    keys_[7] = "TROFF\r";
    keys_[8] = "KEY ";
    keys_[9] = "SCREEN 0,0,0\r";
}

const std::string& SoftKeys::getKey(size_t index) const {
    if (index >= NUM_KEYS) {
        throw IllegalFunctionCallException();
    }
    return keys_[index];
}

void SoftKeys::setKey(size_t index, const std::string& text) {
    if (index >= NUM_KEYS || text.size() > 15) {
        throw IllegalFunctionCallException();
    }
    keys_[index] = text;
}

MemoryManager::MemoryManager(size_t total_memory)
    : total_memory_(total_memory), ram_(total_memory, 0) {}

uint8_t MemoryManager::peek(size_t addr) const {
    if (addr < ram_.size()) {
        return ram_[addr];
    }
    return 0;
}

void MemoryManager::poke(size_t addr, uint8_t val) {
    if (addr < ram_.size()) {
        ram_[addr] = val;
    }
}

void MemoryManager::setVariable(const std::string& name, const VariableValue& val) {
    // GW-BASIC variable names are case-insensitive
    std::string upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
    variables_[upper_name] = val;
}

VariableValue MemoryManager::getVariable(const std::string& name) const {
    std::string upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
    auto it = variables_.find(upper_name);
    if (it != variables_.end()) {
        return it->second;
    }
    // Default value if not set
    if (!name.empty() && name.back() == '$') {
        return std::string("");
    } else if (!name.empty() && name.back() == '%') {
        return static_cast<int16_t>(0);
    } else if (!name.empty() && name.back() == '#') {
        return static_cast<double>(0.0);
    }
    return static_cast<float>(0.0f);
}

bool MemoryManager::hasVariable(const std::string& name) const {
    std::string upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
    return variables_.find(upper_name) != variables_.end();
}

void MemoryManager::clearVariables() {
    variables_.clear();
    arrays_.clear();
}

// ---- Array management ----

static std::string upperName(const std::string& name) {
    std::string u = name;
    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
    return u;
}

void MemoryManager::dimArray(const std::string& name, const std::vector<int>& dims) {
    std::string uname = upperName(name);
    ArrayDim arr;
    // GW-BASIC arrays: DIM A(10) means indices 0..10 → size = max+1
    for (int d : dims) arr.dimensions.push_back(d + 1);
    arr.is_string = (!uname.empty() && uname.back() == '$');
    size_t total = arr.totalElements();
    if (arr.is_string) arr.str_data.assign(total, "");
    else               arr.num_data.assign(total, 0.0);
    arrays_[uname] = std::move(arr);
}

bool MemoryManager::hasArray(const std::string& name) const {
    return arrays_.find(upperName(name)) != arrays_.end();
}

void MemoryManager::setArrayElement(const std::string& name, const std::vector<int>& indices, const VariableValue& val) {
    std::string uname = upperName(name);
    // Auto-DIM with default size 10 if not explicitly DIMmed
    if (arrays_.find(uname) == arrays_.end()) {
        std::vector<int> default_dims(indices.size(), 10);
        dimArray(name, default_dims);
    }
    ArrayDim& arr = arrays_.at(uname);
    size_t idx = arr.computeIndex(indices);
    if (arr.is_string) {
        if (idx < arr.str_data.size())
            arr.str_data[idx] = std::holds_alternative<std::string>(val) ? std::get<std::string>(val) : "";
    } else {
        if (idx < arr.num_data.size()) {
            if (std::holds_alternative<double>(val)) arr.num_data[idx] = std::get<double>(val);
            else if (std::holds_alternative<float>(val)) arr.num_data[idx] = std::get<float>(val);
            else if (std::holds_alternative<int16_t>(val)) arr.num_data[idx] = std::get<int16_t>(val);
        }
    }
}

VariableValue MemoryManager::getArrayElement(const std::string& name, const std::vector<int>& indices) const {
    std::string uname = upperName(name);
    auto it = arrays_.find(uname);
    if (it == arrays_.end()) {
        // Auto-DIM: return default 0 / ""
        if (!uname.empty() && uname.back() == '$') return std::string("");
        return 0.0;
    }
    const ArrayDim& arr = it->second;
    size_t idx = arr.computeIndex(indices);
    if (arr.is_string) {
        if (idx < arr.str_data.size()) return arr.str_data[idx];
        return std::string("");
    } else {
        if (idx < arr.num_data.size()) return arr.num_data[idx];
        return 0.0;
    }
}



size_t MemoryManager::getFreeNumericMemory() const {
    // Calculate simulated free bytes
    size_t used = variables_.size() * 16;
    return (used < total_memory_) ? (total_memory_ - used) : 0;
}

size_t MemoryManager::getFreeStringMemory() const {
    size_t string_bytes = 0;
    for (const auto& [k, v] : variables_) {
        if (std::holds_alternative<std::string>(v)) {
            string_bytes += std::get<std::string>(v).size() + 3; // 3 bytes descriptor
        }
    }
    return (string_bytes < total_memory_) ? (total_memory_ - string_bytes) : 0;
}

void MemoryManager::garbageCollectStrings() {
    // In C++, std::string is managed automatically.
    // In GW-BASIC, this cleans up dead temporary descriptors.
}

void MemoryManager::setMemoryLimit(size_t bytes) {
    total_memory_ = bytes;
}

} // namespace gwbasic
