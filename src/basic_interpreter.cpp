#include "basic_interpreter.h"
#include <cctype>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <iomanip>

namespace gwbasic {

BasicInterpreter::BasicInterpreter()
    : eval_(mem_, files_, console_, &gfx_) {}

void BasicInterpreter::addLine(int line_number, const std::string& text) {
    if (line_number <= 0) {
        throw BasicException(ErrorCode::UndefinedStatement, "Invalid line number");
    }
    program_[line_number] = text;
}

void BasicInterpreter::deleteLine(int line_number) {
    program_.erase(line_number);
}

void BasicInterpreter::newProgram() {
    program_.clear();
    mem_.clearVariables();
    files_.closeAll();
    gosub_stack_.clear();
    for_stack_.clear();
    while_stack_.clear();
    data_items_.clear();
    data_ptr_ = 0;
    clearOutput();
}

std::string BasicInterpreter::listProgram() const {
    std::ostringstream oss;
    for (const auto& [num, text] : program_) {
        oss << num << " " << text << "\n";
    }
    return oss.str();
}

std::string BasicInterpreter::listProgram(int start_line, int end_line) const {
    std::ostringstream oss;
    for (const auto& [num, text] : program_) {
        if (num < start_line) continue;
        if (num > end_line) break;
        oss << num << " " << text << "\n";
    }
    return oss.str();
}

void BasicInterpreter::run() {
    mem_.clearVariables();
    gosub_stack_.clear();
    for_stack_.clear();
    while_stack_.clear();
    rebuildDataPool();
    executeProgram();
}

void BasicInterpreter::run(int start_line) {
    mem_.clearVariables();
    gosub_stack_.clear();
    for_stack_.clear();
    while_stack_.clear();
    rebuildDataPool();

    if (program_.empty()) return;

    // Find the starting line (at or after start_line)
    auto it = program_.lower_bound(start_line);
    if (it == program_.end()) {
        throw BasicException(ErrorCode::UndefinedStatement,
            "Undefined line number " + std::to_string(start_line));
    }

    current_line_ = it->first;
    current_stmt_idx_ = 0;
    is_running_ = true;

    while (is_running_ && it != program_.end()) {
        auto statements = splitStatements(it->second);
        int line_num = it->first;

        while (is_running_ && current_stmt_idx_ < statements.size()) {
            size_t this_stmt = current_stmt_idx_;
            std::string_view stmt = statements[this_stmt];
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

            if (stmt.empty()) {
                current_stmt_idx_++;
                continue;
            }

            bool branch_taken = false;
            try {
                executeSingleStatement(stmt, branch_taken);
            } catch (const BasicException& e) {
                if (error_handler_line_ != 0 && program_.find(error_handler_line_) != program_.end()) {
                    current_line_ = error_handler_line_;
                    current_stmt_idx_ = 0;
                    it = program_.lower_bound(current_line_);
                    break;
                }
                throw;
            }

            if (!is_running_) break;

            if (current_line_ != line_num) {
                it = program_.lower_bound(current_line_);
                break;
            }

            if (current_stmt_idx_ != this_stmt) {
                continue;
            }

            if (branch_taken) {
                current_stmt_idx_ = statements.size();
                break;
            }

            current_stmt_idx_++;
        }

        if (!is_running_) break;

        if (it != program_.end() && it->first == line_num && current_stmt_idx_ >= statements.size()) {
            ++it;
            if (it != program_.end()) {
                current_line_ = it->first;
                current_stmt_idx_ = 0;
            }
        }
    }

    is_running_ = false;
}

void BasicInterpreter::loadProgram(const std::string& filename, bool run_after) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw BasicException(ErrorCode::FileNotFound,
            "File not found: " + filename);
    }

    // Clear current program before loading
    newProgram();

    std::string line;
    while (std::getline(file, line)) {
        // Strip trailing \r if present (Windows line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;

        if (std::isdigit(static_cast<unsigned char>(line[start]))) {
            size_t space_pos = line.find_first_of(" \t", start);
            int line_num = std::stoi(line.substr(start, space_pos - start));
            std::string code = (space_pos != std::string::npos) ? line.substr(space_pos + 1) : "";
            addLine(line_num, code);
        }
        // Lines without line numbers are silently skipped (consistent with GW-BASIC)
    }

    file.close();

    if (run_after) {
        run();
    }
}

void BasicInterpreter::saveProgram(const std::string& filename, bool ascii_format) {
    if (program_.empty()) {
        throw BasicException(ErrorCode::IllegalFunctionCall,
            "No program in memory");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw BasicException(ErrorCode::FileNotFound,
            "Cannot create file: " + filename);
    }

    // GW-BASIC SAVE with ,A option saves in ASCII (text) format
    // We always save in ASCII/text format since we don't implement tokenized format
    for (const auto& [num, text] : program_) {
        file << num << " " << text << "\n";
    }

    file.close();
}

void BasicInterpreter::executeProgram() {
    if (program_.empty()) return;

    auto it = program_.begin();
    current_line_ = it->first;
    current_stmt_idx_ = 0;
    is_running_ = true;

    while (is_running_ && it != program_.end()) {
        auto statements = splitStatements(it->second);
        int line_num = it->first;

        while (is_running_ && current_stmt_idx_ < statements.size()) {
            size_t this_stmt = current_stmt_idx_;
            std::string_view stmt = statements[this_stmt];
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

            if (stmt.empty()) {
                current_stmt_idx_++;
                continue;
            }

            bool branch_taken = false;
            try {
                executeSingleStatement(stmt, branch_taken);
            } catch (const BasicException& e) {
                if (error_handler_line_ != 0 && program_.find(error_handler_line_) != program_.end()) {
                    current_line_ = error_handler_line_;
                    current_stmt_idx_ = 0;
                    it = program_.lower_bound(current_line_);
                    break;
                }
                throw;
            }

            if (!is_running_) break;

            // Did the statement jump to a different line?
            if (current_line_ != line_num) {
                it = program_.lower_bound(current_line_);
                break;
            }

            // Did the statement jump to a different statement on the same line?
            if (current_stmt_idx_ != this_stmt) {
                continue;
            }

            // If branch_taken was set (e.g. IF condition false without ELSE, or REM):
            if (branch_taken) {
                current_stmt_idx_ = statements.size();
                break;
            }

            current_stmt_idx_++;
        }

        if (!is_running_) break;

        // Advance to next line if we finished all statements on this line without jumping
        if (it != program_.end() && it->first == line_num && current_stmt_idx_ >= statements.size()) {
            ++it;
            if (it != program_.end()) {
                current_line_ = it->first;
                current_stmt_idx_ = 0;
            }
        }
    }
    is_running_ = false;
}

void BasicInterpreter::executeStatement(const std::string& statement) {
    std::string_view stmt = statement;
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (stmt.empty()) return;

    bool was_running = is_running_;
    is_running_ = true;
    executeLine(std::string(stmt));
    is_running_ = was_running;
}

