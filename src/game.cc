#include "game.h"

#include <cstdio>
#include <cmath>
#include <cassert>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "quickcg.h"

using namespace chibicraft;

Game::Game(int screen_width, int screen_height, bool fullscreen)
    : prev_lmb_(false) {
    if (screen_width < 0 || screen_height < 0) {
        Screen::GetDefaultResolution(screen_width, screen_height);
    }
    world_ = std::make_shared<World>();
    screen_ = std::make_shared<Screen>(screen_width, screen_height, fullscreen, "Chibigit");
    player_ = std::make_shared<Player>();
}

void Game::Start() {
    Init();
    while (!QuickCG::done()) {
        Update();
        player_->HandleInput();
    }
    Quit();
}

void Game::Init() {
world_->Init();
}

void Game::Update() {
    Raycasting();
    screen_->DrawCursor();

    old_time_ = time_;
    time_ = QuickCG::getTicks();
    frame_time_ = (time_ - old_time_) / 1000.0;

    screen_->Print(1.0 / frame_time_);
    screen_->Print(player_->GetSelectBlock());

    screen_->Update();
}

bool Game::CastRay(int x, int y, Ray &ray) const {
    float camera_y = 2.0 * y / screen_->GetHeight() - 1;
    float camera_x = 2.0 * x / screen_->GetWidth() - 1;

    glm::vec3 dir = player_->GetDir();
    glm::vec3 plane_x = player->GetPlaneX();
    glm::vec3 plane_y = player->GetPlaneY();
    glm::vec3 pos = player->GetPos();

    ray.dir = dir + plane_x * camera_x + plane_y * camera_y;
    ray.pos = pos;

    float delta_dist_x = (ray.dir.x == 0) ? 1e30 : std::abs(1 / ray.dir.x);
    float delta_dist_y = (ray.dir.y == 0) ? 1e30 : std::abs(1 / ray.dir.y);
    float delta_dist_z = (ray.dir.z == 0) ? 1e30 : std::abs(1 / ray.dir.z);

    int step_x, step_y, step_z;
    float side_dist_x, side_dist_y, side_dist_z;
    if (ray.dir.x < 0) {
        step_x = -1;
        side_dist_x = (pos.x - ray.pos.x) * delta_dist_x;
    }
    else {
        step_x = 1;
        side_dist_x = (ray.pos.x + 1.0 - pos.x) * delta_dist_x;
    }
    if (ray.dir.y < 0) {
        step_y = -1;
        side_dist_y = (pos.y - ray.pos.y) * delta_dist_y;
    }
    else {
        step_y = 1;
        side_dist_y = (ray.pos.y + 1.0 - pos.y) * delta_dist_y;
    }
    if (ray.dir.z < 0) {
        step_z = -1;
        side_dist_z = (pos.z - ray.pos.z) * delta_dist_z;
    }
    else {
        step_z = 1;
        side_dist_z = (ray.pos.z + 1.0 - pos.z) * delta_dist_z;
    }

    bool hit = false;
    while (!hit) {
        if (side_dist_x <= side_dist_y
         && side_dist_x <= side_dist_z
         && side_dist_x <= kMaxRayDist) {
            side_dist_x += delta_dist_x;
            ray.pos.x += step_x;
            ray.collision_side = 0;
        }
        else if (side_dist_y <= side_dist_z
              && side_dist_y <= kMaxRayDist) {
            side_dist_y += delta_dist_y;
            ray.pos.y += step_y;
            ray.collision_side = 1;
        }
        else if (side_dist_z <= kMaxRayDist) {
            side_dist_z += delta_dist_z;
            ray.pos.z += step_z;
            ray.collision_side = 2;
        }
        else {
            break;
        }
        if (ray.pos.x < 0 || ray.pos.y < 0 || ray.pos.z < 0) {
            break;
        }
        const auto *block = world_->GetBlock(ray.pos);
        hit = !block->IsAir();
    }

    if (ray.collision_side == 0) {
        ray.perp_wall_dist = (side_dist_x - delta_dist_x);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_x);
    }
    else if (ray.collision_side == 1) {
        ray.perp_wall_dist = (side_dist_y - delta_dist_y);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_y);
    }
    else {
        ray.perp_wall_dist = (side_dist_z - delta_dist_z);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_z);
    }

    return hit;
}

uint32_t Game::CalcPixelColor(const Ray &ray) const {
    const auto *block = world_->GetBlock(ray.pos);
    float wall_x, wall_y;
    glm::vec3 pos = player_->GetPos();
    if (ray.collision_side == 0) {
        wall_x = pos.z + ray.perp_wall_dist * ray.dir.z;
        wall_y = pos.y + ray.perp_wall_dist * ray.dir.y;
    }
    else if (ray.collision_side == 1) {
        wall_x = pos.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos.z + ray.perp_wall_dist * ray.dir.z;
    }
    else {
        wall_x = pos.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos.y + ray.perp_wall_dist * ray.dir.y;
    }
    wall_x -= floor(wall_x);
    wall_y -= floor(wall_y);

    int tex_x = wall_x * Texture::kWidth;
    int tex_y = wall_y * Texture::kHeight;
    BlockSurface surface;
    if (ray.collision_side == 0) {
        if (ray.dir.x > 0) {
            tex_x = Texture::kWidth - tex_x - 1;
            surface = BlockSurface::YZ_Higher;
        }
        else {
            surface = BlockSurface::YZ_Lower;
        }
    }
    if (ray.collision_side == 1) {
        if (ray.dir.y > 0) {
            tex_x = Texture::kWidth - tex_x - 1;
            surface = BlockSurface::XZ_Higher;
        }
        else {
            surface = BlockSurface::XZ_Lower;
        }
    }
    if (ray.collision_side == 2) {
        if (ray.dir.z < 0) {
            tex_x = Texture::kWidth - tex_x - 1;
            surface = BlockSurface::XY_Higher;
        }
        else {
            surface = BlockSurface::XY_Lower;
        }
    }

    const auto *tex = block->GetTexture(surface);
    uint32_t color = tex->GetPixel(tex_x, Texture::kHeight - tex_y - 1);
    if (ray.collision_side == 1 || ray.collision_side == 2) {
        color = (color >> 1) & 0x7F7F7F;
    }
    uint32_t fog = std::min(0xFFu,
        (uint32_t)(0xFF * ray.perp_wall_dist / ray.max_perp_wall_dist));
    color =
        std::min((color >> 16 & 0xFF) + fog, 0xFFu) << 16 |
        std::min((color >> 8  & 0xFF) + fog, 0xFFu) << 8  |
        std::min((color       & 0xFF) + fog, 0xFFu);
    return color;
}

void Game::Raycasting() {
    for (int y = 0; screen_->GetHeight() > y; y++) {
        for (int x = 0; screen_->GetWidth() > x; x++) {
            Ray ray;
            bool hit = CastRay(x, y, ray);
            if (hit) {
                uint32_t color = CalcPixelColor(ray);
                screen_->SetColor(x, screen_->GetHeight() - y - 1, color);
            }
            else {
                screen_->SetColor(x, screen_->GetHeight() - y - 1, 0xFFFFFF);
            }
        }
    }
}

void Game::Quit() {
    world_->Save();
    QuickCG::end();
}

