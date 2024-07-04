#include <cstdlib>
#include "game.h"

int main(int argc, char **argv) {
    Game game(960, 600, false);
    game.Start();
    return EXIT_SUCCESS;
}
