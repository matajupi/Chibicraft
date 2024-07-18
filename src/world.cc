#include "world.h"

CID_T World::TransPos(const glm::ivec3 &pos, glm::ivec3 &chpos) const {
    int chunk_x = pos.x / Chunk::kChunkWidth;
    int chunk_y = pos.x / Chunk::kChunkHeight;
    int chunk_z = pos.x / Chunk::kChunkDepth;

    chpos.x = pos.x % Chunk::kChunkWidth;
    chpos.y = pos.y % Chunk::kChunkHeight;
    chpos.z = pos.z % Chunk::kChunkDepth;

    return chunk_x  << 16
         | chunk_y  << 8
         | chunk_z;
}

Chunk &World::GetChunk(CID_T cid) const {
    if (chunks_.find(cid) == chunks_.end()) {
        chunks_.emplace(std::piecewise_construct,
                       std::forward_as_tuple(cid),
                       std::forward_as_tuple(cid));
        if (chunks_[cid].Exists()) {
            chunks_[cid].Load();
        }
        else {
            chunks_[cid].Generate();
        }
    }
    return chunks_[cid];
}

const Block *World::GetBlock(const glm::ivec3 &pos) const {
    glm::ivec3 chpos;
    CID_T cid = TransPos(pos, chpos);
    const Chunk &chunk = GetChunk(cid);
    return chunk.GetBlock(chpos);
}

void World::SetBlock(const glm::ivec3 &pos, const Block *block) {
    glm::ivec3 chpos;
    CID_T cid = TransPos(pos, chpos);
    Chunk &chunk = GetChunk(cid);
    chunk.SetBlock(chpos, block);
}

void World::Save() {
    for (auto &[cid, chunk] : chunks_) {
        chunk.Save();
    }
}
