#include "player.h"

using namespace chibicraft;

Player::Player() {
    // y = 1だったら、床にへばりついている状態なので、床が単色になる(それはそう)
    pos_ = glm::vec3(21.0, 3.5, 11.5);
    dir_ = glm::vec3(0.0, 0.0, 1.0);
    plane_x_ = glm::vec3(0.66, 0.0, 0.0);
    plane_y_ = glm::vec3(0.0, 0.4125, 0.0);
}

void Player::HandleKeys(std::shared_ptr<World> world) {
    float move_speed = frame_time_ * 2.0;

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
        TryMoveX(mvdir.x, world);
        TryMoveY(mvdir.y, world);
        TryMoveZ(mvdir.z, world);
    }

    int n_visible_blocks = Block::kNBlocks - 1;
    if (QuickCG::keyPressed(SDLK_RIGHT)) {
        select_block_ = select_block_ % n_visible_blocks + 1;
    }
    if (QuickCG::keyPressed(SDLK_LEFT)) {
        select_block_ = (select_block_ - 2 + n_visible_blocks) % n_visible_blocks + 1;
    }
}

void Game::HandleMouseMove(int mouse_x, int mouse_y, std::shared_ptr<Screen> screen) {
    float rot_speed = frame_time_ * 0.05;

    SDL_WarpMouse(screen->GetWidth() / 2, screen->GetHeight() / 2);
    if (mouse_x != screen->GetWidth() / 2) {
        int delta_x = mouse_x - screen->GetWidth() / 2;
        dir_ = glm::rotateY(dir_, rot_speed * delta_x);
        plane_x_ = glm::rotateY(plane_x_, rot_speed * delta_x);
        plane_y_ = glm::rotateY(plane_y_, rot_speed * delta_x);
    }
    if (mouse_y != screen->GetHeight() / 2) {
        int delta_y = mouse_y - screen->GetHeight() / 2;
        float angle = rot_speed * delta_y;
        TryRotateY(angle);
    }
}

void Game::OnLeftButtonPress(std::shared_ptr<Screen> screen) {
    Ray ray;
    bool hit = CastRay(screen->GetWidth() / 2, screen->GetHeight() / 2, ray);
    if (!hit || ray.perp_wall_dist > kPlayerDestBlockDist) {
        return;
    }
    world_->SetBlock(ray.pos, Block::GetBlock(Block::kAirBlock));
}

void Game::OnRightButtonPress(std::shared_ptr<Screen> screen) {
    Ray ray;
    bool hit = CastRay(screen->GetWidth() / 2, screen->GetHeight() / 2, ray);
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

    assert(world_->GetBlock(block_pos)->GetBID() == Block::kAirBlock);

    world_->SetBlock(block_pos, Block::GetBlock(select_block_));
    hit = HitBlock({
        glm::ivec3(0, 0, 0), glm::ivec3(0, 0, 1),
        glm::ivec3(0, 1, 0), glm::ivec3(0, 1, 1),
        glm::ivec3(0, 2, 0), glm::ivec3(0, 2, 1),
        glm::ivec3(1, 0, 0), glm::ivec3(1, 0, 1),
        glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1),
        glm::ivec3(1, 2, 0), glm::ivec3(1, 2, 1),
    });
    if (hit) {
        world_->SetBlock(block_pos, Block::GetBlock(Block::kAirBlock));
    }
}

void Game::HandleInput(std::shared_ptr<World> world, std::shared_ptr<Screen> screen) {
    HandleKeys(world);

    int mouse_x, mouse_y;
    bool lmb, rmb;
    QuickCG::getMouseState(mouse_x, mouse_y, lmb, rmb);

    HandleMouseMove(mouse_x, mouse_y, screen);
    if (!prev_lmb_ && lmb) {
        OnLeftButtonPress(world, screen);
    }
    if (!prev_rmb_ && rmb) {
        OnRightButtonPress(world, screen);
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

bool Game::HitBlock(const std::vector<glm::ivec3> &parts, std::shared_ptr<World> world) const {
    bool hit = false;
    for (const glm::ivec3 &part : parts) {
        hit |= !world->GetBlock(GetPlayerPartPos(part))->IsAir();
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

void Game::TryMoveX(float mvx, std::shared_ptr<World> world) {
    pos_.x += mvx;
    int part_x = mvx < 0 ? 0 : 1;
    bool hit = HitBlock({
        glm::ivec3(part_x, 0, 0), glm::ivec3(part_x, 0, 1),
        glm::ivec3(part_x, 1, 0), glm::ivec3(part_x, 1, 1),
        glm::ivec3(part_x, 2, 0), glm::ivec3(part_x, 2, 1),
    }, world);
    if (hit) {
        pos_.x -= mvx;
    }
}

void Game::TryMoveY(float mvy, std::shared_ptr<World> world) {
    pos_.y += mvy;
    int part_y = mvy < 0 ? 0 : 2;
    bool hit = HitBlock({
        glm::ivec3(0, part_y, 0), glm::ivec3(0, part_y, 1),
        glm::ivec3(1, part_y, 0), glm::ivec3(1, part_y, 1),
    }, world);
    if (hit) {
        pos_.y -= mvy;
    }
}

void Game::TryMoveZ(float mvz, std::shared_ptr<World> world) {
    pos_.z += mvz;
    int part_z = mvz < 0 ? 0 : 1;
    bool hit = HitBlock({
        glm::ivec3(0, 0, part_z), glm::ivec3(0, 1, part_z), glm::ivec3(0, 2, part_z),
        glm::ivec3(1, 0, part_z), glm::ivec3(1, 1, part_z), glm::ivec3(1, 2, part_z),
    }, world);
    if (hit) {
        pos_.z -= mvz;
    }
}

