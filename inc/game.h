#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <glm/glm.hpp>

class Game {
public:
    Game() : Game(-1, -1, false) { }
    Game(int screen_width, int screen_height, bool fullscreen = true);
    void Start();

private:
    static constexpr const int kNTexs = 8;

    static constexpr const int kTexWidth = 64;
    static constexpr const int kTexHeight = 64;

    static constexpr const int kMapWidth = 26;
    static constexpr const int kMapDepth = 26;
    static constexpr const int kMapHeight = 8;

    static constexpr const int kMaxRayDist = 20;

    static constexpr const float kPlayerHalfWidth = 0.3;
    static constexpr const float kPlayerHalfDepth = 0.3;
    static constexpr const float kPlayerUpperHalfHeight = 0.3;
    static constexpr const float kPlayerLowerHalfHeight = 1.3;

    char world_map_[kMapHeight * kMapWidth * kMapDepth];

    char GetMapBlock(int x, int y, int z) const {
        assert(0 <= x && x < kMapWidth && 0 <= y && y < kMapHeight &&
            0 <= z && z < kMapDepth);
        return world_map_[y * kMapWidth * kMapDepth + x * kMapDepth + z];
    }

    int screen_width_;
    int screen_height_;
    bool fullscreen_;

    std::vector<std::vector<uint32_t>> texs_;
    // static constexpr const int kTransparentBlock = 255;
    static constexpr const int kTransparentBlock = 9;

    uint32_t GetTexColor(int tex, int x, int y) {
        assert(0 <= tex && tex < kNTexs && 0 <= x && x < kTexWidth &&
            0 <= y && y < kTexHeight);
        return texs_[tex][kTexWidth * y + x];
    }

    glm::vec3 pos_, dir_, plane_x_, plane_y_;

    float time_;
    float old_time_;
    float frame_time_;

    uint32_t *buffer_;

    uint32_t GetBufColor(int x, int y) const {
        assert(0 <= x && x < screen_width_ && 0 <= y && y < screen_height_);
        return buffer_[screen_width_ * y + x];
    }
    void SetBufColor(int x, int y, uint32_t color) {
        assert(0 <= x && x < screen_width_ && 0 <= y && y < screen_height_);
        buffer_[screen_width_ * y + x] = color;
    }

    void LoadMap(int mid);
    void LoadTexs();

    void Init();
    void InitScreen();
    void InitRaycaster();

    void Update();
    void Raycasting();

    bool CanRotateY(float angle);
    bool CanMoveX(float mvx);
    bool CanMoveY(float mvy);
    bool CanMoveZ(float mvz);

    void HandleInput();

    void Quit();
};

