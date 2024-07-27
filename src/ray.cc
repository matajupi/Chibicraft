
bool Game::CastRay(int x, int y, Ray &ray) const {
    float camera_y = 2.0 * y / screen_height_ - 1;
    float camera_x = 2.0 * x / screen_width_ - 1;

    ray.dir = dir_ + plane_x_ * camera_x + plane_y_ * camera_y;
    ray.pos = pos_;

    float delta_dist_x = (ray.dir.x == 0) ? 1e30 : std::abs(1 / ray.dir.x);
    float delta_dist_y = (ray.dir.y == 0) ? 1e30 : std::abs(1 / ray.dir.y);
    float delta_dist_z = (ray.dir.z == 0) ? 1e30 : std::abs(1 / ray.dir.z);

    int step_x, step_y, step_z;
    float side_dist_x, side_dist_y, side_dist_z;
    if (ray.dir.x < 0) {
        step_x = -1;
        side_dist_x = (pos_.x - ray.pos.x) * delta_dist_x;
    }
    else {
        step_x = 1;
        side_dist_x = (ray.pos.x + 1.0 - pos_.x) * delta_dist_x;
    }
    if (ray.dir.y < 0) {
        step_y = -1;
        side_dist_y = (pos_.y - ray.pos.y) * delta_dist_y;
    }
    else {
        step_y = 1;
        side_dist_y = (ray.pos.y + 1.0 - pos_.y) * delta_dist_y;
    }
    if (ray.dir.z < 0) {
        step_z = -1;
        side_dist_z = (pos_.z - ray.pos.z) * delta_dist_z;
    }
    else {
        step_z = 1;
        side_dist_z = (ray.pos.z + 1.0 - pos_.z) * delta_dist_z;
    }

    bool hit = false;
    while (!hit) {
        if (side_dist_x <= side_dist_y
         && side_dist_x <= side_dist_z
         && side_dist_x <= kMaxRayDist) {
            side_dist_x += delta_dist_x;
            ray.pos.x += step_x;
            ray.collision_side = 0;
        }
        else if (side_dist_y <= side_dist_z
              && side_dist_y <= kMaxRayDist) {
            side_dist_y += delta_dist_y;
            ray.pos.y += step_y;
            ray.collision_side = 1;
        }
        else if (side_dist_z <= kMaxRayDist) {
            side_dist_z += delta_dist_z;
            ray.pos.z += step_z;
            ray.collision_side = 2;
        }
        else {
            break;
        }
        if (ray.pos.x < 0 || ray.pos.x >= kMapWidth ||
            ray.pos.y < 0 || ray.pos.y >= kMapHeight ||
            ray.pos.z < 0 || ray.pos.z >= kMapDepth) {
            break;
        }
        int block = GetMapBlock(ray.pos);
        hit = block != kAirBlock && block != kTransparentBlock;
    }

    if (ray.collision_side == 0) {
        ray.perp_wall_dist = (side_dist_x - delta_dist_x);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_x);
    }
    else if (ray.collision_side == 1) {
        ray.perp_wall_dist = (side_dist_y - delta_dist_y);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_y);
    }
    else {
        ray.perp_wall_dist = (side_dist_z - delta_dist_z);
        ray.max_perp_wall_dist = (kMaxRayDist - delta_dist_z);
    }

    return hit;
}

uint32_t Game::CalcPixelColor(const Ray &ray) const {
    char block = GetMapBlock(ray.pos);
    float wall_x, wall_y;
    if (ray.collision_side == 0) {
        wall_x = pos_.z + ray.perp_wall_dist * ray.dir.z;
        wall_y = pos_.y + ray.perp_wall_dist * ray.dir.y;
    }
    else if (ray.collision_side == 1) {
        wall_x = pos_.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos_.z + ray.perp_wall_dist * ray.dir.z;
    }
    else {
        wall_x = pos_.x + ray.perp_wall_dist * ray.dir.x;
        wall_y = pos_.y + ray.perp_wall_dist * ray.dir.y;
    }
    wall_x -= floor(wall_x);
    wall_y -= floor(wall_y);

    int tex_x = wall_x * kTexWidth;
    int tex_y = wall_y * kTexHeight;
    int face;
    if (ray.collision_side == 0) {
        if (ray.dir.x > 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 0;
        }
        else {
            face = 1;
        }
    }
    if (ray.collision_side == 1) {
        if (ray.dir.y > 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 2;
        }
        else {
            face = 3;
        }
    }
    if (ray.collision_side == 2) {
        if (ray.dir.z < 0) {
            tex_x = kTexWidth - tex_x - 1;
            face = 4;
        }
        else {
            face = 5;
        }
    }

    int tex = GetTex(block, face);
    uint32_t color = GetTexColor(tex, tex_x, kTexHeight - tex_y - 1);
    if (ray.collision_side == 1 || ray.collision_side == 2) {
        color = (color >> 1) & 0x7F7F7F;
    }
    uint32_t fog = std::min(0xFFu,
        (uint32_t)(0xFF * ray.perp_wall_dist / ray.max_perp_wall_dist));
    color =
        std::min((color >> 16 & 0xFF) + fog, 0xFFu) << 16 |
        std::min((color >> 8  & 0xFF) + fog, 0xFFu) << 8  |
        std::min((color       & 0xFF) + fog, 0xFFu);
    return color;
}

