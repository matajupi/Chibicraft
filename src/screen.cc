#include "screen.h"

#include <cassert>
#include <sstream>
#include <X11/Xlib.h>

#include "quickcg.h"

Screen::Screen(int width, int height, bool fullscreen, std::string title)
    : width_(width), height_(height) {

    QuickCG::screen(width_, height_, fullscreen, title);
    SDL_ShowCursor(false);

    // Stack overflowを起こすので、Heap領域にメモリを確保
    buffer_ = new uint32_t[height_ * width_];
}

Screen::~Screen() {
    if (buffer_) {
        delete buffer_;
    }
}

uint32_t Screen::GetBufColor(int x, int y) const {
    assert(0 <= x && x < width_ && 0 <= y && y < height_);
    return buffer_[width_ * y + x];
}

void Screen::SetBufColor(int x, int y, uint32_t color) {
    assert(0 <= x && x < width_ && 0 <= y && y < height_);
    buffer_[width_ * y + x] = color;
}

void Screen::Update() {
    QuickCG::drawBuffer(buffer_);
    for (auto i = 0; texts_.size() > i; i++) {
        QuickCG::print(texts_[i], 0, 20 * i);
    }
    QuickCG::redraw();
    texts_.clear();
}

template<typename T>
void Screen::Print(const T &val) {
    std::stringstream ss;
    ss << val;
    texts_.push_back(ss.str());
}

void Screen::GetScreenResolution(int &width, int &height) {
    Display* disp = XOpenDisplay(NULL);
    Screen*  scrn = DefaultScreenOfDisplay(disp);

    width = scrn->width;
    height = scrn->height;

    XCloseDisplay(disp);
}
