#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace chibicraft {

class Screen {
public:
    Screen(int width, int height, bool fullscreen, std::string title);

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

    uint32_t GetColor(int x, int y) const;
    void SetColor(int x, int y, uint32_t color);

    void DrawCursor();
    template<typename T>
    void Print(const T &val) {
        std::stringstream ss;
        ss << val;
        msgs_.push_back(ss.str());
    }

    void Update();

    ~Screen();

    static void GetDefaultResolution(int &width, int &height);

private:
    int width_;
    int height_;

    uint32_t *buffer_;
    std::vector<std::string> msgs_;
};

}
