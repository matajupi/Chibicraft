#include "game.h"

#include <cstdio>
#include <cmath>
#include <cassert>
#include <omp.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <X11/Xlib.h>

#include "quickcg.h"

namespace {
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

    void GetScreenResolution(int &width, int &height) {
        Display* disp = XOpenDisplay(NULL);
        Screen*  scrn = DefaultScreenOfDisplay(disp);

        width = scrn->width;
        height = scrn->height;

        XCloseDisplay(disp);
    }
}

const std::string Game::kTexDir = "bedrock-samples/resource_pack/textures/blocks/";
const std::array<std::string, Game::kNTexs> Game::kTexFiles = {
    "grass_carried.png",            // 0x00
    "grass_side_carried.png",       // 0x01
    "dirt.png",                     // 0x02
    "planks_big_oak.png",           // 0x03
    "quartz_block_chiseled_top.png",// 0x04
    "quartz_block_chiseled.png",    // 0x05
    "brick.png",                    // 0x06
    "cherry_log_top.png",           // 0x07
    "cherry_log_side.png",          // 0x08
    "cherry_planks.png",            // 0x09
    "cherry_leaves_opaque.png",     // 0x0a
    "coal_block.png",               // 0x0b
    "cracked_nether_bricks.png",    // 0x0c
    "crafting_table_front.png",     // 0x0d
    "crafting_table_side.png",      // 0x0e
    "crafting_table_top.png",       // 0x0f
    "planks_oak.png",               // 0x10
    "stone.png",                    // 0x11
    "tnt_top.png",                  // 0x12
    "tnt_bottom.png",               // 0x13
    "tnt_side.png",                 // 0x14
};
// 6面を6byteで指定
const std::array<long long int, Game::kNBlocks> Game::kBlockToTexs = {
    0xffffffffffff,                 // Air
    0x010100020101,                 // Grass
    0x030303030303,                 // Big oak plank
    0x050504040505,                 // Quartz block chiseled
    0x060606060606,                 // Brick
    0x080807070808,                 // Cherry log
    0x090909090909,                 // Cherry plank
    0x0a0a0a0a0a0a,                 // Cherry leaves
    0x0b0b0b0b0b0b,                 // Coal block
    0x0c0c0c0c0c0c,                 // Cracked nether bricks
    0x0d0d0f100e0e,                 // Crafting table
    0x111111111111,                 // Stone
    0x141412131414,                 // TNT
    0x010100020101,                 // Reserved
    0x010100020101,                 // Reserved
    0x010100020101,                 // Reserved
    0x010100020101,                 // Reserved
    0x010100020101,                 // Reserved
    0x010100020101,                 // Reserved
    0xffffffffffff,                 // Transparent
};
const std::array<std::string, Game::kNBlocks> Game::kBlockName = {
    "Air",
    "Grass",
    "Big oak plank",
    "Quartz block chiseled",
    "Brick",
    "Cherry log",
    "Cherry plank",
    "Cherry leaves",
    "Coal block",
    "Cracked nether brick",
    "Crafting table",
    "Stone",
    "TNT",
    "Reserved1",
    "Reserved2",
    "Reserved3",
    "Reserved4",
    "Reserved5",
    "Reserved6",
    "Transparent",
};

Game::Game(int screen_width, int screen_height, bool fullscreen)
    : fullscreen_(fullscreen), prev_lmb_(false) {
    if (screen_width < 0 || screen_height < 0) {
        GetScreenResolution(screen_width, screen_height);
    }
    screen_width_ = screen_width;
    screen_height_ = screen_height;
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
    LoadMap(0);
    LoadTexs();
    InitPlayer();
}

void Game::InitScreen() {
    QuickCG::screen(screen_width_, screen_height_, fullscreen_, "Chibicraft");
    SDL_ShowCursor(false);

    buffer_ = new uint32_t[screen_height_ * screen_width_];
}

void Game::LoadMap(int mid) {
    std::string mfn = ToMapFileName(mid);

    std::ifstream ifs(mfn, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Failed to open map." << std::endl;
        Quit();
    }

    ifs.seekg(0, std::ios::end);
    int file_size = ifs.tellg();
    int map_size = kMapHeight * kMapDepth * kMapWidth;
    if (file_size < map_size) {
        std::cerr << "Error: Failed to load map." << std::endl;
        Quit();
    }

    ifs.seekg(0);
    ifs.read(world_map_, map_size);
}

