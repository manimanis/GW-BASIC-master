#include "basic_graphics.h"
#include "cga_font.h"
#include <queue>
#include <algorithm>
#include <cstring>

namespace gwbasic {

BasicGraphics::BasicGraphics(int width, int height, ScreenMode mode)
    : width_(width), height_(height), mode_(mode),
      framebuffer_(static_cast<size_t>(width * height), 0) {}

void BasicGraphics::setMode(ScreenMode mode) {
    dirty_ = true;
    mode_ = mode;
    switch (mode) {
        case ScreenMode::Text:
            width_ = 80;
            height_ = 25;
            break;
        case ScreenMode::MediumRes:
            width_ = 320;
            height_ = 200;
            break;
        case ScreenMode::HighRes:
            width_ = 640;
            height_ = 200;
            break;
    }
    cls(0);
}

void BasicGraphics::cls(uint8_t bg_color) {
    dirty_ = true;
    background_color_ = bg_color;
    framebuffer_.assign(static_cast<size_t>(width_ * height_), bg_color);
    cursor_x_ = 0;
    cursor_y_ = 0;
}

bool BasicGraphics::isValidCoord(int x, int y) const {
    return (x >= 0 && x < width_ && y >= 0 && y < height_);
}

void BasicGraphics::pset(int x, int y, uint8_t color) {
    dirty_ = true;
    cursor_x_ = x;
    cursor_y_ = y;
    if (isValidCoord(x, y)) {
        framebuffer_[static_cast<size_t>(y * width_ + x)] = color;
    }
}

void BasicGraphics::preset(int x, int y) {
    pset(x, y, background_color_);
}

uint8_t BasicGraphics::point(int x, int y) const {
    if (!isValidCoord(x, y)) {
        return 0;
    }
    return framebuffer_[static_cast<size_t>(y * width_ + x)];
}

// Bresenham's line algorithm with 16-bit style pattern mask (GENGRP.ASM)
void BasicGraphics::line(int x1, int y1, int x2, int y2, uint8_t color, uint16_t style) {
    dirty_ = true;
    cursor_x_ = x2;
    cursor_y_ = y2;

    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int pattern_bit = 0;

    int cur_x = x1;
    int cur_y = y1;

    while (true) {
        // Apply 16-bit style mask (bit 15 to 0)
        bool draw_pixel = (style & (1 << (15 - (pattern_bit % 16)))) != 0;
        if (draw_pixel && isValidCoord(cur_x, cur_y)) {
            framebuffer_[static_cast<size_t>(cur_y * width_ + cur_x)] = color;
        }
        pattern_bit++;

        if (cur_x == x2 && cur_y == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            cur_x += sx;
        }
        if (e2 < dx) {
            err += dx;
            cur_y += sy;
        }
    }
}

// Box / Box-Fill operations
void BasicGraphics::box(int x1, int y1, int x2, int y2, uint8_t color, bool filled) {
    int xmin = std::min(x1, x2);
    int xmax = std::max(x1, x2);
    int ymin = std::min(y1, y2);
    int ymax = std::max(y1, y2);

    if (filled) {
        for (int y = ymin; y <= ymax; ++y) {
            line(xmin, y, xmax, y, color);
        }
    } else {
        line(xmin, ymin, xmax, ymin, color);
        line(xmax, ymin, xmax, ymax, color);
        line(xmax, ymax, xmin, ymax, color);
        line(xmin, ymax, xmin, ymin, color);
    }
}

// Midpoint circle/ellipse drawing algorithm (ADVGRP.ASM)
void BasicGraphics::circle(int xc, int yc, int radius, uint8_t color, 
                           double start_angle, double end_angle, double aspect) {
    dirty_ = true;
    if (radius <= 0) return;

    // Approximate circle with steps based on radius, connecting points with lines
    // to guarantee an unbroken boundary for flood-fill (PAINT).
    int num_steps = std::max(64, radius * 4);
    double step = (2.0 * BASIC_PI) / num_steps;

    int prev_x = -1, prev_y = -1;
    bool has_prev = false;

    for (int i = 0; i <= num_steps; ++i) {
        double theta = i * step;
        if (theta >= start_angle && theta <= end_angle) {
            int px = static_cast<int>(std::round(xc + radius * std::cos(theta)));
            int py = static_cast<int>(std::round(yc + (radius * aspect) * std::sin(theta)));
            if (has_prev) {
                line(prev_x, prev_y, px, py, color);
            } else if (isValidCoord(px, py)) {
                framebuffer_[static_cast<size_t>(py * width_ + px)] = color;
            }
            prev_x = px;
            prev_y = py;
            has_prev = true;
        } else {
            has_prev = false;
        }
    }
}

// Scanline flood-fill algorithm corresponding to PAINT in ADVGRP.ASM (lines 165-250)
void BasicGraphics::paint(int start_x, int start_y, uint8_t fill_color, uint8_t border_color) {
    dirty_ = true;
    if (!isValidCoord(start_x, start_y)) {
        return;
    }

    uint8_t start_col = point(start_x, start_y);
    if (start_col == border_color || start_col == fill_color) {
        return; // Started on border or already filled
    }

    struct Span {
        int left;
        int right;
        int y;
        int dir; // -1 for UP, +1 for DOWN
    };

    std::queue<Span> q;

    // Initial scan left and right from starting point
    int left = start_x;
    while (left > 0 && point(left - 1, start_y) != border_color && point(left - 1, start_y) != fill_color) {
        left--;
    }
    int right = start_x;
    while (right < width_ - 1 && point(right + 1, start_y) != border_color && point(right + 1, start_y) != fill_color) {
        right++;
    }

    for (int x = left; x <= right; ++x) {
        framebuffer_[static_cast<size_t>(start_y * width_ + x)] = fill_color;
    }

    q.push({left, right, start_y, 1});
    q.push({left, right, start_y, -1});

    while (!q.empty()) {
        Span span = q.front();
        q.pop();

        int target_y = span.y + span.dir;
        if (target_y < 0 || target_y >= height_) continue;

        int curr_x = span.left;
        while (curr_x <= span.right) {
            if (point(curr_x, target_y) != border_color && point(curr_x, target_y) != fill_color) {
                int seg_left = curr_x;
                while (seg_left > 0 && point(seg_left - 1, target_y) != border_color && point(seg_left - 1, target_y) != fill_color) {
                    seg_left--;
                }
                int seg_right = curr_x;
                while (seg_right < width_ - 1 && point(seg_right + 1, target_y) != border_color && point(seg_right + 1, target_y) != fill_color) {
                    seg_right++;
                }

                for (int px = seg_left; px <= seg_right; ++px) {
                    framebuffer_[static_cast<size_t>(target_y * width_ + px)] = fill_color;
                }

                q.push({seg_left, seg_right, target_y, span.dir});

                // Check for overhang in opposite direction (PNTLP6 in ADVGRP.ASM)
                if (seg_left < span.left || seg_right > span.right) {
                    q.push({seg_left, seg_right, target_y, -span.dir});
                }

                curr_x = seg_right + 1;
            } else {
                curr_x++;
            }
        }
    }
}

void BasicGraphics::draw(const std::string& commands) {
    dirty_ = true;
    size_t i = 0;
    size_t len = commands.size();

    auto skip_spaces = [&]() {
        while (i < len && (std::isspace(static_cast<unsigned char>(commands[i])) || commands[i] == ';')) {
            i++;
        }
    };

    auto parse_int = [&](int default_val = 1) -> int {
        skip_spaces();
        if (i >= len) return default_val;
        bool negative = false;
        if (commands[i] == '-') {
            negative = true;
            i++;
        } else if (commands[i] == '+') {
            i++;
        }
        if (i >= len || !std::isdigit(static_cast<unsigned char>(commands[i]))) {
            return default_val;
        }
        int val = 0;
        while (i < len && std::isdigit(static_cast<unsigned char>(commands[i]))) {
            val = val * 10 + (commands[i] - '0');
            i++;
        }
        return negative ? -val : val;
    };

    while (i < len) {
        skip_spaces();
        if (i >= len) break;

        bool blind = false;
        bool no_update = false;

        // Check prefixes B and N
        while (i < len) {
            char p = std::toupper(static_cast<unsigned char>(commands[i]));
            if (p == 'B') {
                blind = true;
                i++;
                skip_spaces();
            } else if (p == 'N') {
                no_update = true;
                i++;
                skip_spaces();
            } else {
                break;
            }
        }

        if (i >= len) break;
        char cmd = std::toupper(static_cast<unsigned char>(commands[i++]));
        int start_x = cursor_x_;
        int start_y = cursor_y_;
        int target_x = cursor_x_;
        int target_y = cursor_y_;
        bool is_motion = false;

        switch (cmd) {
            case 'U': { // Up
                int dist = parse_int(1);
                target_y -= dist;
                is_motion = true;
                break;
            }
            case 'D': { // Down
                int dist = parse_int(1);
                target_y += dist;
                is_motion = true;
                break;
            }
            case 'L': { // Left
                int dist = parse_int(1);
                target_x -= dist;
                is_motion = true;
                break;
            }
            case 'R': { // Right
                int dist = parse_int(1);
                target_x += dist;
                is_motion = true;
                break;
            }
            case 'E': { // Up and Right
                int dist = parse_int(1);
                target_x += dist;
                target_y -= dist;
                is_motion = true;
                break;
            }
            case 'F': { // Down and Right
                int dist = parse_int(1);
                target_x += dist;
                target_y += dist;
                is_motion = true;
                break;
            }
            case 'G': { // Down and Left
                int dist = parse_int(1);
                target_x -= dist;
                target_y += dist;
                is_motion = true;
                break;
            }
            case 'H': { // Up and Left
                int dist = parse_int(1);
                target_x -= dist;
                target_y -= dist;
                is_motion = true;
                break;
            }
            case 'M': { // Move absolute or relative
                skip_spaces();
                bool rel_x = false;
                if (i < len && (commands[i] == '+' || commands[i] == '-')) {
                    rel_x = true;
                }
                int mx = parse_int(0);
                skip_spaces();
                if (i < len && commands[i] == ',') i++;
                skip_spaces();
                bool rel_y = false;
                if (i < len && (commands[i] == '+' || commands[i] == '-')) {
                    rel_y = true;
                }
                int my = parse_int(0);

                target_x = rel_x ? (start_x + mx) : mx;
                target_y = rel_y ? (start_y + my) : my;
                is_motion = true;
                break;
            }
            case 'C': { // Color
                int c = parse_int(1);
                current_attribute_ = static_cast<uint8_t>(c);
                break;
            }
            default:
                // Skip unknown command or letter
                break;
        }

        if (is_motion) {
            if (!blind) {
                line(start_x, start_y, target_x, target_y, current_attribute_);
            }
            if (no_update) {
                cursor_x_ = start_x;
                cursor_y_ = start_y;
            } else {
                cursor_x_ = target_x;
                cursor_y_ = target_y;
            }
        }
    }
}

BasicGraphics::GraphicImage BasicGraphics::getImage(int x1, int y1, int x2, int y2) const {
    int min_x = std::min(x1, x2);
    int max_x = std::max(x1, x2);
    int min_y = std::min(y1, y2);
    int max_y = std::max(y1, y2);

    int w = max_x - min_x + 1;
    int h = max_y - min_y + 1;

    GraphicImage img;
    img.width = w;
    img.height = h;
    img.pixels.resize(static_cast<size_t>(w * h), 0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            img.pixels[static_cast<size_t>(y * w + x)] = point(min_x + x, min_y + y);
        }
    }
    return img;
}

