#include "block.h"

static std::array<Block, kNBlocks> Block::blocks_ = {
    // TODO:
};

Block::Block(BID_T bid, const std::array<TID_T, 6> &textures)
    : bid_(bid), textures_(textures) { }

const Texture *Block::GetTexture(BlockSurface surface) const {
    return Texture::GetTexture(textures_[static_cast<int>(surface)]);
}

static const Block *Block::GetBlock(BID_T bid) {
    // TODO:
}

