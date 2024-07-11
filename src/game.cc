#include "game.h"

#include <iostream>
#include <cmath>
#include <X11/Xlib.h>
#include <cassert>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "quickcg.h"

namespace {
    void GetScreenResolution(int &width, int &height) {
        Display* disp = XOpenDisplay(NULL);
        Screen*  scrn = DefaultScreenOfDisplay(disp);

        width = scrn->width;
        height = scrn->height;

        XCloseDisplay(disp);
    }
}

Game::Game(int screen_width, int screen_height, bool fullscreen) {
    if (screen_width < 0 || screen_height < 0) {
        GetScreenResolution(screen_width, screen_height);
    }
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    fullscreen_ = fullscreen;
}

void Game::Start() {
    Init();
    while (!QuickCG::done()) {
        Update();
        HandleInput();
    }
    Quit();
}

void Game::Init() {
    InitScreen();
    LoadTextures();
    InitRaycaster();
}

void Game::InitScreen() {
    QuickCG::screen(screen_width_, screen_height_, fullscreen_, "Chibicraft");
    SDL_ShowCursor(false);

    buffer_ = new uint32_t[screen_height_ * screen_width_];
}

void Game::LoadTextures() {
    textures_.resize(kNTextures);
    for (int i = 0; kNTextures > i; i++) textures_[i].resize(kTexWidth * kTexHeight);

    unsigned long tw, th, err = 0;
    err |= QuickCG::loadImage(textures_[0], tw, th, "res/textures/eagle.png");
    err |= QuickCG::loadImage(textures_[1], tw, th, "res/textures/redbrick.png");
err |= QuickCG::loadImage(textures_[3], tw, th, "res/textures/purplestone.png");
    err |= QuickCG::loadImage(textures_[2], tw, th, "res/textures/greystone.png");
    err |= QuickCG::loadImage(textures_[4], tw, th, "res/textures/bluestone.png");
err |= QuickCG::loadImage(textures_[6], tw, th, "res/textures/mossy.png");
    err |= QuickCG::loadImage(textures_[5], tw, th, "res/textures/wood.png");
    err |= QuickCG::loadImage(textures_[7], tw, th, "res/textures/colorstone.png");
    if (err) {
        std::cerr << "Error: Failed to load textures." << std::endl;
        Quit();
    }
}

void Game::InitRaycaster() {
    // y = 1だったら、床にへばりついている状態なので、床が単色になる(それはそう)
    pos_ = glm::vec3(21.0, 3.0, 11.5);
    dir_ = glm::vec3(0.0, 0.0, 1.0);
    plane_x_ = glm::vec3(0.66, 0.0, 0.0);
    plane_y_ = glm::vec3(0.0, 0.4125, 0.0);
}

void Game::Update() {
    Raycasting();

    QuickCG::drawBuffer(buffer_);

    old_time_ = time_;
    time_ = QuickCG::getTicks();
    frame_time_ = (time_ - old_time_) / 1000.0;
    QuickCG::print(1.0 / frame_time_);
    QuickCG::redraw();
}

