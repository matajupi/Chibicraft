
void Game::SimpleRaycasting() {
#pragma omp parallel for num_threads(4)
    for (int y = 0; screen_height_ > y; y++) {
        for (int x = 0; screen_width_ > x; x++) {
            Ray ray;
            bool hit = CastRay(x, y, ray);
            if (hit) {
                uint32_t color = CalcPixelColor(ray);
                SetBufColor(x, screen_height_ - y - 1, color);
            }
            else {
                SetBufColor(x, screen_height_ - y - 1, 0xFFFFFF);
            }
        }
    }
}

void Game::SlackOffRaycasting() {
#pragma omp parallel for num_threads(4)
    for (int y = 1; screen_height_ > y; y += 3) {
        for (int x = 1; screen_width_ > x; x += 3) {
            Ray ray;
            bool hit = CastRay(x, y, ray);
            if (hit) {
                uint32_t color = CalcPixelColor(ray);
                for (int dy = -1; 1 >= dy; dy++) {
                    for (int dx = -1; 1 >= dx; dx++) {
                        SetBufColor(x + dx, screen_height_ - y - dy - 1, color);
                    }
                }
            }
            else {
                for (int dy = -1; 1 >= dy; dy++) {
                    for (int dx = -1; 1 >= dx; dx++) {
                        SetBufColor(x + dx, screen_height_ - y - dy - 1, 0xFFFFFF);
                    }
                }
            }
        }
    }
}

