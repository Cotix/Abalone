#include <iostream>
#include <unistd.h>
#include <cstring>
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
    srand(time(0));
    Game game = Game(2);
//    game.daisy();
    int player = 0;
    int moves = 0;
    while (!game.is_over()) {
        moves++;
        game.position_evaluated = rand()&0b111; // small random initializer
        std::cout << game.to_string() << std::endl;
        std::cout << "Current score: " << game.get_score() << std::endl << std::endl;
        std::cout << "Expected by: " << (player == 0 ? 'A' : 'B') << ": "
                  << game.generate_moves(player, 7, -127, 127, 1)
                  << std::endl << game.position_evaluated << std::endl << std::endl;
        player ^= 1;
    }
    std::cout << "Game won by " << (game.has_won(0) ? 'A' : 'B') << " in " << moves << " moves." << std::endl;

}

int main() {
    demo_play();
    return 0;
}
