#include "basic_io.h"
#include <iostream>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <conio.h>
#endif

namespace gwbasic {

// Standard CGA 16-color Palette
const ScreenExport::RGBColor ScreenExport::CGA_PALETTE[16] = {
    {0, 0, 0},       // 0: Black
    {0, 0, 170},     // 1: Blue
    {0, 170, 0},     // 2: Green
    {0, 170, 170},   // 3: Cyan
    {170, 0, 0},     // 4: Red
    {170, 0, 170},   // 5: Magenta
    {170, 85, 0},    // 6: Brown
    {170, 170, 170}, // 7: Light Gray
    {85, 85, 85},    // 8: Dark Gray
    {85, 85, 255},   // 9: Light Blue
    {85, 255, 85},   // 10: Light Green
    {85, 255, 255},  // 11: Light Cyan
    {255, 85, 85},   // 12: Light Red
    {255, 85, 255},  // 13: Light Magenta
    {255, 255, 85},  // 14: Yellow
    {255, 255, 255}  // 15: White
};

// ==================== FileManager ====================

FileManager::~FileManager() {
    closeAll();
}

void FileManager::validateFileNum(int filenum) const {
    if (filenum < 1 || filenum > MAX_FILES) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file number (1-15)");
    }
}

void FileManager::openFile(int filenum, const std::string& filename, FileMode mode, size_t reclen) {
    validateFileNum(filenum);
    if (open_files_.find(filenum) != open_files_.end()) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "File already open");
    }

    std::ios_base::openmode open_flags = std::ios::binary;
    switch (mode) {
        case FileMode::Input:
            open_flags |= std::ios::in;
            break;
        case FileMode::Output:
            open_flags |= std::ios::out | std::ios::trunc;
            break;
        case FileMode::Append:
            open_flags |= std::ios::out | std::ios::app;
            break;
        case FileMode::Random:
            open_flags |= std::ios::in | std::ios::out;
            break;
    }

    auto stream = std::make_unique<std::fstream>(filename, open_flags);
    if (!stream->is_open()) {
        if (mode == FileMode::Input) {
            throw BasicException(ErrorCode::UndefinedStatement, "File not found: " + filename);
        }
        // If random mode and file does not exist, create it
        if (mode == FileMode::Random) {
            std::ofstream create(filename, std::ios::binary);
            create.close();
            stream = std::make_unique<std::fstream>(filename, open_flags);
        }
        if (!stream->is_open()) {
            throw BasicException(ErrorCode::UndefinedStatement, "Cannot open file: " + filename);
        }
    }

    FileHandle handle;
    handle.filenum = filenum;
    handle.filename = filename;
    handle.mode = mode;
    handle.stream = std::move(stream);
    handle.record_length = reclen;
    handle.current_record = 1;

    open_files_[filenum] = std::move(handle);
}

void FileManager::closeFile(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it != open_files_.end()) {
        if (it->second.stream && it->second.stream->is_open()) {
            it->second.stream->close();
        }
        open_files_.erase(it);
    }
}

void FileManager::closeAll() {
    for (auto& [num, handle] : open_files_) {
        if (handle.stream && handle.stream->is_open()) {
            handle.stream->close();
        }
    }
    open_files_.clear();
}

bool FileManager::isFileOpen(int filenum) const {
    return open_files_.find(filenum) != open_files_.end();
}

FileMode FileManager::getMode(int filenum) const {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end()) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file number (not open)");
    }
    return it->second.mode;
}

void FileManager::writeString(int filenum, const std::string& text) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end() || it->second.mode == FileMode::Input) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file mode for writing");
    }
    *(it->second.stream) << text;
}

std::string FileManager::readLine(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end() || it->second.mode == FileMode::Output) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file mode for reading");
    }

    std::string line;
    if (std::getline(*(it->second.stream), line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        return line;
    }
    return "";
}

std::string FileManager::readToken(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end() || it->second.mode == FileMode::Output) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file mode for reading");
    }

    std::fstream& fs = *(it->second.stream);
    char c = 0;
    // Skip leading whitespace / commas
    while (fs.get(c) && (std::isspace(static_cast<unsigned char>(c)) || c == ',')) {}
    if (!fs) return "";

    std::string token;
    if (c == '"') {
        // Quoted string
        while (fs.get(c) && c != '"') {
            token += c;
        }
    } else {
        token += c;
        while (fs.get(c) && c != ',' && c != '\r' && c != '\n') {
            token += c;
        }
        // Trim trailing whitespace
        while (!token.empty() && std::isspace(static_cast<unsigned char>(token.back()))) {
            token.pop_back();
        }
    }
    return token;
}

bool FileManager::isEOF(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end()) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file number");
    }
    std::fstream& fs = *(it->second.stream);
    int c = fs.peek();
    while (c != EOF && (c == '\r' || c == '\n' || c == ' ' || c == '\t' || c == 26)) {
        fs.get();
        c = fs.peek();
    }
    return (c == EOF || fs.eof() || c == 26); // 26 = Ctrl-Z (ASCCTZ in GIODSK.ASM)
}

int64_t FileManager::getLOF(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end()) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file number");
    }
    std::fstream& fs = *(it->second.stream);
    auto cur = fs.tellg();
    fs.seekg(0, std::ios::end);
    auto end = fs.tellg();
    fs.seekg(cur);
    return (end >= 0) ? static_cast<int64_t>(end) : 0;
}

