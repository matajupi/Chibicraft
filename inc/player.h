#pragma once

#include <glm/glm.hpp>

namespace chibicraft {

class Player {
public:
    static constexpr const float kHalfWidth = 0.3;
    static constexpr const float kHalfDepth = 0.3;
    static constexpr const float kUpperHalfHeight = 0.3;
    static constexpr const float kLowerHalfHeight = 1.3;

    static constexpr const float kPutBlockDist = 3.5;
    static constexpr const float kDestBlockDist = 3.5;

    Player();
    const glm::vec3 &GetPos() const { return pos_; }
    const glm::vec3 &GetDir() const { return dir_; }
    const glm::vec3 &GetPlaneX() const { return plane_x_; }
    const glm::vec3 &GetPlaneY() const { return plane_y_; }
    int GetSelectBlock() const { return select_block_; }

private:
    glm::vec3 pos_, dir_, plane_x_, plane_y_;
    int select_block_ = 1;
};

}

