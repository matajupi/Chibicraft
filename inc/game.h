#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>

static const int kCursorHeight = 30;
static const int kCursorWidth = 30;

class Game {
public:
    Game() : Game(-1, -1, true) { }
    Game(int screen_width, int screen_height, bool fullscreen = true);
    void Start();

private:
    // ======== Map ========
    static constexpr const int kMapWidth = 64;
    static constexpr const int kMapDepth = 64;
    static constexpr const int kMapHeight = 64;

    // AirBlock, TransparentBlockもBlockとして含む
    static constexpr const int kNBlocks = 20;
    static constexpr const int kAirBlock = 0;
    static constexpr const int kTransparentBlock = kNBlocks - 1;

    char world_map_[kMapHeight * kMapWidth * kMapDepth];

    char GetMapBlock(int x, int y, int z) const {
        assert(0 <= x && x < kMapWidth && 0 <= y && y < kMapHeight &&
            0 <= z && z < kMapDepth);
        return world_map_[y * kMapWidth * kMapDepth + x * kMapDepth + z];
    }
    char GetMapBlock(const glm::ivec3 &map_pos) const {
        return GetMapBlock(map_pos.x, map_pos.y, map_pos.z);
    }
    void SetMapBlock(int x, int y, int z, char block) {
        assert(0 <= x && x < kMapWidth && 0 <= y && y < kMapHeight &&
            0 <= z && z < kMapDepth && 0 <= block && block < kNBlocks);
        world_map_[y * kMapWidth * kMapDepth + x * kMapDepth + z] = block;
    }
    void SetMapBlock(const glm::ivec3 &map_pos, char block) {
        SetMapBlock(map_pos.x, map_pos.y, map_pos.z, block);
    }
    std::string ToMapFileName(int mid) {
        std::stringstream ss;
        ss << "res/map/" << std::setfill('0') << std::setw(8)
            << std::hex << mid << ".map";
        return ss.str();
    }
    void LoadMap(int mid);
    void SaveMap(int mid);

    // ======== Textures ========
    static constexpr const int kNTexs = 21;

    static constexpr const int kTexWidth = 16;
    static constexpr const int kTexHeight = 16;

    static const std::string kTexDir;
    static const std::array<std::string, kNTexs> kTexFiles;
    static const std::array<long long int, kNBlocks> kBlockToTexs;
    static const std::array<std::string, kNBlocks> kBlockName;
    std::vector<uint32_t> texs_[kNTexs];

    // face: ブロックの面
    // 0: x+面, 1: x-面, 2: y+面, 3: y-面, 4: z+面, 5: z-面
    int GetTex(int block, int face) const {
        assert(0 <= block && block < kNBlocks && 0 <= face && face < 6);
        return kBlockToTexs[block] >> (face * 8) & 0xff;
    }
    uint32_t GetTexColor(int tex, int x, int y) const {
        assert(0 <= tex && tex < kNTexs && 0 <= x && x < kTexWidth &&
            0 <= y && y < kTexHeight);
        return texs_[tex][kTexWidth * y + x];
    }
    void LoadTexs();

    // ======== Ray ========
    static constexpr const int kMaxRayDist = 60;

    struct Ray {
        glm::vec3 dir;
        glm::ivec3 pos;
        // collision_side: 衝突した面に垂直な軸
        // 0: x軸(面はy-z平面), 1: y軸, 2: z軸
        int collision_side;
        float perp_wall_dist;
        float max_perp_wall_dist;
    };

    // ======== Player ========
    static constexpr const float kPlayerHalfWidth = 0.3;
    static constexpr const float kPlayerHalfDepth = 0.3;
    static constexpr const float kPlayerUpperHalfHeight = 0.1;
    static constexpr const float kPlayerLowerHalfHeight = 1.5;

    static constexpr const float kPlayerPutBlockDist = 5.0;
    static constexpr const float kPlayerDestBlockDist = 5.0;

    glm::vec3 pos_, dir_, plane_x_, plane_y_;
    int select_block_ = 1;

    // ======== Screen ========
    int screen_width_;
    int screen_height_;
    bool fullscreen_;

    // Stack overflowを起こすので、Heap領域にメモリを確保
    uint32_t *buffer_;

    uint32_t GetBufColor(int x, int y) const {
        assert(0 <= x && x < screen_width_ && 0 <= y && y < screen_height_);
        return buffer_[screen_width_ * y + x];
    }
    void SetBufColor(int x, int y, uint32_t color) {
        assert(0 <= x && x < screen_width_ && 0 <= y && y < screen_height_);
        buffer_[screen_width_ * y + x] = color;
    }

    // ======== Time ========
    float time_;
    float old_time_;
    float frame_time_;

    // ======== Input ========
    // QuickCGではMouseのPressが取れないので、flagを持っておく
    bool prev_lmb_;
    bool prev_rmb_;

    void Init();
    void InitScreen();
    void InitPlayer();

    void Update();
    void DrawCursor();
    bool CastRay(int x, int y, Ray &ray) const;
    uint32_t CalcPixelColor(const Ray &ray) const;
    void SimpleRaycasting();
    void SlackOffRaycasting();

    // part: Playerの部位を指定
    // x: 0: x-(Playerのx-面のx座標), 1: x+
    // y: 0: y-, 1: y(Playerの中心のy座標), 2: y+
    // z: 0: z-, 1: z+
    glm::ivec3 GetPlayerPartPos(const glm::ivec3 &part) const;
    bool HitBlock(const std::vector<glm::ivec3> &parts) const;
    void TryRotateY(float angle);
    void TryMoveX(float mvx);
    void TryMoveY(float mvy);
    void TryMoveZ(float mvz);

    void HandleKeys();
    void HandleMouseMove(int mouse_x, int mouse_y);
    void OnLeftButtonPress();
    void OnRightButtonPress();
    void HandleInput();

    void Quit();
};