std::vector<std::string> BasicInterpreter::splitStatements(std::string_view line) {
    std::vector<std::string> stmts;
    std::string current;
    bool in_quote = false;

    // If entire line starts with REM or ', return as single statement
    std::string_view trimmed = line;
    while (!trimmed.empty() && std::isspace(static_cast<unsigned char>(trimmed.front()))) trimmed.remove_prefix(1);
    if (!trimmed.empty() && trimmed.front() == '\'') {
        stmts.push_back(std::string(line));
        return stmts;
    }
    if (trimmed.size() >= 3) {
        std::string prefix(trimmed.substr(0, 3));
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
        if (prefix == "REM" && (trimmed.size() == 3 || std::isspace(static_cast<unsigned char>(trimmed[3])))) {
            stmts.push_back(std::string(line));
            return stmts;
        }
    }

    auto is_if_statement = [](const std::string& s) -> bool {
        std::string_view sv = s;
        while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
        if (sv.size() >= 2) {
            char c0 = std::toupper(static_cast<unsigned char>(sv[0]));
            char c1 = std::toupper(static_cast<unsigned char>(sv[1]));
            if (c0 == 'I' && c1 == 'F') {
                if (sv.size() == 2 || std::isspace(static_cast<unsigned char>(sv[2])) || sv[2] == '(') {
                    return true;
                }
            }
        }
        return false;
    };

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            in_quote = !in_quote;
            current += c;
        } else if (c == '\'' && !in_quote) {
            // In GW-BASIC, an unquoted ' introduces a comment until end of line
            while (!current.empty() && std::isspace(static_cast<unsigned char>(current.back()))) {
                current.pop_back();
            }
            if (!current.empty()) {
                stmts.push_back(current);
                current.clear();
            }
            stmts.push_back(std::string(line.substr(i)));
            return stmts;
        } else if (c == ':' && !in_quote) {
            if (is_if_statement(current)) {
                // In GW-BASIC, colons after IF belong to the IF statement's THEN/ELSE clauses
                current += c;
            } else {
                stmts.push_back(current);
                current.clear();
                // If remainder of line is REM or ', treat entire remainder as single comment
                std::string_view rem_check(line.data() + i + 1, line.size() - i - 1);
                while (!rem_check.empty() && std::isspace(static_cast<unsigned char>(rem_check.front()))) rem_check.remove_prefix(1);
                if (!rem_check.empty() && rem_check.front() == '\'') {
                    stmts.push_back(std::string(line.substr(i + 1)));
                    return stmts;
                }
                if (rem_check.size() >= 3) {
                    std::string p(rem_check.substr(0, 3));
                    std::transform(p.begin(), p.end(), p.begin(), ::toupper);
                    if (p == "REM" && (rem_check.size() == 3 || std::isspace(static_cast<unsigned char>(rem_check[3])))) {
                        stmts.push_back(std::string(line.substr(i + 1)));
                        return stmts;
                    }
                }
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        stmts.push_back(current);
    }
    return stmts;
}

void BasicInterpreter::executeLine(const std::string& line_text) {
    auto statements = splitStatements(line_text);
    int starting_line = current_line_;
    current_stmt_idx_ = 0;

    while (is_running_ && current_stmt_idx_ < statements.size()) {
        size_t this_stmt = current_stmt_idx_;
        std::string_view stmt = statements[this_stmt];
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (stmt.empty()) {
            current_stmt_idx_++;
            continue;
        }

        bool branch_taken = false;
        executeSingleStatement(stmt, branch_taken);

        if (!is_running_) break;
        if (current_line_ != starting_line) break;
        if (current_stmt_idx_ != this_stmt) continue;
        if (branch_taken) break;
        current_stmt_idx_++;
    }
}

void BasicInterpreter::executeSingleStatement(std::string_view stmt, bool& branch_taken) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (stmt.empty()) return;

    // Comments REM or '
    if (stmt.size() >= 3) {
        std::string prefix(stmt.substr(0, 3));
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
        if (prefix == "REM") {
            branch_taken = true;
            return;
        }
    }
    if (stmt.front() == '\'') {
        branch_taken = true;
        return;
    }

    // Dispatch keywords
    auto match_kw = [&stmt](const std::string& kw) -> bool {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (stmt.size() < kw.size()) return false;
        for (size_t i = 0; i < kw.size(); ++i) {
            if (std::toupper(static_cast<unsigned char>(stmt[i])) != std::toupper(static_cast<unsigned char>(kw[i]))) return false;
        }
        if (stmt.size() > kw.size() && std::isalpha(static_cast<unsigned char>(kw.back()))) {
            char next = stmt[kw.size()];
            if (std::isalnum(static_cast<unsigned char>(next)) || next == '_' || next == '$' || next == '%' || next == '#' || next == '!') {
                return false;
            }
        }
        stmt.remove_prefix(kw.size());
        return true;
    };

    if (match_kw("PRINT") || match_kw("?")) {
        handlePrint(stmt);
    } else if (match_kw("OPEN")) {
        handleOpen(stmt);
    } else if (match_kw("CLOSE")) {
        handleClose(stmt);
    } else if (match_kw("LINE INPUT")) {
        handleLineInput(stmt);
    } else if (match_kw("INPUT")) {
        handleInput(stmt);
    } else if (match_kw("LOCATE")) {
        handleLocate(stmt);
    } else if (match_kw("COLOR")) {
        handleColor(stmt);
    } else if (match_kw("WIDTH")) {
        handleWidth(stmt);
    } else if (match_kw("SAVE_SCREEN") || match_kw("BSAVE")) {
        handleSaveScreen(stmt);
    } else if (match_kw("LET")) {
        handleLet(stmt);
    } else if (match_kw("GOTO")) {
        handleGoto(stmt);
        branch_taken = true;
    } else if (match_kw("GOSUB")) {
        handleGosub(stmt);
        branch_taken = true;
    } else if (match_kw("RETURN")) {
        handleReturn();
        branch_taken = true;
    } else if (match_kw("IF")) {
        handleIf(stmt, branch_taken);
    } else if (match_kw("FOR")) {
        handleFor(stmt);
    } else if (match_kw("NEXT")) {
        handleNext(stmt);
    } else if (match_kw("WHILE")) {
        handleWhile(stmt, branch_taken);
    } else if (match_kw("WEND")) {
        handleWend();
        branch_taken = true;
    } else if (match_kw("DATA")) {
        // DATA is declarative; skip at runtime
    } else if (match_kw("READ")) {
        handleRead(stmt);
    } else if (match_kw("RESTORE")) {
        handleRestore(stmt);
    } else if (match_kw("SWAP")) {
        handleSwap(stmt);
    } else if (match_kw("RANDOMIZE")) {
        handleRandomize(stmt);
    } else if (match_kw("POKE")) {
        handlePoke(stmt);
    } else if (match_kw("BEEP")) {
        handleBeep();
    } else if (match_kw("SOUND")) {
        handleSound(stmt);
    } else if (match_kw("ON")) {
        handleOn(stmt);
        branch_taken = true;
    } else if (match_kw("DIM")) {
        handleDim(stmt);
    } else if (match_kw("ON ERROR GOTO")) {
        // Parse target line for error handler
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty()) {
            std::string ln(stmt);
            int target = std::stoi(ln);
            error_handler_line_ = target; // 0 = disable handler
        }
    } else if (match_kw("CLS")) {
        gfx_.cls(0);
    } else if (match_kw("SCREEN")) {
        // SCREEN mode[, colorswitch[, apage[, vpage]]]
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty()) {
            int mode = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
            if (mode == 0) gfx_.setMode(ScreenMode::Text);
            else if (mode == 1) gfx_.setMode(ScreenMode::MediumRes);
            else if (mode == 2) gfx_.setMode(ScreenMode::HighRes);
        }
    } else if (match_kw("PRESET")) {
        // PRESET (x,y)[,color] - draws in background color (color 0)
        handlePset(stmt, true);
    } else if (match_kw("PSET")) {
        handlePset(stmt, false);
    } else if (match_kw("LINE")) {
        handleLine(stmt);
    } else if (match_kw("CIRCLE")) {
        handleCircle(stmt);
    } else if (match_kw("PAINT")) {
        handlePaint(stmt);
    } else if (match_kw("DRAW")) {
        handleDraw(stmt);
    } else if (match_kw("CLEAR")) {
        mem_.clearVariables();
    } else if (match_kw("ERROR")) {
        int code = 1;
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty()) {
            code = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
        throw BasicException(ErrorCode::SyntaxError, "User error " + std::to_string(code));
    } else if (match_kw("END") || match_kw("STOP")) {
        is_running_ = false;
        branch_taken = true;
    } else {
        // Implicit LET assignment (e.g. X = 10 or A$ = "HELLO" or MID$(A$, 1, 2) = "MS")
        handleLet(stmt);
    }

    if (step_hook_) {
        step_hook_();
    }
}