void BasicGraphics::putImage(int x, int y, const GraphicImage& img, PutAction action) {
    if (img.width <= 0 || img.height <= 0) return;
    dirty_ = true;

    for (int r = 0; r < img.height; ++r) {
        int py = y + r;
        if (py < 0 || py >= height_) continue;
        for (int c = 0; c < img.width; ++c) {
            int px = x + c;
            if (px < 0 || px >= width_) continue;

            uint8_t src = img.pixels[static_cast<size_t>(r * img.width + c)];
            size_t idx = static_cast<size_t>(py * width_ + px);
            uint8_t dest = framebuffer_[idx];

            uint8_t result = dest;
            switch (action) {
                case PutAction::Pset:
                    result = src;
                    break;
                case PutAction::Preset:
                    result = (mode_ == ScreenMode::HighRes) ? ((~src) & 0x01) : ((~src) & 0x03);
                    break;
                case PutAction::Xor:
                    result = dest ^ src;
                    break;
                case PutAction::Or:
                    result = dest | src;
                    break;
                case PutAction::And:
                    result = dest & src;
                    break;
            }
            framebuffer_[idx] = result;
        }
    }
}

void BasicGraphics::storeImage(const std::string& name, const GraphicImage& img) {
    std::string uname = name;
    std::transform(uname.begin(), uname.end(), uname.begin(), ::toupper);
    images_[uname] = img;
}