void Game::Raycasting() {
    for (int y = 0; screen_height_ > y; y++) {
        float camera_y = 2.0 * y / screen_height_ - 1;
        for (int x = 0; screen_width_ > x; x++) {
            float camera_x = 2.0 * x / screen_width_ - 1;
            glm::vec3 raydir = dir_ + plane_x_ * camera_x + plane_y_ * camera_y;

            int map_x = pos_.x;
            int map_y = pos_.y;
            int map_z = pos_.z;

            float delta_dist_x = (raydir.x == 0) ? 1e30 : std::abs(1 / raydir.x);
            float delta_dist_y = (raydir.y == 0) ? 1e30 : std::abs(1 / raydir.y);
            float delta_dist_z = (raydir.z == 0) ? 1e30 : std::abs(1 / raydir.z);

            int step_x, step_y, step_z;
            float side_dist_x, side_dist_y, side_dist_z;
            if (raydir.x < 0) {
                step_x = -1;
                side_dist_x = (pos_.x - map_x) * delta_dist_x;
            }
            else {
                step_x = 1;
                side_dist_x = (map_x + 1.0 - pos_.x) * delta_dist_x;
            }
            if (raydir.y < 0) {
                step_y = -1;
                side_dist_y = (pos_.y - map_y) * delta_dist_y;
            }
            else {
                step_y = 1;
                side_dist_y = (map_y + 1.0 - pos_.y) * delta_dist_y;
            }
            if (raydir.z < 0) {
                step_z = -1;
                side_dist_z = (pos_.z - map_z) * delta_dist_z;
            }
            else {
                step_z = 1;
                side_dist_z = (map_z + 1.0 - pos_.z) * delta_dist_z;
            }

            int side;
            bool hit = false;
            while (!hit) {
                if (side_dist_x <= side_dist_y
                 && side_dist_x <= side_dist_z
                 && side_dist_x <= kMaxRayDist) {
                    side_dist_x += delta_dist_x;
                    map_x += step_x;
                    side = 0;
                }
                else if (side_dist_y <= side_dist_z
                      && side_dist_y <= kMaxRayDist) {
                    side_dist_y += delta_dist_y;
                    map_y += step_y;
                    side = 1;
                }
                else if (side_dist_z <= kMaxRayDist) {
                    side_dist_z += delta_dist_z;
                    map_z += step_z;
                    side = 2;
                }
                else {
                    break;
                }
                if (map_x < 0 || map_x >= kMapWidth ||
                    map_y < 0 || map_y >= kMapHeight ||
                    map_z < 0 || map_z >= kMapDepth) {
                    break;
                }
                int tex = GetMapBlock(map_x, map_y, map_z);
                hit = tex > 0 && tex != kTransparentBlock;
            }
            if (hit) {
                float perp_wall_dist;
                float max_perp_wall_dist;
                if (side == 0) {
                    perp_wall_dist = (side_dist_x - delta_dist_x);
                    max_perp_wall_dist = (kMaxRayDist - delta_dist_x);
                }
                else if (side == 1) {
                    perp_wall_dist = (side_dist_y - delta_dist_y);
                    max_perp_wall_dist = (kMaxRayDist - delta_dist_y);
                }
                else {
                    perp_wall_dist = (side_dist_z - delta_dist_z);
                    max_perp_wall_dist = (kMaxRayDist - delta_dist_z);
                }

                int tex = GetMapBlock(map_x, map_y, map_z) - 1;
                float wall_x, wall_y;
                if (side == 0) {
                    wall_x = pos_.z + perp_wall_dist * raydir.z;
                    wall_y = pos_.y + perp_wall_dist * raydir.y;
                }
                else if (side == 1) {
                    wall_x = pos_.x + perp_wall_dist * raydir.x;
                    wall_y = pos_.z + perp_wall_dist * raydir.z;
                }
                else {
                    wall_x = pos_.x + perp_wall_dist * raydir.x;
                    wall_y = pos_.y + perp_wall_dist * raydir.y;
                }
                wall_x -= floor(wall_x);
                wall_y -= floor(wall_y);

                int tex_x = wall_x * kTexWidth;
                int tex_y = wall_y * kTexHeight;
                if (side == 0 && raydir.x > 0) {
                    tex_x = kTexWidth - tex_x - 1;
                }
                if (side == 1 && raydir.y > 0) {
                    tex_x = kTexWidth - tex_x - 1;
                }
                if (side == 2 && raydir.z < 0) {
                    tex_x = kTexWidth - tex_x - 1;
                }

                uint32_t color = GetTexColor(tex, tex_x, kTexWidth - tex_y - 1);
                if (side == 1 || side == 2) {
                    color = (color >> 1) & 0x7F7F7F;
                }
                uint32_t fog = std::min(0xFFu,
                    (uint32_t)(0xFF * perp_wall_dist / max_perp_wall_dist));
                color =
                    std::min((color >> 16 & 0xFF) + fog, 0xFFu) << 16 |
                    std::min((color >> 8  & 0xFF) + fog, 0xFFu) << 8  |
                    std::min((color       & 0xFF) + fog, 0xFFu);
                SetBufColor(x, screen_height_ - y - 1, color);
            }
            else {
                SetBufColor(x, screen_height_ - y - 1, 0xFFFFFF);
            }
        }
    }
}

