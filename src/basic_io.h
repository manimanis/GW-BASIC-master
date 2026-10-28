#pragma once

#include "basic_types.h"
#include "basic_graphics.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <cstdint>

namespace gwbasic {

enum class FileMode {
    Input,
    Output,
    Append,
    Random
};

struct FileHandle {
    int filenum;
    std::string filename;
    FileMode mode;
    std::unique_ptr<std::fstream> stream;
    size_t record_length = 128;
    int64_t current_record = 1;
};

/**
 * Disk and File Management (GIODSK.ASM & DSKCOM.ASM)
 */
class FileManager {
public:
    static constexpr int MAX_FILES = 15;

    FileManager() = default;
    ~FileManager();

    // File operations
    void openFile(int filenum, const std::string& filename, FileMode mode, size_t reclen = 128);
    void closeFile(int filenum);
    void closeAll();

    bool isFileOpen(int filenum) const;
    FileMode getMode(int filenum) const;

    // Sequential I/O
    void writeString(int filenum, const std::string& text);
    std::string readLine(int filenum);
    std::string readToken(int filenum);

    // File info functions (GIODSK.ASM)
    bool isEOF(int filenum);
    int64_t getLOF(int filenum);
    int64_t getLOC(int filenum);

private:
    std::unordered_map<int, FileHandle> open_files_;
    void validateFileNum(int filenum) const;
};

/**
 * Keyboard and Console I/O Management (GIOKYB.ASM & SCNDRV.ASM)
 */
class ConsoleIO {
public:
    ConsoleIO();

    // INKEY$ function (GIOKYB.ASM)
    std::string inkey();
    void pushKey(const std::string& key); // For testing or simulated typing

    // Text cursor and screen state (SCNDRV.ASM)
    void locate(int row, int col);
    int getCsrlin() const { return cursor_row_; }
    int getPos() const { return cursor_col_; }

    void setColor(uint8_t fg, uint8_t bg = 0, uint8_t border = 0);
    uint8_t getForeground() const { return fg_color_; }
    uint8_t getBackground() const { return bg_color_; }
    uint8_t getBorder() const { return border_color_; }

    void setWidth(int width);
    int getWidth() const { return text_width_; }

private:
    std::string key_buffer_;
    int cursor_row_ = 1;
    int cursor_col_ = 1;
    int text_width_ = 80;
    uint8_t fg_color_ = 7;
    uint8_t bg_color_ = 0;
    uint8_t border_color_ = 0;
};

/**
 * Screen Driver and Graphics Export (SCNDRV.ASM)
 * Allows exporting the CGA framebuffer to standard BMP or PPM image files.
 */
class ScreenExport {
public:
    struct RGBColor {
        uint8_t r, g, b;
    };

    static const RGBColor CGA_PALETTE[16];

    // Exports framebuffer to Windows 24-bit Bitmap (BMP)
    static bool saveBMP(const std::string& filename, const BasicGraphics& gfx);

    // Exports framebuffer to Netpbm Portable Pixmap (PPM)
    static bool savePPM(const std::string& filename, const BasicGraphics& gfx);
};

} // namespace gwbasic
