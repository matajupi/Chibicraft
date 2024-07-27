#pragma once

#include "screen.h"
#include "raycaster.h"
#include "player.h"
#include "world.h"
#include "cursor.h"

class Game {
    Screen screen_;
    Raycaster raycaster_;
    Player player_;
    World world_;
    Cursor cursor_;

    float time_;
    float old_time_;
    float frame_time_;

    bool Done() const;
    void Quit();

public:
    Game() : Game(-1, -1, true) { }
    Game(int screen_width, int screen_height, bool fullscreen = true);
    void Start();
};