void Game::SaveMap(int mid) {
    std::string mfn = ToMapFileName(mid);

    std::ofstream ofs(mfn, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Failed to open map." << std::endl;
        Quit();
    }

    int map_size = kMapHeight * kMapDepth * kMapWidth;
    ofs.write(world_map_, map_size);
}

void Game::LoadTexs() {
    unsigned long tw, th, err = 0;
    for (int i = 0; kNTexs > i; i++) {
        texs_[i].resize(kTexWidth * kTexHeight);
        err |= QuickCG::loadImage(texs_[i], tw, th, kTexDir + kTexFiles[i]);
        if (err) {
            std::cerr << "Error: Failed to load textures: "
                << kTexFiles[i] << std::endl;
            Quit();
        }
    }
}

void Game::InitPlayer() {
    // y = 1だったら、床にへばりついている状態なので、床が単色になる(それはそう)
    pos_ = glm::vec3(kMapWidth / 2, kMapHeight / 2 + 3.5, kMapDepth / 2);
    dir_ = glm::vec3(0.0, 0.0, 1.0);
    plane_x_ = glm::vec3(0.66, 0.0, 0.0);
    plane_y_ = glm::vec3(0.0, 0.4125, 0.0);
}

void Game::Update() {
    SlackOffRaycasting();
    // SimpleRaycasting();
    DrawCursor();

    QuickCG::drawBuffer(buffer_);

    old_time_ = time_;
    time_ = QuickCG::getTicks();
    frame_time_ = (time_ - old_time_) / 1000.0;
    QuickCG::print(1.0 / frame_time_, 20, 20, QuickCG::RGB_Black);

    QuickCG::print(kBlockName[select_block_], 20, 40, QuickCG::RGB_Black);

    QuickCG::redraw();
}

void Game::DrawCursor() {
    for (int y = 0; kCursorHeight > y; y++) {
        for (int x = 0; kCursorWidth > x; x++) {
            if (kCursorShape[y][x] == '@') {
                SetBufColor(
                    x + screen_width_ / 2 - kCursorWidth / 2,
                    y + screen_height_ / 2 - kCursorHeight / 2,
                    0x19959c
                );
            }
        }
    }
}

