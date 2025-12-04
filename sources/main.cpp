#include "game.h"

int main(void)
{
    Game game = {0};
    game.init();
    game.loop();
    game.destroy();
}
