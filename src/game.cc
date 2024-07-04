#include "game.h"

#include <iostream>
#include <cmath>
#include <X11/Xlib.h>

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

void Game::Start() {
    Init();
    while (!QuickCG::done()) {
        Update();
        HandleInput();
    }
}

void Game::Init() {
    InitScreen();
    LoadTextures();

    pos_ = glm::vec2(22.0, 11.5);
    dir_ = glm::vec2(-1.0, 0.0);
    plane_ = glm::vec2(0.0, 0.66);
}

void Game::InitScreen() {
    GetScreenResolution(screen_width_, screen_height_);
    QuickCG::screen(screen_width_, screen_height_, true, "Chibicraft");
    SDL_ShowCursor(false);

    buffer_ = new uint32_t[screen_height_ * screen_width_];
}

void Game::LoadTextures() {
    textures_.resize(kNTextures);
    for (int i = 0; kNTextures > i; i++) textures_[i].resize(kTexWidth * kTexHeight);

    unsigned long tw, th, err = 0;
    err |= QuickCG::loadImage(textures_[0], tw, th, "res/textures/eagle.png");
    err |= QuickCG::loadImage(textures_[1], tw, th, "res/textures/redbrick.png");
    err |= QuickCG::loadImage(textures_[2], tw, th, "res/textures/purplestone.png");
    err |= QuickCG::loadImage(textures_[3], tw, th, "res/textures/greystone.png");
    err |= QuickCG::loadImage(textures_[4], tw, th, "res/textures/bluestone.png");
    err |= QuickCG::loadImage(textures_[5], tw, th, "res/textures/mossy.png");
    err |= QuickCG::loadImage(textures_[6], tw, th, "res/textures/wood.png");
    err |= QuickCG::loadImage(textures_[7], tw, th, "res/textures/colorstone.png");
    if (err) {
        std::cerr << "Error: Failed to load textures." << std::endl;
        Quit();
    }
}

void Game::Update() {
    FloorCasting();
    WallCasting();

    QuickCG::drawBuffer(buffer_);

    old_time_ = time_;
    time_ = QuickCG::getTicks();
    frame_time_ = (time_ - old_time_) / 1000.0;
    QuickCG::print(1.0 / frame_time_);
    QuickCG::redraw();
}

void Game::FloorCasting() {
    for (int y = screen_height_ / 2 + 1; screen_height_ > y; y++) {
        glm::vec2 raydir0 = dir_ - plane_;
        glm::vec2 raydir1 = dir_ + plane_;

        int p = y - screen_height_ / 2;
        float posZ = 0.5 * screen_height_;
        float row_dist = posZ / p;

        glm::vec2 floor_step = row_dist / screen_width_ * (raydir1 - raydir0);
        glm::vec2 floor = pos_ + row_dist * raydir0;

        for (int x = 0; screen_width_ > x; x++) {
            int cellx = floor.x;
            int celly = floor.y;

            int tx = static_cast<int>(kTexWidth * (floor.x - cellx))
                & (kTexWidth - 1);
            int ty = static_cast<int>(kTexHeight * (floor.y - celly))
                & (kTexHeight - 1);

            floor += floor_step;

            int floor_tex = (cellx + celly) % 2 == 0 ? 3 : 4;
            int ceiling_tex = 6;

            uint32_t color;
            color = textures_[floor_tex][kTexWidth * ty + tx];
            color = (color >> 1) & 0x7F7F7F;
            buffer_[screen_width_ * y + x] = color;

            color = textures_[ceiling_tex][kTexWidth * ty + tx];
            color = (color >> 1) & 0x7F7F7F;
            buffer_[screen_width_ * (screen_height_ - y - 1) + x] = color;
        }
    }
}