bool Game::CastRay(int x, int y, Ray &ray) const {
    float camera_y = 2.0 * y / screen_height_ - 1;
    float camera_x = 2.0 * x / screen_width_ - 1;

    ray.dir = dir_ + plane_x_ * camera_x + plane_y_ * camera_y;
    ray.pos = pos_;

    float delta_dist_x = (ray.dir.x == 0) ? 1e30 : std::abs(1 / ray.dir.x);
    float delta_dist_y = (ray.dir.y == 0) ? 1e30 : std::abs(1 / ray.dir.y);
    float delta_dist_z = (ray.dir.z == 0) ? 1e30 : std::abs(1 / ray.dir.z);

    int step_x, step_y, step_z;
    float side_dist_x, side_dist_y, side_dist_z;
    if (ray.dir.x < 0) {
        step_x = -1;
        side_dist_x = (pos_.x - ray.pos.x) * delta_dist_x;
    }
    else {
        step_x = 1;
        side_dist_x = (ray.pos.x + 1.0 - pos_.x) * delta_dist_x;
    }
    if (ray.dir.y < 0) {
        step_y = -1;
        side_dist_y = (pos_.y - ray.pos.y) * delta_dist_y;
    }
    else {
        step_y = 1;
        side_dist_y = (ray.pos.y + 1.0 - pos_.y) * delta_dist_y;
    }
    if (ray.dir.z < 0) {
        step_z = -1;
        side_dist_z = (pos_.z - ray.pos.z) * delta_dist_z;
    }
    else {
        step_z = 1;
        side_dist_z = (ray.pos.z + 1.0 - pos_.z) * delta_dist_z;
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
        if (ray.pos.x < 0 || ray.pos.x >= kMapWidth ||
            ray.pos.y < 0 || ray.pos.y >= kMapHeight ||
            ray.pos.z < 0 || ray.pos.z >= kMapDepth) {
            break;
        }
        int block = GetMapBlock(ray.pos);
        hit = block != kAirBlock && block != kTransparentBlock;
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
    char block = GetMapBlock(ray.pos);
    float wall_x, wall_y;
    if (ray.collision_side == 0) {
        wall_x = pos_.z + ray.perp_wall_dist * ray.dir.z;
        wall_y = pos_.y + ray.perp_wall_dist * ray.dir.y;
    }
    else if (ray.collision_side == 1) {
        wall_x = pos_.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos_.z + ray.perp_wall_dist * ray.dir.z;
    }
    else {
        wall_x = pos_.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos_.y + ray.perp_wall_dist * ray.dir.y;
    }
    wall_x -= floor(wall_x);
    wall_y -= floor(wall_y);

    int tex_x = wall_x * kTexWidth;
    int tex_y = wall_y * kTexHeight;
    int face = 0;
    if (ray.collision_side == 0) {
        if (ray.dir.x > 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 0;
        }
        else {
            face = 1;
        }
    }
    if (ray.collision_side == 1) {
        if (ray.dir.y > 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 2;
        }
        else {
            face = 3;
        }
    }
    if (ray.collision_side == 2) {
        if (ray.dir.z < 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 4;
        }
        else {
            face = 5;
        }
    }

    int tex = GetTex(block, face);
    uint32_t color = GetTexColor(tex, tex_x, kTexHeight - tex_y - 1);
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

void Game::SimpleRaycasting() {
#pragma omp parallel for num_threads(4)
    for (int y = 0; screen_height_ > y; y++) {
        for (int x = 0; screen_width_ > x; x++) {
            Ray ray;
            bool hit = CastRay(x, y, ray);
            if (hit) {
                uint32_t color = CalcPixelColor(ray);
                SetBufColor(x, screen_height_ - y - 1, color);
            }
            else {
                SetBufColor(x, screen_height_ - y - 1, 0xFFFFFF);
            }
        }
    }
}

void Game::SlackOffRaycasting() {
#pragma omp parallel for num_threads(4)
    for (int y = 1; screen_height_ > y; y += 3) {
        for (int x = 1; screen_width_ > x; x += 3) {
            Ray ray;
            bool hit = CastRay(x, y, ray);
            if (hit) {
                uint32_t color = CalcPixelColor(ray);
                for (int dy = -1; 1 >= dy; dy++) {
                    for (int dx = -1; 1 >= dx; dx++) {
                        SetBufColor(x + dx, screen_height_ - y - dy - 1, color);
                    }
                }
            }
            else {
                for (int dy = -1; 1 >= dy; dy++) {
                    for (int dx = -1; 1 >= dx; dx++) {
                        SetBufColor(x + dx, screen_height_ - y - dy - 1, 0xFFFFFF);
                    }
                }
            }
        }
    }
}

void Game::HandleKeys() {
    float move_speed = frame_time_ * 5.0;

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
        TryMoveX(mvdir.x);
        TryMoveY(mvdir.y);
        TryMoveZ(mvdir.z);
    }

    int n_visible_blocks = kNBlocks - 2;
    if (QuickCG::keyPressed(SDLK_RIGHT)) {
        select_block_ = select_block_ % n_visible_blocks + 1;
    }
    if (QuickCG::keyPressed(SDLK_LEFT)) {
        select_block_ = (select_block_ - 2 + n_visible_blocks) % n_visible_blocks + 1;
    }
}

void Game::HandleMouseMove(int mouse_x, int mouse_y) {
    float rot_speed = frame_time_ * 0.05;

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
        TryRotateY(angle);
    }
}

void Game::OnLeftButtonPress() {
    Ray ray;
    bool hit = CastRay(screen_width_ / 2, screen_height_ / 2, ray);
    if (!hit || ray.perp_wall_dist > kPlayerDestBlockDist) {
        return;
    }
    SetMapBlock(ray.pos, kAirBlock);
}

