#pragma once

#include <string>
#include <glm/glm.hpp>

#include "block.h"

namespace chibicraft {

using CID_T = int;

class Chunk {
public:
    static constexpr const int kChunkWidth   = 16;
    static constexpr const int kChunkHeight  = 16;
    static constexpr const int kChunkDepth   = 16;
    static const std::string kDefaultChunkDirectory;

    Chunk() { }
    Chunk(CID_T cid);
    CID_T GetCID() const { return cid_; }
    bool Exists() const;
    void Generate();
    void Load();
    void Save();
    const Block *GetBlock(const glm::ivec3 &pos) const;
    void SetBlock(const glm::ivec3 &pos, const Block *block);

private:
    BID_T contents_[kChunkHeight * kChunkWidth * kChunkDepth];
    CID_T cid_;

    BID_T GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BID_T bid);

    std::string GetFilePath() const;
};

}
