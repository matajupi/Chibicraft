#include "screen.h"

#include <cassert>
#include <X11/Xlib.h>

#include "quickcg.h"

namespace chibicraft {

static const int kCursorHeight = 30;
static const int kCursorWidth = 30;

const char kCursorShape[kCursorHeight][kCursorWidth + 1] = {
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
    "              @@              ",
};

Screen::Screen(int width, int height, bool fullscreen, std::string title)
    : width_(width), height_(height) {
    QuickCG::screen(width_, height_, fullscreen, title);
    SDL_ShowCursor(false);

    // Stack overflowを起こすので、Heap領域にメモリを確保
    buffer_ = new uint32_t[height_ * width_];
}

uint32_t Screen::GetColor(int x, int y) const {
    assert(0 <= x && x < width_ && 0 <= y && y < height_);
    return buffer_[width_ * y + x];
}

void Screen::SetColor(int x, int y, uint32_t color) {
    assert(0 <= x && x < width_ && 0 <= y && y < height_);
    buffer_[width_ * y + x] = color;
}

void Screen::DrawCursor() {
    for (int y = 0; kCursorHeight > y; y++) {
        for (int x = 0; kCursorWidth > x; x++) {
            if (kCursorShape[y][x] == '@') {
                SetColor(
                    x + width_ / 2 - kCursorWidth / 2,
                    y + height_ / 2 - kCursorHeight / 2,
                    0x19959c
                );
            }
        }
    }
}

void Screen::Update() {
    QuickCG::drawBuffer(buffer_);
    for (size_t i = 0; msgs_.size() > i; i++) {
        QuickCG::print(msgs_[i], 0, 20 * i);
    }
    QuickCG::redraw();
    msgs_.clear();
}

Screen::~Screen() {
    if (buffer_ != nullptr) {
        delete buffer_;
    }
}

void Screen::GetDefaultResolution(int &width, int &height) {
    ::Display* disp = XOpenDisplay(NULL);
    ::Screen*  scrn = DefaultScreenOfDisplay(disp);

    width = scrn->width;
    height = scrn->height;

    XCloseDisplay(disp);
}

}
