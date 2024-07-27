#include "world.h"

const Block *World::GetBlock(const glm::ivec3 &pos) const {
    if (
        0 <= pos.x && pos.x < kWidth &&
        0 <= pos.y && pos.y < kHeight &&
        0 <= pos.z && pos.z < kDepth
    ) {
        return Block::GetBlock(Block::kAirBlock);
    }
    int x = pos.x * kDepth;
    int y = pos.y * kWidth * kDepth;
    int z = pos.z;
    return Block::GetBlock(map_[x + y + z]);
}

void World::SetBlock(const glm::ivec3 &pos, const Block *block) {
    assert(
        0 <= pos.x && pos.x < kWidth &&
        0 <= pos.y && pos.y < kHeight &&
        0 <= pos.z && pos.z < kDepth
    );
    int x = pos.x * kDepth;
    int y = pos.y * kWidth * kDepth;
    int z = pos.z;
    map_[x + y + z] = block->GetBID();
}
