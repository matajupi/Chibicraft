#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <glm/glm.hpp>

#include "screen.h"
#include "world.h"

namespace chibicraft {

class Game {
public:
    Game() : Game(-1, -1, false) { }
    Game(int screen_width, int screen_height, bool fullscreen = true);
    void Start();

private:
    std::shared_ptr<World> world_;
    std::shared_ptr<Screen> screen_;
    std::shared_ptr<Player> player_;

    // ======== Ray ========
    static constexpr const int kMaxRayDist = 20;

    struct Ray {
        glm::vec3 dir;
        glm::ivec3 pos;
        // collision_side: 衝突した面に垂直な軸
        // 0: x軸(面はy-z平面), 1: y軸, 2: z軸
        int collision_side;
        float perp_wall_dist;
        float max_perp_wall_dist;
    };

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
    void Raycasting();

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

}
