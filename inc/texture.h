#pragma once

#include <string>
#include <vector>
#include <array>

namespace chibicraft {

using TID_T = int;

class Texture {
public:
    static constexpr const int kNTextures = 6;

    static constexpr const int kWidth    = 16;
    static constexpr const int kHeight   = 16;

    static const std::string kDefaultDirectory;

    TID_T GetTID() const { return tid_; }
    std::string GetName() const { return name_; }
    uint32_t GetPixel(int x, int y) const;

    static const Texture *GetTexture(TID_T tid);

private:
    static std::array<Texture, kNTextures> texture_table_;

    TID_T tid_;
    std::string name_;

    std::vector<uint32_t> contents_;

    Texture(TID_T tid, std::string name);
    void Load();
};

}