void BasicInterpreter::handlePrint(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    int file_num = 0;
    if (!stmt.empty() && stmt.front() == '#') {
        stmt.remove_prefix(1);
        file_num = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
            stmt.remove_prefix(1);
        }
    }

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    // Check for USING
    bool is_using = false;
    std::string format_str;
    size_t fmt_idx = 0;
    if (stmt.size() >= 5) {
        std::string prefix(stmt.substr(0, 5));
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
        if (prefix == "USING") {
            stmt.remove_prefix(5);
            is_using = true;
            format_str = BasicEvaluator::toString(eval_.evaluate(stmt));
            while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ';')) {
                stmt.remove_prefix(1);
            }
        }
    }

    std::ostringstream line_out;
    bool trailing_separator = false;

    while (true) {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (stmt.empty()) break;

        if (stmt.front() == ';') {
            stmt.remove_prefix(1);
            trailing_separator = true;
            continue;
        }
        if (stmt.front() == ',') {
            stmt.remove_prefix(1);
            if (!is_using) line_out << "\t";
            trailing_separator = true;
            continue;
        }

        VariableValue val = eval_.evaluate(stmt);
        if (is_using) {
            // Advance through format_str printing literals until next specifier
            auto is_spec_char = [](const std::string& fmt, size_t idx) -> bool {
                char c = fmt[idx];
                if (c == '+' && idx + 1 < fmt.size() && (fmt[idx+1] == '#' || fmt[idx+1] == '$' || fmt[idx+1] == '*')) return true;
                return c == '#' || c == '!' || c == '&' || c == '\\' || c == '$' || c == '*';
            };

            while (fmt_idx < format_str.size() && !is_spec_char(format_str, fmt_idx)) {
                if (format_str[fmt_idx] == '_') {
                    fmt_idx++;
                    if (fmt_idx < format_str.size()) {
                        line_out << format_str[fmt_idx++];
                    }
                } else {
                    line_out << format_str[fmt_idx++];
                }
            }

            if (fmt_idx >= format_str.size()) {
                fmt_idx = 0;
                while (fmt_idx < format_str.size() && !is_spec_char(format_str, fmt_idx)) {
                    line_out << format_str[fmt_idx++];
                }
            }

            if (fmt_idx < format_str.size()) {
                if (BasicEvaluator::isString(val)) {
                    std::string s = BasicEvaluator::toString(val);
                    if (format_str[fmt_idx] == '!') {
                        line_out << (s.empty() ? ' ' : s[0]);
                        fmt_idx++;
                    } else if (format_str[fmt_idx] == '&') {
                        line_out << s;
                        fmt_idx++;
                    } else if (format_str[fmt_idx] == '\\') {
                        size_t end_slash = format_str.find('\\', fmt_idx + 1);
                        size_t width = (end_slash != std::string::npos) ? (end_slash - fmt_idx + 1) : 2;
                        fmt_idx = (end_slash != std::string::npos) ? (end_slash + 1) : (fmt_idx + 1);
                        if (s.size() >= width) {
                            line_out << s.substr(0, width);
                        } else {
                            line_out << s << std::string(width - s.size(), ' ');
                        }
                    } else {
                        line_out << s;
                        fmt_idx++;
                    }
                } else {
                    double num = BasicEvaluator::toDouble(val);
                    bool force_sign = false;
                    if (format_str[fmt_idx] == '+') {
                        force_sign = true;
                        fmt_idx++;
                    }
                    bool has_dollar = false;
                    bool has_asterisk = false;
                    while (fmt_idx < format_str.size() && (format_str[fmt_idx] == '$' || format_str[fmt_idx] == '*')) {
                        if (format_str[fmt_idx] == '$') has_dollar = true;
                        if (format_str[fmt_idx] == '*') has_asterisk = true;
                        fmt_idx++;
                    }

                    size_t int_digits = 0;
                    size_t dec_digits = 0;
                    bool in_decimal = false;
                    bool has_commas = false;

                    while (fmt_idx < format_str.size() && (format_str[fmt_idx] == '#' || format_str[fmt_idx] == '.' || format_str[fmt_idx] == ',')) {
                        if (format_str[fmt_idx] == '.') {
                            in_decimal = true;
                        } else if (format_str[fmt_idx] == ',') {
                            has_commas = true;
                        } else if (format_str[fmt_idx] == '#') {
                            if (in_decimal) dec_digits++;
                            else int_digits++;
                        }
                        fmt_idx++;
                    }

                    bool negative = (num < 0.0);
                    double abs_num = std::abs(num);
                    int64_t int_part = static_cast<int64_t>(abs_num);
                    double frac_part = abs_num - static_cast<double>(int_part);

                    if (dec_digits > 0) {
                        double factor = std::pow(10.0, dec_digits);
                        double rounded_frac = std::round(frac_part * factor) / factor;
                        if (rounded_frac >= 1.0) {
                            int_part += 1;
                            rounded_frac -= 1.0;
                        }
                        frac_part = rounded_frac;
                    } else {
                        int_part = static_cast<int64_t>(std::round(abs_num));
                    }

                    std::string int_str = std::to_string(int_part);
                    if (has_commas && int_str.size() > 3) {
                        std::string with_commas;
                        int count = 0;
                        for (int k = static_cast<int>(int_str.size()) - 1; k >= 0; --k) {
                            if (count > 0 && count % 3 == 0) with_commas.insert(0, 1, ',');
                            with_commas.insert(0, 1, int_str[k]);
                            count++;
                        }
                        int_str = with_commas;
                    }

                    std::string sign_str;
                    if (negative) sign_str = "-";
                    else if (force_sign) sign_str = "+";

                    std::string num_str = sign_str + int_str;
                    if (in_decimal) {
                        num_str += ".";
                        std::string frac_str = std::to_string(static_cast<int64_t>(std::round(frac_part * std::pow(10.0, dec_digits))));
                        while (frac_str.size() < dec_digits) frac_str = "0" + frac_str;
                        num_str += frac_str;
                    }

                    size_t target_width = int_digits + (in_decimal ? (dec_digits + 1) : 0);
                    if (force_sign || negative) target_width += 1;

                    if (num_str.size() < target_width) {
                        char pad_char = has_asterisk ? '*' : ' ';
                        num_str = std::string(target_width - num_str.size(), pad_char) + num_str;
                    }

                    if (has_dollar) num_str = "$" + num_str;

                    line_out << num_str;
                }
            } else {
                line_out << BasicEvaluator::toString(val);
            }
        } else {
            line_out << BasicEvaluator::toString(val);
        }
        trailing_separator = false;
    }

    if (is_using && fmt_idx < format_str.size()) {
        auto is_spec_char = [](char c) -> bool {
            return c == '#' || c == '!' || c == '&' || c == '\\' || c == '$' || c == '*';
        };
        while (fmt_idx < format_str.size() && !is_spec_char(format_str[fmt_idx])) {
            line_out << format_str[fmt_idx++];
        }
    }

    if (!trailing_separator) {
        line_out << "\n";
    }

    if (file_num > 0) {
        files_.writeString(file_num, line_out.str());
    } else {
        output_ << line_out.str();
    }
}