bool BasicGraphics::hasImage(const std::string& name) const {
    std::string uname = name;
    std::transform(uname.begin(), uname.end(), uname.begin(), ::toupper);
    return images_.find(uname) != images_.end();
}

const BasicGraphics::GraphicImage* BasicGraphics::getImage(const std::string& name) const {
    std::string uname = name;
    std::transform(uname.begin(), uname.end(), uname.begin(), ::toupper);
    auto it = images_.find(uname);
    if (it != images_.end()) {
        return &it->second;
    }
    return nullptr;
}

void BasicGraphics::drawChar(int x, int y, uint8_t ch, uint8_t fg_color, uint8_t bg_color, bool transparent_bg) {
    const uint8_t* glyph = CGA_FONT_8X8[ch];
    for (int r = 0; r < 8; ++r) {
        int py = y + r;
        if (py < 0 || py >= height_) continue;
        uint8_t bits = glyph[r];
        for (int c = 0; c < 8; ++c) {
            int px = x + c;
            if (px < 0 || px >= width_) continue;
            bool is_on = ((bits >> (7 - c)) & 1) != 0;
            if (is_on) {
                framebuffer_[static_cast<size_t>(py * width_ + px)] = fg_color;
            } else if (!transparent_bg) {
                framebuffer_[static_cast<size_t>(py * width_ + px)] = bg_color;
            }
        }
    }
    dirty_ = true;
}

