
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

    // ======== Player ========
    static constexpr const float kPlayerHalfWidth = 0.3;
    static constexpr const float kPlayerHalfDepth = 0.3;
    static constexpr const float kPlayerUpperHalfHeight = 0.1;
    static constexpr const float kPlayerLowerHalfHeight = 1.5;

    static constexpr const float kPlayerPutBlockDist = 4.5;
    static constexpr const float kPlayerDestBlockDist = 4.5;

    glm::vec3 pos_, dir_, plane_x_, plane_y_;
    int select_block_ = 1;

    // QuickCGではMouseのPressが取れないので、flagを持っておく
    bool prev_lmb_;
    bool prev_rmb_;

    void Init();
    void InitScreen();
    void InitPlayer();

    void Update();
    void DrawCursor();
    bool CastRay(int x, int y, Ray &ray) const;
    uint32_t CalcPixelColor(const Ray &ray) const;
    void SimpleRaycasting();
    void SlackOffRaycasting();

    // part: Playerの部位を指定
    // x: 0: x-(Playerのx-面のx座標), 1: x+
    // y: 0: y-, 1: y(Playerの中心のy座標), 2: y+
    // z: 0: z-, 1: z+
    glm::ivec3 GetPlayerPartPos(const glm::ivec3 &part) const;
    bool HitBlock(const std::vector<glm::ivec3> &parts) const;
    void TryRotateY(float angle);
    void TryMoveX(float mvx);
    void TryMoveY(float mvy);
    void TryMoveZ(float mvz);

    void HandleKeys();
    void HandleMouseMove(int mouse_x, int mouse_y);
    void OnLeftButtonPress();
    void OnRightButtonPress();
    void HandleInput();

