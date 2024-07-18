#include "block.h"

#include <cassert>

// order: z- z+ y- y+ x- x+
std::array<Block, Block::kNBlocks> Block::block_table_ = {
    Block(0, "air", { -1, -1, -1, -1, -1, -1 }),
    Block(1, "grass", { 1, 1, 0, 2, 1, 1 }),
    Block(2, "oak-plank", { 3, 3, 3, 3, 3, 3 }),
    Block(3, "quartz-block-chiseled", { 5, 5, 4, 4, 5, 5 }),
};

Block::Block(BID_T bid, std::string name, const std::array<TID_T, 6> &textures)
    : bid_(bid), name_(name), textures_(textures) { }

const Texture *Block::GetTexture(BlockSurface surface) const {
    return Texture::GetTexture(textures_[static_cast<int>(surface)]);
}

const Block *Block::GetBlock(BID_T bid) {
    assert(0 <= bid && bid < kNBlocks);
    return &block_table_[bid];
}

