#pragma once

#include "basic_graphics.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace gwbasic {

/**
 * SDL2 Graphics Window for GW-BASIC
 * Renders CGA/EGA 16-color graphics framebuffer in real-time with pixel scaling.
 */
class SDLWindow {
public:
    explicit SDLWindow(const std::string& title = "GW-BASIC 3.23 (C++ Port - CGA Display)", int scale = 3);
    ~SDLWindow();

    bool init(int width = 320, int height = 200);
    void close();

    bool isOpen() const { return is_open_; }
    void render(const BasicGraphics& gfx);
    void pollEvents();
    void waitKeyOrClose();

    // Check if key is available from SDL window
    std::string popKey();
    bool hasKey() const { return !key_queue_.empty(); }

private:
    std::string title_;
    int scale_;
    int width_ = 320;
    int height_ = 200;
    bool is_open_ = false;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
    std::vector<uint32_t> pixel_buffer_;
    std::vector<std::string> key_queue_;

    static const uint32_t CGA_RGBA_PALETTE[16];
};

} // namespace gwbasic
