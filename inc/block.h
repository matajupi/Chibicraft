#pragma once

#include <string>
#include <array>

#include "texture.h"

namespace chibicraft {

using BID_T = char;

enum class BlockSurface {
    XY_Lower,
    XY_Higher,
    XZ_Lower,
    XZ_Higher,
    YZ_Lower,
    YZ_Higher,
};

class Block {
public:
    static constexpr const int kNBlocks = 4;
    static constexpr const BID_T kAirBlock = 0;

    BID_T GetBID() const { return bid_; }
    std::string GetName() const { return name_; }
    const Texture *GetTexture(BlockSurface surface) const;
    bool IsAir() const { return bid_ == kAirBlock; }
    static const Block *GetBlock(BID_T bid);

private:
    static std::array<Block, kNBlocks> block_table_;

    BID_T bid_;
    std::string name_;
    std::array<TID_T, 6> textures_;

    Block(BID_T bid, std::string name, const std::array<TID_T, 6> &textures);
};

}
