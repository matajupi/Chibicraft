#pragma once

#include <array>

#include "texture.h"

using BID_T = char;

enum class BlockSurface {
    // TODO:
};

class Block {
public:
    static constexpr const int kNBlocks = 10;
    static constexpr const int kAirBlock = 0;

private:
    BID_T bid_;
    std::array<TID_T, 6> textures_;

    static std::array<Block, kNBlocks> blocks;

public:
    Block(BID_T bid, const std::array<TID_T, 6> &textures);
    const Texture *GetTexture(BlockSurface surface) const;
    static const Block *GetBlock(BID_T bid);
};
