#include <iostream>
#include "Game.h"


void benchmark() {
    srand(123);
    Game game = Game(2);
    game.generate_moves(0, 8, -127, 127, 0);
    for (int i = 0; i != 10; ++i) {
        game.random_position(6);
        game.generate_moves(0, 10, -127, 127, 0);
    }
    game.daisy();
    game.generate_moves(0, 10, -127, 127, 0);

    std::cout << game.position_evaluated << std::endl;
    // Prints 111396198 on commit 375c5b9 in 7sec user time 9.3 sec realtime
}

void demo_play() {
    Game game = Game(2);
    game.daisy();
    int player = 0;
    while (true) {
        std::cout << game.to_string() << std::endl;
        game.position_evaluated = 0;
        std::cout << game.generate_moves(player, 10, -127, 127, 1) << std::endl << game.position_evaluated << std::endl;
        player ^= 1;
    }
}

int main() {
    benchmark();
    return 0;
}
