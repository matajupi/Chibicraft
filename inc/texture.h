#pragma once

#include <string>
#include <vector>
#include <array>

using TID_T = int;

class Texture {
public:
    static constexpr const int kNTextures = 6;

    static constexpr const int kTextureWidth    = 16;
    static constexpr const int kTextureHeight   = 16;

    static const std::string kTextureDefaultDirectory;

private:
    static std::array<Texture, kNTextures> texture_table_;

    TID_T tid_;
    std::string name_;
    bool loaded_;

    std::vector<uint32_t> contents_;

    Texture(TID_T tid, std::string name);

public:
    Texture() { }

    TID_T GetTID() const { return tid_; }
    std::string GetName() const { return name_; }
    uint32_t GetPixel(int x, int y) const;
    void Load();
    static const Texture *GetTexture(TID_T tid);
};
