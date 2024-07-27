
    // ======== Textures ========
    static constexpr const int kNTexs = 6;

    static constexpr const int kTexWidth = 16;
    static constexpr const int kTexHeight = 16;

    static const std::string kTexDir;
    static const std::array<std::string, kNTexs> kTexFiles;
    static const std::array<long long int, kNBlocks> kBlockToTexs;
    std::vector<uint32_t> texs_[kNTexs];

    // face: ブロックの面
    // 0: x-面, 1: x+面, 2: y-面, 3: y+面, 4: z-面, 5: z+面
    int GetTex(int block, int face) const {
        assert(0 <= block && block < kNBlocks && 0 <= face && face < 6);
        return kBlockToTexs[block] >> (face * 8) & 0xff;
    }
    uint32_t GetTexColor(int tex, int x, int y) const {
        assert(0 <= tex && tex < kNTexs && 0 <= x && x < kTexWidth &&
            0 <= y && y < kTexHeight);
        return texs_[tex][kTexWidth * y + x];
    }
    void LoadTexs();

