#include "game.h"

#include "quickcg.h"

Game::Game(int screen_width, int screen_height, bool fullscreen)
    : fullscreen_(fullscreen), prev_lmb_(false) {
    if (screen_width < 0 || screen_height < 0) {
        Screen::GetDefaultResolution(screen_width, screen_height);
    }
    screen_ = Screen(screen_width, screen_height, fullscreen, "Chibicraft");
    raycaster_ = SimpleRaycaster(3, 3);
}

void Game::Start() {
    while (!Done()) {
        Update();
        player_.HandleInput();
    }
    Quit();
}

void Game::Update() {
    raycaster_.Raycasting(player_, world_, screen_);
    cursor_.Draw();

    old_time_ = time_;
    time_ = QuickCG::getTicks();
    frame_time_ = (time_ - old_time_) / 1000.0;

    screen_.Print(1.0 / frame_time_);
    screen_.Print(select_block_);

    screen_.Update();
}

bool Game::Done() const {
    return QuickCG::done();
}

void Game::Quit() {
    world_.Save();
    QuickCG::end();
}

