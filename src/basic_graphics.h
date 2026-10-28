#pragma once

#include "basic_types.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cmath>

namespace gwbasic {

enum class ScreenMode {
    Text = 0,         // SCREEN 0: Text 80x25
    MediumRes = 1,    // SCREEN 1: 320x200 (4 colors CGA)
    HighRes = 2       // SCREEN 2: 640x200 (2 colors Monochrome)
};

/**
 * C++ implementation of GW-BASIC Graphics subsystem (GENGRP.ASM & ADVGRP.ASM).
 * Implements Bresenham Line, Box/Box-Fill, Midpoint Circle/Ellipse, and Scanline Flood Fill (PAINT).
 */
class BasicGraphics {
public:
    explicit BasicGraphics(int width = 320, int height = 200, ScreenMode mode = ScreenMode::MediumRes);

    void setMode(ScreenMode mode);
    ScreenMode getMode() const { return mode_; }

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    // Screen clearing
    void cls(uint8_t bg_color = 0);

    // Pixel operations (GENGRP.ASM)
    void pset(int x, int y, uint8_t color);
    void preset(int x, int y);
    uint8_t point(int x, int y) const;

    // Line and Box operations (GENGRP.ASM)
    void line(int x1, int y1, int x2, int y2, uint8_t color, uint16_t style = 0xFFFF);
    void box(int x1, int y1, int x2, int y2, uint8_t color, bool filled = false);

    static constexpr double BASIC_PI = 3.14159265358979323846;

    // Circle and Ellipse operations (ADVGRP.ASM)
    void circle(int xc, int yc, int radius, uint8_t color, 
                double start_angle = 0.0, double end_angle = 2.0 * BASIC_PI, double aspect = 1.0);

    // Flood Fill (PAINT statement in ADVGRP.ASM)
    void paint(int start_x, int start_y, uint8_t fill_color, uint8_t border_color);

    // DRAW macro language interpreter (ADVGRP.ASM & MACLNG.ASM)
    void draw(const std::string& commands);

    // GET and PUT graphics operations (ADVGRP.ASM)
    struct GraphicImage {
        int width = 0;
        int height = 0;
        std::vector<uint8_t> pixels; // size = width * height
    };

    enum class PutAction {
        Xor,
        Pset,
        Preset,
        Or,
        And
    };

    GraphicImage getImage(int x1, int y1, int x2, int y2) const;
    void putImage(int x, int y, const GraphicImage& img, PutAction action = PutAction::Xor);

    void storeImage(const std::string& name, const GraphicImage& img);
    bool hasImage(const std::string& name) const;
    const GraphicImage* getImage(const std::string& name) const;

    // Character and text rendering (SCNDRV.ASM)
    void drawChar(int x, int y, uint8_t ch, uint8_t fg_color, uint8_t bg_color, bool transparent_bg = false);
    void printString(const std::string& text, int& row, int& col, uint8_t fg_color, uint8_t bg_color);
    void scrollUp(int pixels);

    // Framebuffer access
    const std::vector<uint8_t>& getFramebuffer() const { return framebuffer_; }
    bool isValidCoord(int x, int y) const;

    // Dirty flag: set when any drawing operation modifies the framebuffer
    bool isDirty() const { return dirty_; }
    void clearDirty() { dirty_ = false; }
    void markDirty() { dirty_ = true; }

    // Current graphics cursor position
    int getCursorX() const { return cursor_x_; }
    int getCursorY() const { return cursor_y_; }
    void setCursor(int x, int y) { cursor_x_ = x; cursor_y_ = y; }

private:
    int width_;
    int height_;
    ScreenMode mode_;
    int cursor_x_ = 0;
    int cursor_y_ = 0;
    uint8_t current_attribute_ = 3;
    uint8_t background_color_ = 0;
    std::vector<uint8_t> framebuffer_;
    std::unordered_map<std::string, GraphicImage> images_;
    bool dirty_ = false;
};

} // namespace gwbasic
