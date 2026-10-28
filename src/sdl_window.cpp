#include "sdl_window.h"
#include <cctype>
#include <algorithm>

namespace gwbasic {

// CGA 16-color palette as RGBA (for SDL2 ARGB8888 texture)
// Format: 0xAARRGGBB
const uint32_t SDLWindow::CGA_RGBA_PALETTE[16] = {
    0xFF000000, // 0: Black
    0xFF0000AA, // 1: Blue
    0xFF00AA00, // 2: Green
    0xFF00AAAA, // 3: Cyan
    0xFFAA0000, // 4: Red
    0xFFAA00AA, // 5: Magenta
    0xFFAA5500, // 6: Brown
    0xFFAAAAAA, // 7: Light Gray
    0xFF555555, // 8: Dark Gray
    0xFF5555FF, // 9: Light Blue
    0xFF55FF55, // 10: Light Green
    0xFF55FFFF, // 11: Light Cyan
    0xFFFF5555, // 12: Light Red
    0xFFFF55FF, // 13: Light Magenta
    0xFFFFFF55, // 14: Yellow
    0xFFFFFFFF  // 15: White
};

SDLWindow::SDLWindow(const std::string& title, int scale)
    : title_(title), scale_(scale) {}

SDLWindow::~SDLWindow() {
    close();
}

bool SDLWindow::init(int width, int height) {
    width_ = width;
    height_ = height;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    window_ = SDL_CreateWindow(
        title_.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width_ * scale_,
        height_ * scale_,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window_) {
        SDL_Quit();
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // Disable linear filtering for crisp pixel look
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(renderer_, width_, height_);

    texture_ = SDL_CreateTexture(
        renderer_,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width_, height_
    );

    if (!texture_) {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    pixel_buffer_.resize(static_cast<size_t>(width_ * height_), 0xFF000000);
    is_open_ = true;
    return true;
}

void SDLWindow::close() {
    if (!is_open_) return;
    is_open_ = false;
    if (texture_) { SDL_DestroyTexture(texture_); texture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    SDL_Quit();
}

void SDLWindow::render(const BasicGraphics& gfx) {
    if (!is_open_) return;

    const auto& fb = gfx.getFramebuffer();
    int w = gfx.getWidth();
    int h = gfx.getHeight();

    // Resize pixel buffer if mode changed
    if (w != width_ || h != height_) {
        width_ = w;
        height_ = h;
        SDL_RenderSetLogicalSize(renderer_, w, h);
        if (texture_) SDL_DestroyTexture(texture_);
        texture_ = SDL_CreateTexture(renderer_,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            w, h);
        pixel_buffer_.resize(static_cast<size_t>(w * h), 0xFF000000);
    }

    // Map framebuffer (1 byte per pixel = CGA color index) → RGBA
    for (int i = 0; i < w * h; ++i) {
        uint8_t color_idx = fb[static_cast<size_t>(i)] & 0x0F;
        pixel_buffer_[static_cast<size_t>(i)] = CGA_RGBA_PALETTE[color_idx];
    }

    SDL_UpdateTexture(texture_, nullptr, pixel_buffer_.data(), w * sizeof(uint32_t));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

void SDLWindow::pollEvents() {
    if (!is_open_) return;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            close();
            return;
        }
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode sym = event.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                close();
                return;
            }
            // Map printable ASCII keys for INKEY$
            if (sym >= SDLK_SPACE && sym <= SDLK_z) {
                char c = static_cast<char>(sym);
                // Apply shift for uppercase
                const Uint8* state = SDL_GetKeyboardState(nullptr);
                if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) {
                    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                }
                key_queue_.push_back(std::string(1, c));
            } else if (sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
                key_queue_.push_back("\r");
            } else if (sym == SDLK_BACKSPACE) {
                key_queue_.push_back("\x08");
            }
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                // Re-render on resize
            }
        }
    }
}

void SDLWindow::waitKeyOrClose() {
    if (!is_open_) return;

    SDL_Event event;
    while (is_open_) {
        if (SDL_WaitEventTimeout(&event, 100)) {
            if (event.type == SDL_QUIT) {
                close();
                return;
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode sym = event.key.keysym.sym;
                if (sym == SDLK_ESCAPE || sym == SDLK_RETURN || sym == SDLK_KP_ENTER || sym == SDLK_SPACE) {
                    return;
                }
            }
        }
    }
}

std::string SDLWindow::popKey() {
    if (key_queue_.empty()) return "";
    std::string key = key_queue_.front();
    key_queue_.erase(key_queue_.begin());
    return key;
}

} // namespace gwbasic