void Game::OnRightButtonPress() {
    Ray ray;
    bool hit = CastRay(screen_width_ / 2, screen_height_ / 2, ray);
    if (!hit || ray.perp_wall_dist > kPlayerPutBlockDist) {
        return;
    }
    glm::ivec3 block_pos = ray.pos;
    if (ray.collision_side == 0) {
        block_pos.x += ray.dir.x < 0 ? 1 : -1;
    }
    if (ray.collision_side == 1) {
        block_pos.y += ray.dir.y < 0 ? 1 : -1;
    }
    if (ray.collision_side == 2) {
        block_pos.z += ray.dir.z < 0 ? 1 : -1;
    }

    assert(GetMapBlock(block_pos) == 0);

    SetMapBlock(block_pos, select_block_);
    hit = HitBlock({
        glm::ivec3(0, 0, 0), glm::ivec3(0, 0, 1),
        glm::ivec3(0, 1, 0), glm::ivec3(0, 1, 1),
        glm::ivec3(0, 2, 0), glm::ivec3(0, 2, 1),
        glm::ivec3(1, 0, 0), glm::ivec3(1, 0, 1),
        glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1),
        glm::ivec3(1, 2, 0), glm::ivec3(1, 2, 1),
    });
    if (hit) {
        SetMapBlock(block_pos, kAirBlock);
    }
}

void Game::HandleInput() {
    HandleKeys();

    int mouse_x, mouse_y;
    bool lmb, rmb;
    QuickCG::getMouseState(mouse_x, mouse_y, lmb, rmb);

    HandleMouseMove(mouse_x, mouse_y);
    if (!prev_lmb_ && lmb) {
        OnLeftButtonPress();
    }
    if (!prev_rmb_ && rmb) {
        OnRightButtonPress();
    }
    prev_lmb_ = lmb;
    prev_rmb_ = rmb;
}

glm::ivec3 Game::GetPlayerPartPos(const glm::ivec3 &part) const {
    assert(0 <= part.x && part.x <= 1 && 0 <= part.y && part.y <= 2 &&
        0 <= part.z && part.z <= 1);
    glm::ivec3 part_pos;

    part_pos.x = pos_.x + (part.x == 0 ? -kPlayerHalfWidth : kPlayerHalfWidth);
    if (part.y == 0) {
        part_pos.y = pos_.y - kPlayerLowerHalfHeight;
    }
    else if (part.y == 1) {
        part_pos.y = pos_.y - kPlayerLowerHalfHeight + 1.0;
    }
    else if (part.y == 2) {
        part_pos.y = pos_.y + kPlayerUpperHalfHeight;
    }
    part_pos.z = pos_.z + (part.z == 0 ? -kPlayerHalfDepth : kPlayerHalfDepth);

    return part_pos;
}

bool Game::HitBlock(const std::vector<glm::ivec3> &parts) const {
    bool hit = false;
    for (const glm::ivec3 &part : parts) {
        hit |= GetMapBlock(GetPlayerPartPos(part)) != kAirBlock;
    }
    return hit;
}

void Game::TryRotateY(float angle) {
    // 視点が逆立ち -> plane_y.yが-
    glm::vec3 new_plane_y = glm::rotate(plane_y_, angle, plane_x_);
    if (new_plane_y.y > 0) {
        dir_ = glm::rotate(dir_, angle, plane_x_);
        plane_y_ = new_plane_y;
    }
}

void Game::TryMoveX(float mvx) {
    pos_.x += mvx;
    int part_x = mvx < 0 ? 0 : 1;
    bool hit = HitBlock({
        glm::ivec3(part_x, 0, 0), glm::ivec3(part_x, 0, 1),
        glm::ivec3(part_x, 1, 0), glm::ivec3(part_x, 1, 1),
        glm::ivec3(part_x, 2, 0), glm::ivec3(part_x, 2, 1),
    });
    if (hit) {
        pos_.x -= mvx;
    }
}

void Game::TryMoveY(float mvy) {
    pos_.y += mvy;
    int part_y = mvy < 0 ? 0 : 2;
    bool hit = HitBlock({
        glm::ivec3(0, part_y, 0), glm::ivec3(0, part_y, 1),
        glm::ivec3(1, part_y, 0), glm::ivec3(1, part_y, 1),
    });
    if (hit) {
        pos_.y -= mvy;
    }
}

void Game::TryMoveZ(float mvz) {
    pos_.z += mvz;
    int part_z = mvz < 0 ? 0 : 1;
    bool hit = HitBlock({
        glm::ivec3(0, 0, part_z), glm::ivec3(0, 1, part_z), glm::ivec3(0, 2, part_z),
        glm::ivec3(1, 0, part_z), glm::ivec3(1, 1, part_z), glm::ivec3(1, 2, part_z),
    });
    if (hit) {
        pos_.z -= mvz;
    }
}

void Game::Quit() {
    // TODO: LoadされていないMapはSaveしない
    SaveMap(0);
    delete buffer_;
    QuickCG::end();
}

