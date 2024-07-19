#pragma once

#include <map>
#include <vector>
#include <glm/glm.hpp>

#include "chunk.h"
#include "block.h"

// TODO: Chunk page out algorithm
class World {
    // mutable std::map<CID_T, Chunk> chunks_;
    mutable BID_T buffer_[kWorldBufferHeight * kWorldBufferWidth * kWorldBufferDepth];

    CID_T TransPos(const glm::ivec3 &pos, glm::ivec3 &chpos) const;
    Chunk &GetChunk(CID_T cid) const;

public:
    const Block *GetBlock(const glm::ivec3 &pos) const;
    void SetBlock(const glm::ivec3 &pos, const Block *block);
    void Save();
};

