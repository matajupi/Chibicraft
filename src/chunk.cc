#include "chunk.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <exception>

using namespace chibicraft;

const std::string Chunk::kDefaultChunkDirectory = "data/chunks/";

BID_T Chunk::GetBlock(int x, int y, int z) const {
    assert(0 <= x && x < kChunkWidth &&
        0 <= y && y < kChunkHeight &&
        0 <= z && z < kChunkDepth);
    int idx = y * kChunkWidth * kChunkDepth + x * kChunkDepth + z;
    return contents_[idx];
}

void Chunk::SetBlock(int x, int y, int z, BID_T bid) {
    assert(0 <= x && x < kChunkWidth &&
        0 <= y && y < kChunkHeight &&
        0 <= z && z < kChunkDepth);
    int idx = y * kChunkWidth * kChunkDepth + x * kChunkDepth + z;
    contents_[idx] = bid;
}

std::string Chunk::GetFilePath() const {
    std::stringstream ss;
    ss << kDefaultChunkDirectory
       << std::setfill('0') << std::setw(8) << std::hex << cid_ << ".map";
    return ss.str();
}

Chunk::Chunk(CID_T cid) : cid_(cid) { }

bool Chunk::Exists() const {
    std::string path = GetFilePath();
    std::ifstream ifs(path, std::ios::binary);
    return ifs.is_open();
}

void Chunk::Generate() {
    int posy = cid_ >> 8  & 0xff;
    for (int y = 0; kChunkHeight > y; y++) {
        for (int x = 0; kChunkWidth > x; x++) {
            for (int z = 0; kChunkDepth > z; z++) {
                if (posy == 0 && y == 0) {
                    SetBlock(x, y, z, 1);
                }
                else {
                    SetBlock(x, y, z, 0);
                }
            }
        }
    }
}

void Chunk::Load() {
    std::string path = GetFilePath();

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Failed to open chunk \"" << cid_ << "\"" <<  std::endl;
        std::terminate();
    }

    ifs.seekg(0, std::ios::end);
    int file_size = ifs.tellg();
    int chunk_size = kChunkHeight * kChunkDepth * kChunkWidth;
    if (file_size < chunk_size) {
        std::cerr << "Error: Failed to load chunk \"" << cid_ << "\"" << std::endl;
        std::terminate();
    }

    ifs.seekg(0);
    ifs.read(contents_, chunk_size);
}

void Chunk::Save() {
    std::string path = GetFilePath();

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Failed to open chunk \"" << cid_ << "\"" << std::endl;
        std::terminate();
    }

    int chunk_size = kChunkHeight * kChunkDepth * kChunkWidth;
    ofs.write(contents_, chunk_size);
}

const Block *Chunk::GetBlock(const glm::ivec3 &pos) const {
    BID_T bid = GetBlock(pos.x, pos.y, pos.z);
    return Block::GetBlock(bid);
}

void Chunk::SetBlock(const glm::ivec3 &pos, const Block *block) {
    SetBlock(pos.x, pos.y, pos.z, block->GetBID());
}

