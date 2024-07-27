#pragma once

#include <string>

class Screen {
    int width_;
    int height_;
    uint32_t *buffer_;
    std::vector<std::string> texts_;

public:
    Screen() : buffer_(nullptr) { }
    Screen(int width, int height, bool fullscreen, std::string title);
    ~Screen();

    uint32_t GetBufColor(int x, int y) const;
    void SetBufColor(int x, int y, uint32_t color);
    void Update();
    template<typename T>
    void Print(const T &vaml);

    static void GetDefaultResolution(int &width, int &height);
};