void Game::HandleInput() {
    float move_speed = frame_time_ * 2.0;
    float rot_speed = frame_time_ * 0.05;

    glm::vec3 perdir = glm::normalize(plane_x_);
    glm::vec3 mvdir(0, 0, 0);
    QuickCG::readKeys();
    if (QuickCG::keyDown(SDLK_w)) {
        mvdir.x += dir_.x;
        mvdir.z += dir_.z;
    }
    if (QuickCG::keyDown(SDLK_s)) {
        mvdir.x -= dir_.x;
        mvdir.z -= dir_.z;
    }
    if (QuickCG::keyDown(SDLK_d)) {
        mvdir.x += perdir.x;
        mvdir.z += perdir.z;
    }
    if (QuickCG::keyDown(SDLK_a)) {
        mvdir.x -= perdir.x;
        mvdir.z -= perdir.z;
    }
    if (QuickCG::keyDown(SDLK_SPACE)) {
        mvdir.y += 1;
    }
    if (QuickCG::keyDown(SDLK_LSHIFT)) {
        mvdir.y -= 1;
    }
    if (glm::length(mvdir) > 0) {
        mvdir = move_speed * glm::normalize(mvdir);
        if (CanMoveX(mvdir.x)) {
            pos_.x += mvdir.x;
        }
        if (CanMoveY(mvdir.y)) {
            pos_.y += mvdir.y;
        }
        if (CanMoveZ(mvdir.z)) {
            pos_.z += mvdir.z;
        }
    }

    int mouse_x, mouse_y;
    QuickCG::getMouseState(mouse_x, mouse_y);
    SDL_WarpMouse(screen_width_ / 2, screen_height_ / 2);
    if (mouse_x != screen_width_ / 2) {
        int delta_x = mouse_x - screen_width_ / 2;
        dir_ = glm::rotateY(dir_, rot_speed * delta_x);
        plane_x_ = glm::rotateY(plane_x_, rot_speed * delta_x);
        plane_y_ = glm::rotateY(plane_y_, rot_speed * delta_x);
    }
    if (mouse_y != screen_height_ / 2) {
        int delta_y = mouse_y - screen_height_ / 2;
        float angle = rot_speed * delta_y;
        // 視点が逆立ち -> plane_y.yが-
        if (CanRotateY(angle)) {
            dir_ = glm::rotate(dir_, angle, plane_x_);
            plane_y_ = glm::rotate(plane_y_, angle, plane_x_);
        }
    }
}

bool Game::CanRotateY(float angle) {
    glm::vec3 new_plane_y = glm::rotate(plane_y_, angle, plane_x_);
    return new_plane_y.y > 0;
}

bool Game::CanMoveX(float mvx) {
    float width = mvx > 0 ? kPlayerHalfWidth : -kPlayerHalfWidth;
    return
        GetMapBlock(pos_.x + width + mvx, pos_.y + kPlayerUpperHalfHeight,
            pos_.z + kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x + width + mvx, pos_.y + kPlayerUpperHalfHeight,
            pos_.z - kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x + width + mvx, pos_.y - kPlayerLowerHalfHeight,
            pos_.z + kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x + width + mvx, pos_.y - kPlayerLowerHalfHeight,
            pos_.z - kPlayerHalfDepth) == 0;
}

bool Game::CanMoveY(float mvy) {
    float height = mvy > 0 ? kPlayerUpperHalfHeight : -kPlayerLowerHalfHeight;
    return
        GetMapBlock(pos_.x + kPlayerHalfWidth, pos_.y + height + mvy,
            pos_.z + kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x + kPlayerHalfWidth, pos_.y + height + mvy,
            pos_.z - kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x - kPlayerHalfWidth, pos_.y + height + mvy,
            pos_.z + kPlayerHalfDepth) == 0 &&
        GetMapBlock(pos_.x - kPlayerHalfWidth, pos_.y + height + mvy,
            pos_.z - kPlayerHalfDepth) == 0;
}

bool Game::CanMoveZ(float mvz) {
    float depth = mvz > 0 ? kPlayerHalfDepth : -kPlayerHalfDepth;
    return
        GetMapBlock(pos_.x + kPlayerHalfWidth, pos_.y + kPlayerUpperHalfHeight,
            pos_.z + depth + mvz) == 0 &&
        GetMapBlock(pos_.x + kPlayerHalfWidth, pos_.y - kPlayerLowerHalfHeight,
            pos_.z + depth + mvz) == 0 &&
        GetMapBlock(pos_.x - kPlayerHalfWidth, pos_.y + kPlayerUpperHalfHeight,
            pos_.z + depth + mvz) == 0 &&
        GetMapBlock(pos_.x - kPlayerHalfWidth, pos_.y - kPlayerLowerHalfHeight,
            pos_.z + depth + mvz) == 0;
}

void Game::Quit() {
    delete buffer_;
    QuickCG::end();
}