void BasicGraphics::scrollUp(int pixels) {
    if (pixels <= 0 || pixels >= height_) return;
    size_t copy_bytes = static_cast<size_t>((height_ - pixels) * width_);
    std::memmove(&framebuffer_[0], &framebuffer_[static_cast<size_t>(pixels * width_)], copy_bytes);
    std::fill(framebuffer_.begin() + copy_bytes, framebuffer_.end(), background_color_);
    dirty_ = true;
}

void BasicGraphics::printString(const std::string& text, int& row, int& col, uint8_t fg_color, uint8_t bg_color) {
    int max_cols = (mode_ == ScreenMode::MediumRes) ? 40 : 80;
    int max_rows = 25;

    for (size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (c == '\r') {
            col = 1;
        } else if (c == '\n') {
            col = 1;
            row++;
            if (row > max_rows) {
                scrollUp(8);
                row = max_rows;
            }
        } else if (c == '\t') {
            // Tab stop every 14 columns in GW-BASIC standard
            col = ((col - 1) / 14 + 1) * 14 + 1;
            if (col > max_cols) {
                col = 1;
                row++;
                if (row > max_rows) {
                    scrollUp(8);
                    row = max_rows;
                }
            }
        } else if (c == '\x08') { // Backspace
            if (col > 1) col--;
        } else {
            if (row < 1) row = 1;
            if (col < 1) col = 1;
            int px = (col - 1) * 8;
            int py = (row - 1) * 8;
            drawChar(px, py, static_cast<uint8_t>(c), fg_color, bg_color, false);
            col++;
            if (col > max_cols) {
                col = 1;
                row++;
                if (row > max_rows) {
                    scrollUp(8);
                    row = max_rows;
                }
            }
        }
    }
    dirty_ = true;
}

} // namespace gwbasic
