#include "world.h"

using namespace chibicraft;

const Block *World::GetBlock(const glm::ivec3 &pos) const {
    if (pos.x < 0 || pos.x >= kWidth ||
        pos.y < 0 || pos.y >= kHeight ||
        pos.z < 0 || pos.z >= kDepth) {
        return Block::GetBlock(Block::kAirBlock);
    }
    int x = pos.x * kDepth;
    int y = pos.y * kWidth * kDepth;
    int z = pos.z;
    return Block::GetBlock(map_[x + y + z]);
}

void World::SetBlock(const glm::ivec3 &pos, const Block *block) {
    if (pos.x < 0 || pos.x >= kWidth ||
        pos.y < 0 || pos.y >= kHeight ||
        pos.z < 0 || pos.z >= kDepth) {
        return; // Nop
    }
    int x = pos.x * kDepth;
    int y = pos.y * kWidth * kDepth;
    int z = pos.z;
    map_[x + y + z] = block->GetBID();
}

void World::Save() {
    // TODO:
}

void World::Init() {
    for (int x = 0; kWidth > x; x++) {
        for (int z = 0; kDepth > z; z++) {
            SetBlock({ x, 0, z }, Block::GetBlock(1));
        }
    }
}