int64_t FileManager::getLOC(int filenum) {
    validateFileNum(filenum);
    auto it = open_files_.find(filenum);
    if (it == open_files_.end()) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Bad file number");
    }
    std::fstream& fs = *(it->second.stream);
    auto pos = fs.tellg();
    if (it->second.mode == FileMode::Random) {
        return (pos >= 0) ? (static_cast<int64_t>(pos) / it->second.record_length + 1) : 1;
    }
    return (pos >= 0) ? static_cast<int64_t>(pos) / 128 : 0;
}

// ==================== ConsoleIO ====================

ConsoleIO::ConsoleIO() {}

std::string ConsoleIO::inkey() {
    if (!key_buffer_.empty()) {
        std::string res(1, key_buffer_.front());
        key_buffer_.erase(0, 1);
        return res;
    }

#ifdef _WIN32
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 0xE0) {
            // Extended key code (arrows, function keys)
            int ext = _getch();
            std::string res(1, '\0');
            res += static_cast<char>(ext);
            return res;
        }
        return std::string(1, static_cast<char>(ch));
    }
#endif

    return "";
}

void ConsoleIO::pushKey(const std::string& key) {
    key_buffer_ += key;
}

void ConsoleIO::locate(int row, int col) {
    if (row < 1 || row > 25 || col < 1 || col > text_width_) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Illegal function call in LOCATE");
    }
    cursor_row_ = row;
    cursor_col_ = col;
    // ANSI escape code for terminal repositioning
    std::cout << "\033[" << row << ";" << col << "H";
}

void ConsoleIO::setColor(uint8_t fg, uint8_t bg, uint8_t border) {
    fg_color_ = fg & 0x1F;
    bg_color_ = bg & 0x07;
    border_color_ = border & 0x0F;
}

void ConsoleIO::setWidth(int width) {
    if (width != 40 && width != 80) {
        throw BasicException(ErrorCode::IllegalFunctionCall, "Width must be 40 or 80");
    }
    text_width_ = width;
}

// ==================== ScreenExport ====================

bool ScreenExport::savePPM(const std::string& filename, const BasicGraphics& gfx) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    int w = gfx.getWidth();
    int h = gfx.getHeight();

    out << "P6\n" << w << " " << h << "\n255\n";

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            uint8_t color_idx = gfx.point(x, y) & 0x0F;
            const RGBColor& rgb = CGA_PALETTE[color_idx];
            out.put(static_cast<char>(rgb.r));
            out.put(static_cast<char>(rgb.g));
            out.put(static_cast<char>(rgb.b));
        }
    }
    return true;
}

bool ScreenExport::saveBMP(const std::string& filename, const BasicGraphics& gfx) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    int w = gfx.getWidth();
    int h = gfx.getHeight();

    int row_padded = (w * 3 + 3) & (~3);
    uint32_t image_size = static_cast<uint32_t>(row_padded * h);
    uint32_t file_size = 54 + image_size;

    // Bitmap File Header (14 bytes)
    uint8_t file_header[14] = {
        'B', 'M',
        static_cast<uint8_t>(file_size & 0xFF),
        static_cast<uint8_t>((file_size >> 8) & 0xFF),
        static_cast<uint8_t>((file_size >> 16) & 0xFF),
        static_cast<uint8_t>((file_size >> 24) & 0xFF),
        0, 0, 0, 0, // reserved
        54, 0, 0, 0 // offset to pixel data
    };

    // DIB Header (BITMAPINFOHEADER - 40 bytes)
    uint8_t info_header[40] = {
        40, 0, 0, 0, // header size
        static_cast<uint8_t>(w & 0xFF), static_cast<uint8_t>((w >> 8) & 0xFF),
        static_cast<uint8_t>((w >> 16) & 0xFF), static_cast<uint8_t>((w >> 24) & 0xFF),
        static_cast<uint8_t>(h & 0xFF), static_cast<uint8_t>((h >> 8) & 0xFF),
        static_cast<uint8_t>((h >> 16) & 0xFF), static_cast<uint8_t>((h >> 24) & 0xFF),
        1, 0,        // planes
        24, 0,       // bits per pixel (24-bit BGR)
        0, 0, 0, 0,  // compression (none)
        static_cast<uint8_t>(image_size & 0xFF), static_cast<uint8_t>((image_size >> 8) & 0xFF),
        static_cast<uint8_t>((image_size >> 16) & 0xFF), static_cast<uint8_t>((image_size >> 24) & 0xFF),
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    out.write(reinterpret_cast<char*>(file_header), 14);
    out.write(reinterpret_cast<char*>(info_header), 40);

    std::vector<uint8_t> padding(row_padded - w * 3, 0);

    // BMP rows are stored bottom-to-top
    for (int y = h - 1; y >= 0; --y) {
        for (int x = 0; x < w; ++x) {
            uint8_t color_idx = gfx.point(x, y) & 0x0F;
            const RGBColor& rgb = CGA_PALETTE[color_idx];
            out.put(static_cast<char>(rgb.b)); // BGR order
            out.put(static_cast<char>(rgb.g));
            out.put(static_cast<char>(rgb.r));
        }
        if (!padding.empty()) {
            out.write(reinterpret_cast<char*>(padding.data()), padding.size());
        }
    }

    return true;
}

} // namespace gwbasic