void BasicInterpreter::handleLet(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    // Check for LHS MID$ statement: MID$(A$, start [, length]) = replacement$
    std::string prefix(stmt.substr(0, std::min<size_t>(stmt.size(), 4)));
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
    if (prefix == "MID$") {
        stmt.remove_prefix(4);
        while (!stmt.empty() && stmt.front() != '(') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == '(') stmt.remove_prefix(1);

        // Target variable name
        size_t id_len = 0;
        while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || stmt[id_len] == '$')) id_len++;
        std::string var_name(stmt.substr(0, id_len));
        stmt.remove_prefix(id_len);

        while (!stmt.empty() && stmt.front() != ',') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);

        int start = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        int length = 255;
        if (!stmt.empty() && stmt.front() == ',') {
            stmt.remove_prefix(1);
            length = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
        while (!stmt.empty() && stmt.front() != ')') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ')') stmt.remove_prefix(1);

        while (!stmt.empty() && stmt.front() != '=') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == '=') stmt.remove_prefix(1);

        std::string replacement = BasicEvaluator::toString(eval_.evaluate(stmt));
        std::string target = BasicEvaluator::toString(mem_.getVariable(var_name));
        BasicStrings::lhs_mid(target, start, length, replacement);
        mem_.setVariable(var_name, target);
        return;
    }

    // Standard variable or array element assignment: var = expr  OR  arr(i) = expr
    size_t id_len = 0;
    while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || 
           stmt[id_len] == '_' || stmt[id_len] == '.' ||
           stmt[id_len] == '$' || stmt[id_len] == '%' || stmt[id_len] == '#' || stmt[id_len] == '!')) {
        id_len++;
    }
    std::string var_name(stmt.substr(0, id_len));
    stmt.remove_prefix(id_len);

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    // Check for array assignment: VARNAME(indices) = expr
    if (!stmt.empty() && stmt.front() == '(' && mem_.hasArray(var_name)) {
        stmt.remove_prefix(1); // consume '('
        std::vector<int> indices;
        while (true) {
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            indices.push_back(static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt))));
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty() && stmt.front() == ',') { stmt.remove_prefix(1); continue; }
            if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
            break;
        }
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == '=') stmt.remove_prefix(1);
        VariableValue val = eval_.evaluate(stmt);
        mem_.setArrayElement(var_name, indices, val);
        return;
    }
    // Check for auto-DIM array assignment: VARNAME(indices) = expr (array not yet DIMmed)
    if (!stmt.empty() && stmt.front() == '(') {
        // peek ahead to see if this is "VARNAME(n[,m]) = ..."
        size_t pos = 0;
        int depth = 1;
        pos++; // skip '('
        std::string saved(stmt);
        stmt.remove_prefix(1);
        std::vector<int> indices;
        bool parse_ok = true;
        while (depth > 0 && !stmt.empty()) {
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (stmt.empty()) { parse_ok = false; break; }
            if (stmt.front() == ')') { stmt.remove_prefix(1); depth--; break; }
            try {
                indices.push_back(static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt))));
            } catch (...) { parse_ok = false; break; }
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty() && stmt.front() == ',') { stmt.remove_prefix(1); }
            else if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
            else break;
        }
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (parse_ok && !stmt.empty() && stmt.front() == '=') {
            stmt.remove_prefix(1);
            VariableValue val = eval_.evaluate(stmt);
            mem_.setArrayElement(var_name, indices, val);
            return;
        }
        // Restore stmt if not an array assignment
        stmt = std::string_view(saved);
    }

    if (!stmt.empty() && stmt.front() == '=') {
        stmt.remove_prefix(1);
    }

    VariableValue val = eval_.evaluate(stmt);
    mem_.setVariable(var_name, val);
}

void BasicInterpreter::handleGoto(std::string_view& stmt) {
    int target_line = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    if (program_.find(target_line) == program_.end()) {
        throw BasicException(ErrorCode::UndefinedStatement, "Undefined statement in GOTO");
    }
    current_line_ = target_line;
    current_stmt_idx_ = 0;
}

void BasicInterpreter::handleGosub(std::string_view& stmt) {
    int target_line = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    if (program_.find(target_line) == program_.end()) {
        throw BasicException(ErrorCode::UndefinedStatement, "Undefined statement in GOSUB");
    }
    gosub_stack_.push_back({current_line_, current_stmt_idx_ + 1});
    current_line_ = target_line;
    current_stmt_idx_ = 0;
}

void BasicInterpreter::handleReturn() {
    if (gosub_stack_.empty()) {
        throw BasicException(ErrorCode::ReturnWithoutGosub, "RETURN without GOSUB");
    }
    ProgramPosition ret = gosub_stack_.back();
    gosub_stack_.pop_back();

    current_line_ = ret.line;
    current_stmt_idx_ = ret.stmt_idx;
}

void BasicInterpreter::handleIf(std::string_view& stmt, bool& branch_taken) {
    VariableValue cond_val = eval_.evaluate(stmt);
    bool condition_true = false;
    if (BasicEvaluator::isString(cond_val)) {
        condition_true = !BasicEvaluator::toString(cond_val).empty();
    } else {
        condition_true = (BasicEvaluator::toDouble(cond_val) != 0.0);
    }

    // Match THEN or GOTO
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (stmt.size() >= 4) {
        std::string kw(stmt.substr(0, 4));
        std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
        if (kw == "THEN") {
            stmt.remove_prefix(4);
        } else if (kw == "GOTO") {
            stmt.remove_prefix(4);
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (condition_true) {
                handleGoto(stmt);
                branch_taken = true;
            } else {
                branch_taken = true;
            }
            return;
        }
    }

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    // Look for ELSE outside quotes
    std::string then_part;
    std::string else_part;
    bool in_quote = false;
    bool found_else = false;

    int if_depth = 0;
    for (size_t i = 0; i < stmt.size(); ++i) {
        char c = stmt[i];
        if (c == '"') {
            in_quote = !in_quote;
        } else if (!in_quote) {
            if (i + 2 <= stmt.size()) {
                std::string sub2(stmt.substr(i, 2));
                std::transform(sub2.begin(), sub2.end(), sub2.begin(), ::toupper);
                if (sub2 == "IF" && (i == 0 || std::isspace(static_cast<unsigned char>(stmt[i-1]))) &&
                    (i + 2 == stmt.size() || std::isspace(static_cast<unsigned char>(stmt[i+2])))) {
                    if_depth++;
                }
            }
            if (i + 4 <= stmt.size()) {
                std::string sub(stmt.substr(i, 4));
                std::transform(sub.begin(), sub.end(), sub.begin(), ::toupper);
                if (sub == "ELSE" && (i == 0 || std::isspace(static_cast<unsigned char>(stmt[i-1]))) &&
                    (i + 4 == stmt.size() || std::isspace(static_cast<unsigned char>(stmt[i+4])))) {
                    if (if_depth == 0) {
                        then_part = std::string(stmt.substr(0, i));
                        else_part = std::string(stmt.substr(i + 4));
                        found_else = true;
                        break;
                    } else {
                        if_depth--;
                    }
                }
            }
        }
    }

    if (!found_else) {
        then_part = std::string(stmt);
    }

    auto execute_clause = [&](const std::string& clause) {
        int starting_line = current_line_;
        auto sub_stmts = splitStatements(clause);
        for (const auto& s : sub_stmts) {
            std::string_view sv = s;
            while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
            if (sv.empty()) continue;
            if (std::isdigit(static_cast<unsigned char>(sv.front()))) {
                handleGoto(sv);
                branch_taken = true;
                break;
            }
            bool sub_branch = false;
            executeSingleStatement(sv, sub_branch);
            if (!is_running_ || sub_branch || current_line_ != starting_line) {
                branch_taken = true;
                break;
            }
        }
    };

    if (condition_true) {
        execute_clause(then_part);
    } else {
        if (found_else) {
            execute_clause(else_part);
        } else {
            branch_taken = true;
        }
    }
}

void BasicInterpreter::handleFor(std::string_view& stmt) {
    // FOR var = start TO end [STEP step]
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    size_t id_len = 0;
    while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || stmt[id_len] == '_')) id_len++;
    std::string var_name(stmt.substr(0, id_len));
    stmt.remove_prefix(id_len);

    while (!stmt.empty() && stmt.front() != '=') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == '=') stmt.remove_prefix(1);

    double start_val = BasicEvaluator::toDouble(eval_.evaluate(stmt));

    // Match TO
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    std::string to_kw(stmt.substr(0, std::min<size_t>(stmt.size(), 2)));
    std::transform(to_kw.begin(), to_kw.end(), to_kw.begin(), ::toupper);
    if (to_kw == "TO") stmt.remove_prefix(2);

    double end_val = BasicEvaluator::toDouble(eval_.evaluate(stmt));
    double step_val = 1.0;

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    std::string step_kw(stmt.substr(0, std::min<size_t>(stmt.size(), 4)));
    std::transform(step_kw.begin(), step_kw.end(), step_kw.begin(), ::toupper);
    if (step_kw == "STEP") {
        stmt.remove_prefix(4);
        step_val = BasicEvaluator::toDouble(eval_.evaluate(stmt));
    }

    mem_.setVariable(var_name, start_val);

    // Initial check: skip to matching NEXT if already beyond limit
    bool will_run = (step_val >= 0) ? (start_val <= end_val) : (start_val >= end_val);
    if (!will_run) {
        skipToMatchingNext(var_name);
        return;
    }

    // Determine loop body start location
    int b_line = current_line_;
    size_t b_idx = current_stmt_idx_ + 1;
    auto it = program_.find(current_line_);
    if (it != program_.end()) {
        auto stmts = splitStatements(it->second);
        if (b_idx >= stmts.size()) {
            auto next_it = std::next(it);
            if (next_it != program_.end()) {
                b_line = next_it->first;
                b_idx = 0;
            }
        }
    }

    for_stack_.push_back({var_name, end_val, step_val, b_line, b_idx});
}

