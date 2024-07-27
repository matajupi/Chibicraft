#include "texture.h"

#include <cassert>
#include <iostream>
#include <exception>

#include "quickcg.h"

const std::string Texture::kDefaultDirectory
    = "bedrock-samples/resource_pack/textures/blocks/";

std::array<Texture, Texture::kNTextures> Texture::texture_table_ = {
    Texture(0, "grass_carried.png"),            // 0
    Texture(1, "grass_side_carried.png"),       // 1
    Texture(2, "dirt.png"),                     // 2
    Texture(3, "planks_big_oak.png"),           // 3
    Texture(4, "quartz_block_chiseled_top.png"),// 4
    Texture(5, "quartz_block_chiseled.png"),    // 5
};

Texture::Texture(TID_T tid, std::string name)
    : tid_(tid), name_(name) {
    contents_.resize(kWidth * kHeight);
    Load();
}

uint32_t Texture::GetPixel(int x, int y) const {
    assert(0 <= x && x < kWidth && 0 <= y && y < kHeight);
    return contents_[y * kWidth + x];
}

void Texture::Load() {
    std::string path = kDefaultDirectory + name_;

    unsigned long tw, th;
    int err =  QuickCG::loadImage(contents_, tw, th, path);
    if (err) {
        std::cerr << "Error: Failed to load texture \"" << name_ << "\"" << std::endl;
        std::terminate();
    }
}

const Texture *Texture::GetTexture(TID_T tid) {
    assert(0 <= tid && tid < kNTextures);
    return &texture_table_[tid];
}

