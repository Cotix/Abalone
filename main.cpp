#include <iostream>
#include "Game.h"

int main() {
    Game game = Game(2);
//    game.daisy();
    std::cout << game.to_string() << std::endl;

    std::cout << game.generate_moves(0, 8, -127, 127) << std::endl << game.position_evaluated << std::endl;

    return 0;
}
