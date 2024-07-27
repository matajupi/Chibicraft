    const char kCursorShape[kCursorHeight][kCursorWidth + 1] = {
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
        "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
        "              @@              ",
    };

void Game::DrawCursor() {
    for (int y = 0; kCursorHeight > y; y++) {
        for (int x = 0; kCursorWidth > x; x++) {
            if (kCursorShape[y][x] == '@') {
                SetBufColor(
                    x + screen_width_ / 2 - kCursorWidth / 2,
                    y + screen_height_ / 2 - kCursorHeight / 2,
                    0x19959c
                );
            }
        }
    }
}