void BasicInterpreter::handleNext(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    std::string var_name;
    if (!stmt.empty() && (std::isalpha(static_cast<unsigned char>(stmt.front())) || stmt.front() == '_')) {
        size_t len = 0;
        while (len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[len])) || stmt[len] == '_')) len++;
        var_name = std::string(stmt.substr(0, len));
        stmt.remove_prefix(len);
    }

    if (for_stack_.empty()) {
        throw BasicException(ErrorCode::SyntaxError, "NEXT without FOR");
    }

    size_t stack_idx = for_stack_.size() - 1;
    if (!var_name.empty()) {
        for (int i = static_cast<int>(for_stack_.size()) - 1; i >= 0; --i) {
            std::string n1 = for_stack_[i].var_name;
            std::string n2 = var_name;
            std::transform(n1.begin(), n1.end(), n1.begin(), ::toupper);
            std::transform(n2.begin(), n2.end(), n2.begin(), ::toupper);
            if (n1 == n2) {
                stack_idx = static_cast<size_t>(i);
                break;
            }
        }
    }

    ForLoopState& loop = for_stack_[stack_idx];
    double current_val = BasicEvaluator::toDouble(mem_.getVariable(loop.var_name));
    current_val += loop.step;
    mem_.setVariable(loop.var_name, current_val);

    bool continue_loop = (loop.step >= 0) ? (current_val <= loop.target) : (current_val >= loop.target);
    if (continue_loop) {
        current_line_ = loop.body_line;
        current_stmt_idx_ = loop.body_stmt_idx;
    } else {
        for_stack_.erase(for_stack_.begin() + stack_idx);
    }
}

std::pair<int, int> BasicInterpreter::parseCoordinates(std::string_view& stmt) {
    while (!stmt.empty() && stmt.front() != '(') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == '(') stmt.remove_prefix(1);

    int x = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    while (!stmt.empty() && stmt.front() != ',') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);

    int y = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    while (!stmt.empty() && stmt.front() != ')') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ')') stmt.remove_prefix(1);

    return {x, y};
}

void BasicInterpreter::handlePset(std::string_view& stmt, bool preset) {
    auto [x, y] = parseCoordinates(stmt);
    // Default color: PSET=foreground(3), PRESET=background(0)
    uint8_t color = preset ? 0 : 3;
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() != ')' && stmt.front() != ':') {
            color = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
    }
    gfx_.pset(x, y, color);
}

void BasicInterpreter::handleDim(std::string_view& stmt) {
    // DIM A(10), B$(20), MAT(3,3)
    while (!stmt.empty()) {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        // Parse variable name
        std::string var_name;
        while (!stmt.empty() && (std::isalnum(static_cast<unsigned char>(stmt.front())) || stmt.front() == '_' || stmt.front() == '$' || stmt.front() == '%' || stmt.front() == '#' || stmt.front() == '!')) {
            var_name += stmt.front();
            stmt.remove_prefix(1);
        }
        if (var_name.empty()) break;
        // Parse dimensions
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        std::vector<int> dims;
        if (!stmt.empty() && stmt.front() == '(') {
            stmt.remove_prefix(1); // consume '('
            while (true) {
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                int d = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                dims.push_back(d);
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                if (!stmt.empty() && stmt.front() == ',') { stmt.remove_prefix(1); continue; }
                if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
                break;
            }
        }
        if (!dims.empty()) {
            mem_.dimArray(var_name, dims);
        }
        // Skip comma between declarations
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        else break;
    }
}


void BasicInterpreter::handleLine(std::string_view& stmt) {
    // Syntax: LINE (x1, y1)-(x2, y2) [, [color] [, [B|BF]]]
    auto [x1, y1] = parseCoordinates(stmt);
    while (!stmt.empty() && stmt.front() != '-') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == '-') stmt.remove_prefix(1);
    auto [x2, y2] = parseCoordinates(stmt);

    uint8_t color = 3;
    bool is_box = false;
    bool is_filled = false;

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() != ',') {
            color = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
        if (!stmt.empty() && stmt.front() == ',') {
            stmt.remove_prefix(1);
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (stmt.size() >= 2 && (stmt.substr(0, 2) == "BF" || stmt.substr(0, 2) == "bf")) {
                is_box = true;
                is_filled = true;
            } else if (!stmt.empty() && (stmt.front() == 'B' || stmt.front() == 'b')) {
                is_box = true;
            }
        }
    }

    if (is_box) {
        gfx_.box(x1, y1, x2, y2, color, is_filled);
    } else {
        gfx_.line(x1, y1, x2, y2, color);
    }
}

void BasicInterpreter::handleCircle(std::string_view& stmt) {
    // Syntax: CIRCLE (x, y), radius [, color]
    auto [x, y] = parseCoordinates(stmt);
    while (!stmt.empty() && stmt.front() != ',') stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);

    int radius = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    uint8_t color = 3;
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        color = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    }

    gfx_.circle(x, y, radius, color);
}

// Exactly converts lines 110-140 of ADVGRP.ASM:
// PAINT (X,Y) [, [FILLC] [, [BORDC]]]
void BasicInterpreter::handlePaint(std::string_view& stmt) {
    auto [x, y] = parseCoordinates(stmt);
    uint8_t fill_color = 3;
    uint8_t border_color = fill_color;

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        fill_color = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        border_color = fill_color; // Default border color is same as fill color

        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') {
            stmt.remove_prefix(1);
            border_color = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
    }

    gfx_.paint(x, y, fill_color, border_color);
}

void BasicInterpreter::handleOpen(std::string_view& stmt) {
    // Form 1: OPEN "mode", [#]filenum, "filename"
    // Form 2: OPEN "filename" FOR {INPUT|OUTPUT|APPEND|RANDOM} AS [#]filenum
    VariableValue arg1 = eval_.evaluate(stmt);
    std::string str1 = BasicEvaluator::toString(arg1);

    auto match_kw = [&stmt](const std::string& kw) -> bool {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (stmt.size() < kw.size()) return false;
        for (size_t i = 0; i < kw.size(); ++i) {
            if (std::toupper(static_cast<unsigned char>(stmt[i])) != std::toupper(static_cast<unsigned char>(kw[i]))) return false;
        }
        stmt.remove_prefix(kw.size());
        return true;
    };

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    if (match_kw("FOR")) {
        // Form 2: str1 is filename
        std::string mode_str;
        if (match_kw("INPUT")) mode_str = "INPUT";
        else if (match_kw("OUTPUT")) mode_str = "OUTPUT";
        else if (match_kw("APPEND")) mode_str = "APPEND";
        else if (match_kw("RANDOM")) mode_str = "RANDOM";
        else throw BasicException(ErrorCode::SyntaxError, "Bad file mode in OPEN");

        if (!match_kw("AS")) throw BasicException(ErrorCode::SyntaxError, "Expected AS in OPEN");
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == '#')) stmt.remove_prefix(1);
        int filenum = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));

        FileMode mode = FileMode::Input;
        if (mode_str == "OUTPUT") mode = FileMode::Output;
        else if (mode_str == "APPEND") mode = FileMode::Append;
        else if (mode_str == "RANDOM") mode = FileMode::Random;

        files_.openFile(filenum, str1, mode);
    } else {
        // Form 1: str1 is mode ("I", "O", "A", "R")
        while (!stmt.empty() && stmt.front() != ',') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == '#')) stmt.remove_prefix(1);
        int filenum = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));

        while (!stmt.empty() && stmt.front() != ',') stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        std::string filename = BasicEvaluator::toString(eval_.evaluate(stmt));

        FileMode mode = FileMode::Input;
        std::string m = str1;
        for (char& c : m) c = std::toupper(static_cast<unsigned char>(c));
        if (m == "O" || m == "OUTPUT") mode = FileMode::Output;
        else if (m == "A" || m == "APPEND") mode = FileMode::Append;
        else if (m == "R" || m == "RANDOM") mode = FileMode::Random;

        files_.openFile(filenum, filename, mode);
    }
}

