#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "block.h"

// TODO: Manage chunks
class World {
public:
    static constexpr const int kHeight = 64;
    static constexpr const int kWidth = 64;
    static constexpr const int kDepth = 64;

private:
    mutable BID_T map_[kHeight * kWidth * kDepth];

public:
    const Block *GetBlock(const glm::ivec3 &pos) const;
    void SetBlock(const glm::ivec3 &pos, const Block *block);
    void Save();
void Init();
};

