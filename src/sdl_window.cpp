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
    SDL_StartTextInput();
    return true;
}

void SDLWindow::close() {
    if (!is_open_) return;
    is_open_ = false;
    SDL_StopTextInput();
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

        // Text input handles printable characters (including AZERTY keys, accented chars, NumPad digits)
        if (event.type == SDL_TEXTINPUT) {
            const char* ptr = event.text.text;
            while (*ptr) {
                unsigned char b1 = static_cast<unsigned char>(*ptr);
                if (b1 < 0x80) {
                    // Standard 7-bit ASCII
                    key_queue_.push_back(std::string(1, static_cast<char>(b1)));
                    ptr++;
                } else if ((b1 & 0xE0) == 0xC0 && *(ptr + 1)) {
                    // 2-byte UTF-8 sequence (French accents etc.)
                    unsigned char b2 = static_cast<unsigned char>(*(ptr + 1));
                    uint16_t unicode = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                    ptr += 2;
                    // Convert common Latin-1 / CP437 accented characters
                    char cp437 = '\0';
                    switch (unicode) {
                        case 0x00E9: cp437 = static_cast<char>(0x82); break; // é
                        case 0x00E0: cp437 = static_cast<char>(0x85); break; // à
                        case 0x00E8: cp437 = static_cast<char>(0x8A); break; // è
                        case 0x00F9: cp437 = static_cast<char>(0x97); break; // ù
                        case 0x00E2: cp437 = static_cast<char>(0x83); break; // â
                        case 0x00EA: cp437 = static_cast<char>(0x88); break; // ê
                        case 0x00EE: cp437 = static_cast<char>(0x8C); break; // î
                        case 0x00F4: cp437 = static_cast<char>(0x93); break; // ô
                        case 0x00FB: cp437 = static_cast<char>(0x96); break; // û
                        case 0x00E7: cp437 = static_cast<char>(0x87); break; // ç
                        case 0x00EB: cp437 = static_cast<char>(0x89); break; // ë
                        case 0x00EF: cp437 = static_cast<char>(0x8B); break; // ï
                        case 0x00FC: cp437 = static_cast<char>(0x81); break; // ü
                        case 0x00E4: cp437 = static_cast<char>(0x84); break; // ä
                        case 0x00F6: cp437 = static_cast<char>(0x94); break; // ö
                        case 0x00C9: cp437 = static_cast<char>(0x90); break; // É
                        case 0x00C0: cp437 = static_cast<char>(0x41); break; // À -> A
                        case 0x00C8: cp437 = static_cast<char>(0x45); break; // È -> E
                        case 0x00C7: cp437 = static_cast<char>(0x80); break; // Ç
                        case 0x00B0: cp437 = static_cast<char>(0xF8); break; // °
                        default:
                            if (unicode < 256) cp437 = static_cast<char>(unicode);
                            break;
                    }
                    if (cp437 != '\0') {
                        key_queue_.push_back(std::string(1, cp437));
                    }
                } else {
                    ptr++;
                }
            }
        }

        // Key down handles control keys and extended keystrokes (arrows, function keys)
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode sym = event.key.keysym.sym;
            Uint16 mod = event.key.keysym.mod;

            if (sym == SDLK_ESCAPE) {
                // CHR$(27) for GW-BASIC
                key_queue_.push_back("\x1B");
            } else if (sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
                key_queue_.push_back("\r");
            } else if (sym == SDLK_BACKSPACE) {
                key_queue_.push_back("\x08");
            } else if (sym == SDLK_TAB) {
                key_queue_.push_back("\t");
            } else if (sym == SDLK_UP) {
                key_queue_.push_back(std::string("\0H", 2));
            } else if (sym == SDLK_DOWN) {
                key_queue_.push_back(std::string("\0P", 2));
            } else if (sym == SDLK_LEFT) {
                key_queue_.push_back(std::string("\0K", 2));
            } else if (sym == SDLK_RIGHT) {
                key_queue_.push_back(std::string("\0M", 2));
            } else if (sym == SDLK_HOME) {
                key_queue_.push_back(std::string("\0G", 2));
            } else if (sym == SDLK_END) {
                key_queue_.push_back(std::string("\0O", 2));
            } else if (sym == SDLK_PAGEUP) {
                key_queue_.push_back(std::string("\0I", 2));
            } else if (sym == SDLK_PAGEDOWN) {
                key_queue_.push_back(std::string("\0Q", 2));
            } else if (sym == SDLK_INSERT) {
                key_queue_.push_back(std::string("\0R", 2));
            } else if (sym == SDLK_DELETE) {
                key_queue_.push_back(std::string("\0S", 2));
            } else if (sym >= SDLK_F1 && sym <= SDLK_F10) {
                char scan = static_cast<char>(59 + (sym - SDLK_F1));
                key_queue_.push_back(std::string{'\0', scan});
            } else if (sym == SDLK_F11) {
                key_queue_.push_back(std::string{'\0', static_cast<char>(133)});
            } else if (sym == SDLK_F12) {
                key_queue_.push_back(std::string{'\0', static_cast<char>(134)});
            } else if ((mod & KMOD_CTRL) && sym >= SDLK_a && sym <= SDLK_z) {
                char c = static_cast<char>(sym - SDLK_a + 1);
                key_queue_.push_back(std::string(1, c));
            } else if (!(mod & KMOD_NUM)) {
                // Keypad navigation with NumLock off
                if (sym == SDLK_KP_8) key_queue_.push_back(std::string("\0H", 2));
                else if (sym == SDLK_KP_2) key_queue_.push_back(std::string("\0P", 2));
                else if (sym == SDLK_KP_4) key_queue_.push_back(std::string("\0K", 2));
                else if (sym == SDLK_KP_6) key_queue_.push_back(std::string("\0M", 2));
                else if (sym == SDLK_KP_7) key_queue_.push_back(std::string("\0G", 2));
                else if (sym == SDLK_KP_1) key_queue_.push_back(std::string("\0O", 2));
                else if (sym == SDLK_KP_9) key_queue_.push_back(std::string("\0I", 2));
                else if (sym == SDLK_KP_3) key_queue_.push_back(std::string("\0Q", 2));
                else if (sym == SDLK_KP_0) key_queue_.push_back(std::string("\0R", 2));
                else if (sym == SDLK_KP_PERIOD) key_queue_.push_back(std::string("\0S", 2));
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