void Game::WallCasting() {
    for (int x = 0; screen_width_ > x; x++) {
        float camera_x = 2.0 * x / screen_width_ - 1;
        glm::vec2 raydir = dir_ + plane_ * camera_x;

        int mapx = pos_.x;
        int mapy = pos_.y;

        float delta_dist_x = (raydir.x == 0) ? 1e30 : std::abs(1 / raydir.x);
        float delta_dist_y = (raydir.y == 0) ? 1e30 : std::abs(1 / raydir.y);

        int stepx, stepy;
        float side_dist_x, side_dist_y;
        if (raydir.x < 0) {
            stepx = -1;
            side_dist_x = (pos_.x - mapx) * delta_dist_x;
        }
        else {
            stepx = 1;
            side_dist_x = (mapx + 1.0 - pos_.x) * delta_dist_x;
        }
        if (raydir.y < 0) {
            stepy = -1;
            side_dist_y = (pos_.y - mapy) * delta_dist_y;
        }
        else {
            stepy = 1;
            side_dist_y = (mapy + 1.0 - pos_.y) * delta_dist_y;
        }

        int side;
        bool hit = false;
        while (!hit) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                mapx += stepx;
                side = 0;
            }
            else {
                side_dist_y += delta_dist_y;
                mapy += stepy;
                side = 1;
            }
            hit = world_map_[mapx][mapy] > 0;
        }

        float perp_wall_dist;
        if (side == 0) {
            perp_wall_dist = (side_dist_x - delta_dist_x);
        }
        else {
            perp_wall_dist = (side_dist_y - delta_dist_y);
        }

        int line_height = screen_height_ / perp_wall_dist;
        int draw_start = std::max(-line_height / 2 + screen_height_ / 2, 0);
        int draw_end = std::min(line_height / 2 + screen_height_ / 2,
            screen_height_ - 1);
        int tex = world_map_[mapx][mapy] - 1;

        float wallx;
        if (side == 0) {
            wallx = pos_.y + perp_wall_dist * raydir.y;
        }
        else {
            wallx = pos_.x + perp_wall_dist * raydir.x;
        }
        wallx -= floor(wallx);

        int texx = wallx * kTexWidth;
        if (side == 0 && raydir.x > 0) {
            texx = kTexWidth - texx - 1;
        }
        if (side == 1 && raydir.y < 0) {
            texx = kTexWidth - texx - 1;
        }

        float step = 1.0 * kTexHeight / line_height;
        float texpos = (draw_start - screen_height_ / 2 + line_height / 2) * step;
        for (int y = draw_start; y < draw_end; y++) {
            int texy = static_cast<int>(texpos) & (kTexHeight - 1);
            texpos += step;
            uint32_t color = textures_[tex][kTexHeight * texy + texx];
            if (side == 1) {
                color = (color >> 1) & 0x7F7F7F;
            }
            buffer_[screen_width_ * y + x] = color;
        }
    }
}

void Game::HandleInput() {
    float move_speed = frame_time_ * 3.0;
    float rot_speed = frame_time_ * 0.05;

    glm::vec2 perdir = glm::rotate(dir_, glm::radians(-90.f));
    glm::vec2 mvdir(0, 0);
    QuickCG::readKeys();
    if (QuickCG::keyDown(SDLK_w)) {
        if (world_map_[static_cast<int>(pos_.x + dir_.x * move_speed)]
            [static_cast<int>(pos_.y)] == 0) {
            mvdir.x += dir_.x;
        }
        if (world_map_[static_cast<int>(pos_.x)]
            [static_cast<int>(pos_.y + dir_.y * move_speed)] == 0) {
            mvdir.y += dir_.y;
        }
    }
    if (QuickCG::keyDown(SDLK_s)) {
        if (world_map_[static_cast<int>(pos_.x - dir_.x * move_speed)]
            [static_cast<int>(pos_.y)] == 0) {
            mvdir.x -= dir_.x;
        }
        if (world_map_[static_cast<int>(pos_.x)]
            [static_cast<int>(pos_.y - dir_.y * move_speed)] == 0) {
            mvdir.y -= dir_.y;
        }
    }
    if (QuickCG::keyDown(SDLK_d)) {
        if (world_map_[static_cast<int>(pos_.x + perdir.x * move_speed)]
            [static_cast<int>(pos_.y)] == 0) {
            mvdir.x += perdir.x;
        }
        if (world_map_[static_cast<int>(pos_.x)]
            [static_cast<int>(pos_.y + perdir.y * move_speed)] == 0) {
            mvdir.y += perdir.y;
        }
    }
    if (QuickCG::keyDown(SDLK_a)) {
        if (world_map_[static_cast<int>(pos_.x - perdir.x * move_speed)]
            [static_cast<int>(pos_.y)] == 0) {
            mvdir.x -= perdir.x;
        }
        if (world_map_[static_cast<int>(pos_.x)]
            [static_cast<int>(pos_.y - perdir.y * move_speed)] == 0) {
            mvdir.y -= perdir.y;
        }
    }
    if (glm::length(mvdir) > 0) {
        pos_ += move_speed * glm::normalize(mvdir);
    }

    int mouse_x, mouse_y;
    QuickCG::getMouseState(mouse_x, mouse_y);
    SDL_WarpMouse(screen_width_ / 2, screen_height_ / 2);
    if (mouse_x != screen_width_ / 2) {
        int deltax = screen_width_ / 2 - mouse_x;
        dir_ = glm::rotate(dir_, rot_speed * deltax);
        plane_ = glm::rotate(plane_, rot_speed * deltax);
    }
}

void Game::Quit() {
    delete buffer_;
    QuickCG::end();
}