void BasicInterpreter::handleClose(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (stmt.empty()) {
        files_.closeAll();
        return;
    }
    while (!stmt.empty()) {
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == '#')) stmt.remove_prefix(1);
        if (stmt.empty()) break;
        int filenum = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        files_.closeFile(filenum);
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        else break;
    }
}

void BasicInterpreter::handleLineInput(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    if (!stmt.empty() && stmt.front() == '#') {
        // LINE INPUT #filenum, var$
        stmt.remove_prefix(1);
        int filenum = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) stmt.remove_prefix(1);

        size_t id_len = 0;
        while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || 
               stmt[id_len] == '_' || stmt[id_len] == '.' || stmt[id_len] == '$')) id_len++;
        std::string var_name(stmt.substr(0, id_len));
        stmt.remove_prefix(id_len);

        std::string line = files_.readLine(filenum);
        mem_.setVariable(var_name, line);
        return;
    }

    // Console LINE INPUT ["prompt";] var$
    std::string prompt = "";
    if (!stmt.empty() && stmt.front() == '"') {
        prompt = BasicEvaluator::toString(eval_.evaluate(stmt));
        while (!stmt.empty() && (stmt.front() == ';' || stmt.front() == ',')) stmt.remove_prefix(1);
    }

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    size_t id_len = 0;
    while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || 
           stmt[id_len] == '_' || stmt[id_len] == '.' || stmt[id_len] == '$')) id_len++;
    std::string var_name(stmt.substr(0, id_len));
    stmt.remove_prefix(id_len);

    bool is_array = false;
    std::vector<int> indices;
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == '(') {
        is_array = true;
        stmt.remove_prefix(1);
        while (true) {
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            int idx = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
            indices.push_back(idx);
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
            else if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
            else break;
        }
    }

    // Flush any pending buffered output to stdout before prompting
    std::string pending = output_.str();
    if (!pending.empty()) {
        std::cout << pending;
        std::cout.flush();
        output_.str("");
        output_.clear();
    }

    std::cout << prompt;
    std::cout.flush();
    std::string user_input;
    if (std::getline(std::cin, user_input)) {
        if (is_array) {
            mem_.setArrayElement(var_name, indices, user_input);
        } else {
            mem_.setVariable(var_name, user_input);
        }
    } else {
        is_running_ = false;
    }
}

void BasicInterpreter::handleInput(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    if (!stmt.empty() && stmt.front() == '#') {
        // INPUT #filenum, var1, var2...
        stmt.remove_prefix(1);
        int filenum = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) stmt.remove_prefix(1);

        while (!stmt.empty()) {
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (stmt.empty()) break;
            size_t id_len = 0;
            while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || 
                   stmt[id_len] == '_' || stmt[id_len] == '.' ||
                   stmt[id_len] == '$' || stmt[id_len] == '%' || stmt[id_len] == '#' || stmt[id_len] == '!')) {
                id_len++;
                char last = stmt[id_len - 1];
                if (last == '$' || last == '%' || last == '#' || last == '!') break;
            }
            std::string var_name(stmt.substr(0, id_len));
            stmt.remove_prefix(id_len);

            bool is_array = false;
            std::vector<int> indices;
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty() && stmt.front() == '(') {
                is_array = true;
                stmt.remove_prefix(1);
                while (true) {
                    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                    int idx = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                    indices.push_back(idx);
                    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                    if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
                    else if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
                    else break;
                }
            }

            std::string token = files_.readToken(filenum);
            if (is_array) {
                if (!var_name.empty() && var_name.back() == '$') {
                    mem_.setArrayElement(var_name, indices, token);
                } else {
                    mem_.setArrayElement(var_name, indices, BasicStrings::val(token));
                }
            } else {
                if (!var_name.empty() && var_name.back() == '$') {
                    mem_.setVariable(var_name, token);
                } else {
                    mem_.setVariable(var_name, BasicStrings::val(token));
                }
            }

            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
            else break;
        }
        return;
    }

    // Console INPUT [;] ["prompt" {; | ,}] var1, var2...
    bool no_newline = false;
    if (!stmt.empty() && stmt.front() == ';') {
        no_newline = true;
        stmt.remove_prefix(1);
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    }

    std::string prompt = "? ";
    if (!stmt.empty() && stmt.front() == '"') {
        std::string prompt_text = BasicEvaluator::toString(eval_.evaluate(stmt));
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        bool add_question = true;
        if (!stmt.empty() && stmt.front() == ',') {
            add_question = false;
            stmt.remove_prefix(1);
        } else if (!stmt.empty() && stmt.front() == ';') {
            add_question = true;
            stmt.remove_prefix(1);
        }
        if (add_question) {
            prompt = prompt_text + "? ";
        } else {
            prompt = prompt_text;
        }
    }

    // Parse variable targets
    struct InputTarget {
        std::string name;
        bool is_array = false;
        std::vector<int> indices;
    };
    std::vector<InputTarget> targets;

    while (!stmt.empty()) {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (stmt.empty()) break;

        size_t id_len = 0;
        while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) || 
               stmt[id_len] == '_' || stmt[id_len] == '.' ||
               stmt[id_len] == '$' || stmt[id_len] == '%' || stmt[id_len] == '#' || stmt[id_len] == '!')) {
            id_len++;
            char last = stmt[id_len - 1];
            if (last == '$' || last == '%' || last == '#' || last == '!') break;
        }
        if (id_len == 0) break;

        InputTarget target;
        target.name = std::string(stmt.substr(0, id_len));
        stmt.remove_prefix(id_len);

        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == '(') {
            target.is_array = true;
            stmt.remove_prefix(1);
            while (true) {
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                int idx = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                target.indices.push_back(idx);
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
                else if (!stmt.empty() && stmt.front() == ')') { stmt.remove_prefix(1); break; }
                else break;
            }
        }

        targets.push_back(target);

        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') {
            stmt.remove_prefix(1);
        } else {
            break;
        }
    }

    if (targets.empty()) return;

    // Flush any pending buffered output to stdout before prompting
    std::string pending = output_.str();
    if (!pending.empty()) {
        std::cout << pending;
        std::cout.flush();
        output_.str("");
        output_.clear();
    }

    // Interactive input reading loop with ?? continuation and ?Extra ignored
    std::vector<std::string> values;
    bool is_first_line = true;

    auto tokenize_line = [&](const std::string& line, size_t curr_count) -> std::vector<std::string> {
        std::vector<std::string> tokens;
        size_t i = 0;
        while (i < line.size()) {
            while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) i++;
            if (i >= line.size()) break;

            std::string tok;
            if (line[i] == '"') {
                i++; // skip open quote
                while (i < line.size() && line[i] != '"') {
                    tok += line[i++];
                }
                if (i < line.size() && line[i] == '"') i++;
                while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) i++;
                if (i < line.size() && line[i] == ',') i++;
            } else {
                bool has_comma = (line.find(',', i) != std::string::npos);
                if (has_comma) {
                    while (i < line.size() && line[i] != ',') {
                        tok += line[i++];
                    }
                    if (i < line.size() && line[i] == ',') i++;
                    while (!tok.empty() && std::isspace(static_cast<unsigned char>(tok.back()))) {
                        tok.pop_back();
                    }
                } else {
                    size_t target_idx = curr_count + tokens.size();
                    bool expect_num = (target_idx < targets.size() && 
                                       (targets[target_idx].name.empty() || targets[target_idx].name.back() != '$'));
                    if (expect_num && (curr_count + tokens.size() + 1 < targets.size())) {
                        while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) {
                            tok += line[i++];
                        }
                    } else {
                        tok = line.substr(i);
                        while (!tok.empty() && std::isspace(static_cast<unsigned char>(tok.back()))) {
                            tok.pop_back();
                        }
                        i = line.size();
                    }
                }
            }
            tokens.push_back(tok);
        }
        return tokens;
    };

    while (values.size() < targets.size() && is_running_) {
        if (is_first_line) {
            std::cout << prompt;
            std::cout.flush();
            is_first_line = false;
        } else {
            std::cout << "?? ";
            std::cout.flush();
        }

        std::string user_line;
        if (!std::getline(std::cin, user_line)) {
            is_running_ = false;
            return;
        }

        auto tokens = tokenize_line(user_line, values.size());
        if (tokens.empty() && values.empty()) {
            values.push_back("0");
        } else {
            for (const auto& t : tokens) {
                values.push_back(t);
            }
        }
    }

    if (!is_running_) return;

    if (values.size() > targets.size()) {
        std::cout << "?Extra ignored\n";
        std::cout.flush();
    }

    // Assign to targets
    for (size_t k = 0; k < targets.size(); ++k) {
        const auto& target = targets[k];
        std::string val_str = (k < values.size()) ? values[k] : "0";
        if (target.is_array) {
            if (!target.name.empty() && target.name.back() == '$') {
                mem_.setArrayElement(target.name, target.indices, val_str);
            } else {
                mem_.setArrayElement(target.name, target.indices, BasicStrings::val(val_str));
            }
        } else {
            if (!target.name.empty() && target.name.back() == '$') {
                mem_.setVariable(target.name, val_str);
            } else {
                mem_.setVariable(target.name, BasicStrings::val(val_str));
            }
        }
    }
}

