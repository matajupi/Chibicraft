
    // ======== Ray ========
    static constexpr const int kMaxRayDist = 30;

    struct Ray {
        glm::vec3 dir;
        glm::ivec3 pos;
        // collision_side: 衝突した面に垂直な軸
        // 0: x軸(面はy-z平面), 1: y軸, 2: z軸
        int collision_side;
        float perp_wall_dist;
        float max_perp_wall_dist;
    };