void BasicInterpreter::handleLocate(std::string_view& stmt) {
    int row = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    int col = console_.getPos();
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        col = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    }
    console_.locate(row, col);
}

void BasicInterpreter::handleColor(std::string_view& stmt) {
    uint8_t fg = console_.getForeground();
    uint8_t bg = console_.getBackground();
    uint8_t border = console_.getBorder();

    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (!stmt.empty() && stmt.front() != ',') {
        fg = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    }
    if (!stmt.empty() && stmt.front() == ',') {
        stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() != ',') {
            bg = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
        if (!stmt.empty() && stmt.front() == ',') {
            stmt.remove_prefix(1);
            border = static_cast<uint8_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        }
    }
    console_.setColor(fg, bg, border);
}

void BasicInterpreter::handleWidth(std::string_view& stmt) {
    int w = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    console_.setWidth(w);
}

void BasicInterpreter::handleSaveScreen(std::string_view& stmt) {
    std::string filename = BasicEvaluator::toString(eval_.evaluate(stmt));
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".ppm") {
        ScreenExport::savePPM(filename, gfx_);
    } else {
        ScreenExport::saveBMP(filename, gfx_);
    }
}

void BasicInterpreter::handleDraw(std::string_view& stmt) {
    VariableValue val = eval_.evaluate(stmt);
    std::string commands = BasicEvaluator::toString(val);
    gfx_.draw(commands);
}

void BasicInterpreter::rebuildDataPool() {
    data_items_.clear();
    data_ptr_ = 0;

    for (const auto& [line_num, line_text] : program_) {
        auto stmts = splitStatements(line_text);
        for (auto& s : stmts) {
            std::string_view stmt = s;
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (stmt.size() >= 4) {
                std::string kw(stmt.substr(0, 4));
                std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
                if (kw == "DATA") {
                    stmt.remove_prefix(4);
                    while (!stmt.empty()) {
                        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                        if (stmt.empty()) break;
                        std::string item;
                        if (stmt.front() == '"') {
                            stmt.remove_prefix(1);
                            while (!stmt.empty() && stmt.front() != '"') {
                                item += stmt.front();
                                stmt.remove_prefix(1);
                            }
                            if (!stmt.empty() && stmt.front() == '"') stmt.remove_prefix(1);
                        } else {
                            while (!stmt.empty() && stmt.front() != ',') {
                                item += stmt.front();
                                stmt.remove_prefix(1);
                            }
                            while (!item.empty() && std::isspace(static_cast<unsigned char>(item.back()))) {
                                item.pop_back();
                            }
                        }
                        data_items_.push_back({line_num, item});
                        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
                    }
                }
            }
        }
    }
}

void BasicInterpreter::handleRead(std::string_view& stmt) {
    while (!stmt.empty()) {
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
            stmt.remove_prefix(1);
        }
        if (stmt.empty()) break;

        size_t id_len = 0;
        while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) ||
               stmt[id_len] == '_' || stmt[id_len] == '.' ||
               stmt[id_len] == '$' || stmt[id_len] == '%' || stmt[id_len] == '#' || stmt[id_len] == '!')) {
            id_len++;
            char last = stmt[id_len - 1];
            if (last == '$' || last == '%' || last == '#' || last == '!') break;
        }

        std::string var_name(stmt.substr(0, id_len));
        stmt.remove_prefix(id_len);

        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

        // Check if this is an array element: var_name(idx1, idx2...)
        bool is_array = false;
        std::vector<int> indices;
        if (!stmt.empty() && stmt.front() == '(') {
            is_array = true;
            stmt.remove_prefix(1); // consume '('
            while (true) {
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                int idx = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                indices.push_back(idx);
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                if (!stmt.empty() && stmt.front() == ',') {
                    stmt.remove_prefix(1);
                } else if (!stmt.empty() && stmt.front() == ')') {
                    stmt.remove_prefix(1);
                    break;
                } else {
                    break;
                }
            }
        }

        if (data_ptr_ >= data_items_.size()) {
            throw BasicException(ErrorCode::OutOfData, "Out of data");
        }

        std::string raw_val = data_items_[data_ptr_++].second;
        if (is_array) {
            if (!var_name.empty() && var_name.back() == '$') {
                mem_.setArrayElement(var_name, indices, raw_val);
            } else {
                mem_.setArrayElement(var_name, indices, BasicStrings::val(raw_val));
            }
        } else {
            if (!var_name.empty() && var_name.back() == '$') {
                mem_.setVariable(var_name, raw_val);
            } else {
                mem_.setVariable(var_name, BasicStrings::val(raw_val));
            }
        }

        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        else break;
    }
}

void BasicInterpreter::handleRestore(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    if (stmt.empty()) {
        data_ptr_ = 0;
        return;
    }
    int target_line = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    data_ptr_ = data_items_.size();
    for (size_t i = 0; i < data_items_.size(); ++i) {
        if (data_items_[i].first >= target_line) {
            data_ptr_ = i;
            break;
        }
    }
}

void BasicInterpreter::handleWhile(std::string_view& stmt, bool& branch_taken) {
    VariableValue cond_val = eval_.evaluate(stmt);
    bool cond_true = (BasicEvaluator::isString(cond_val))
        ? !BasicEvaluator::toString(cond_val).empty()
        : (BasicEvaluator::toDouble(cond_val) != 0.0);

    if (cond_true) {
        if (while_stack_.empty() || while_stack_.back().line != current_line_ || while_stack_.back().stmt_idx != current_stmt_idx_) {
            while_stack_.push_back({current_line_, current_stmt_idx_});
        }
    } else {
        if (!while_stack_.empty() && while_stack_.back().line == current_line_ && while_stack_.back().stmt_idx == current_stmt_idx_) {
            while_stack_.pop_back();
        }
        skipToMatchingWend();
        branch_taken = true;
    }
}

void BasicInterpreter::handleWend() {
    if (while_stack_.empty()) {
        throw BasicException(ErrorCode::SyntaxError, "WEND without WHILE");
    }
    ProgramPosition pos = while_stack_.back();
    current_line_ = pos.line;
    current_stmt_idx_ = pos.stmt_idx;
}

void BasicInterpreter::handleSwap(std::string_view& stmt) {
    auto parse_lvalue = [&]() -> std::pair<std::string, std::vector<int>> {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        size_t id_len = 0;
        while (id_len < stmt.size() && (std::isalnum(static_cast<unsigned char>(stmt[id_len])) ||
               stmt[id_len] == '_' || stmt[id_len] == '.' ||
               stmt[id_len] == '$' || stmt[id_len] == '%' || stmt[id_len] == '#' || stmt[id_len] == '!')) {
            id_len++;
            char last = stmt[id_len - 1];
            if (last == '$' || last == '%' || last == '#' || last == '!') break;
        }
        std::string v(stmt.substr(0, id_len));
        stmt.remove_prefix(id_len);
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

        std::vector<int> indices;
        if (!stmt.empty() && stmt.front() == '(') {
            stmt.remove_prefix(1);
            while (true) {
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                int idx = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                indices.push_back(idx);
                while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
                if (!stmt.empty() && stmt.front() == ',') {
                    stmt.remove_prefix(1);
                } else if (!stmt.empty() && stmt.front() == ')') {
                    stmt.remove_prefix(1);
                    break;
                } else {
                    break;
                }
            }
        }
        return {v, indices};
    };

    auto [v1, idx1] = parse_lvalue();
    while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
        stmt.remove_prefix(1);
    }
    auto [v2, idx2] = parse_lvalue();

    VariableValue val1 = idx1.empty() ? mem_.getVariable(v1) : mem_.getArrayElement(v1, idx1);
    VariableValue val2 = idx2.empty() ? mem_.getVariable(v2) : mem_.getArrayElement(v2, idx2);

    if (BasicEvaluator::isString(val1) != BasicEvaluator::isString(val2)) {
        throw TypeMismatchException();
    }

    if (idx1.empty()) mem_.setVariable(v1, val2);
    else mem_.setArrayElement(v1, idx1, val2);

    if (idx2.empty()) mem_.setVariable(v2, val1);
    else mem_.setArrayElement(v2, idx2, val1);
}

void BasicInterpreter::handleRandomize(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    uint32_t seed = 0;
    if (!stmt.empty()) {
        seed = static_cast<uint32_t>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    } else {
        seed = static_cast<uint32_t>(std::time(nullptr));
    }
    BasicMath::randomize(seed);
}

void BasicInterpreter::handlePoke(std::string_view& stmt) {
    size_t addr = static_cast<size_t>(BasicEvaluator::toInt64(eval_.evaluate(stmt)));
    while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
        stmt.remove_prefix(1);
    }
    uint8_t val = static_cast<uint8_t>(BasicEvaluator::toInt64(eval_.evaluate(stmt)));
    mem_.poke(addr, val);
}

void BasicInterpreter::handleBeep() {
    output_ << '\a';
}

void BasicInterpreter::handleSound(std::string_view& stmt) {
    double freq = BasicEvaluator::toDouble(eval_.evaluate(stmt));
    while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
        stmt.remove_prefix(1);
    }
    double duration = BasicEvaluator::toDouble(eval_.evaluate(stmt));
    (void)freq;
    (void)duration;
}

void BasicInterpreter::handleOn(std::string_view& stmt) {
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);

    // Check for ON ERROR GOTO [line | 0]
    auto peek_kw = [&](const std::string& kw) -> bool {
        if (stmt.size() < kw.size()) return false;
        for (size_t i = 0; i < kw.size(); ++i) {
            if (std::toupper(static_cast<unsigned char>(stmt[i])) != std::toupper(static_cast<unsigned char>(kw[i]))) return false;
        }
        if (stmt.size() > kw.size()) {
            char next = stmt[kw.size()];
            if (std::isalnum(static_cast<unsigned char>(next)) || next == '_') return false;
        }
        stmt.remove_prefix(kw.size());
        return true;
    };

    if (peek_kw("ERROR")) {
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (peek_kw("GOTO")) {
            while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
            if (!stmt.empty()) {
                int target = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
                error_handler_line_ = target; // 0 disables handler
            }
        }
        return; // handled
    }

    int n = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
    while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
    bool is_gosub = false;
    if (stmt.size() >= 4) {
        std::string kw(stmt.substr(0, 4));
        std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
        if (kw == "GOTO") {
            stmt.remove_prefix(4);
        } else if (stmt.size() >= 5) {
            std::string kw5(stmt.substr(0, 5));
            std::transform(kw5.begin(), kw5.end(), kw5.begin(), ::toupper);
            if (kw5 == "GOSUB") {
                stmt.remove_prefix(5);
                is_gosub = true;
            }
        }
    }

    std::vector<int> targets;
    while (!stmt.empty()) {
        while (!stmt.empty() && (std::isspace(static_cast<unsigned char>(stmt.front())) || stmt.front() == ',')) {
            stmt.remove_prefix(1);
        }
        if (stmt.empty()) break;
        int line_target = static_cast<int>(BasicEvaluator::toDouble(eval_.evaluate(stmt)));
        targets.push_back(line_target);
        while (!stmt.empty() && std::isspace(static_cast<unsigned char>(stmt.front()))) stmt.remove_prefix(1);
        if (!stmt.empty() && stmt.front() == ',') stmt.remove_prefix(1);
        else break;
    }

    if (n >= 1 && n <= static_cast<int>(targets.size())) {
        int selected_line = targets[n - 1];
        if (is_gosub) {
            gosub_stack_.push_back({current_line_, current_stmt_idx_ + 1});
        }
        current_line_ = selected_line;
        current_stmt_idx_ = 0;
    }
}

void BasicInterpreter::skipToMatchingNext(const std::string& var_name) {
    (void)var_name;
    auto it = program_.find(current_line_);
    if (it == program_.end()) return;

    int depth = 1;
    size_t start_s = current_stmt_idx_ + 1;

    while (it != program_.end() && depth > 0) {
        auto stmts = splitStatements(it->second);
        for (size_t s = (it->first == current_line_ ? start_s : 0); s < stmts.size(); ++s) {
            std::string_view sv = stmts[s];
            while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
            if (sv.size() >= 3) {
                std::string kw(sv.substr(0, 3));
                std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
                if (kw == "FOR" && (sv.size() == 3 || std::isspace(static_cast<unsigned char>(sv[3])))) {
                    depth++;
                }
            }
            if (sv.size() >= 4) {
                std::string kw(sv.substr(0, 4));
                std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
                if (kw == "NEXT" && (sv.size() == 4 || std::isspace(static_cast<unsigned char>(sv[4])))) {
                    depth--;
                    if (depth == 0) {
                        if (s + 1 < stmts.size()) {
                            current_line_ = it->first;
                            current_stmt_idx_ = s + 1;
                        } else {
                            auto next_it = std::next(it);
                            if (next_it != program_.end()) {
                                current_line_ = next_it->first;
                                current_stmt_idx_ = 0;
                            } else {
                                is_running_ = false;
                            }
                        }
                        return;
                    }
                }
            }
        }
        ++it;
        start_s = 0;
    }
    if (depth > 0) {
        throw BasicException(ErrorCode::SyntaxError, "FOR without NEXT");
    }
}

void BasicInterpreter::skipToMatchingWend() {
    auto it = program_.find(current_line_);
    if (it == program_.end()) return;

    int depth = 1;
    size_t start_s = current_stmt_idx_ + 1;

    while (it != program_.end() && depth > 0) {
        auto stmts = splitStatements(it->second);
        for (size_t s = (it->first == current_line_ ? start_s : 0); s < stmts.size(); ++s) {
            std::string_view sv = stmts[s];
            while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
            if (sv.size() >= 5) {
                std::string kw(sv.substr(0, 5));
                std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
                if (kw == "WHILE" && (sv.size() == 5 || std::isspace(static_cast<unsigned char>(sv[5])))) {
                    depth++;
                }
            }
            if (sv.size() >= 4) {
                std::string kw(sv.substr(0, 4));
                std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
                if (kw == "WEND" && (sv.size() == 4 || std::isspace(static_cast<unsigned char>(sv[4])))) {
                    depth--;
                    if (depth == 0) {
                        if (s + 1 < stmts.size()) {
                            current_line_ = it->first;
                            current_stmt_idx_ = s + 1;
                        } else {
                            auto next_it = std::next(it);
                            if (next_it != program_.end()) {
                                current_line_ = next_it->first;
                                current_stmt_idx_ = 0;
                            } else {
                                is_running_ = false;
                            }
                        }
                        return;
                    }
                }
            }
        }
        ++it;
        start_s = 0;
    }
    if (depth > 0) {
        throw BasicException(ErrorCode::SyntaxError, "WHILE without WEND");
    }
}

} // namespace gwbasic
