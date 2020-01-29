#include <iostream>
#include <unistd.h>
#include <cstring>
#include "Game.h"

void demo_play() {
    srand(time(0));
    Game games[2] = {Game(2, 1), Game(2, 0)};

    games[0].daisy();
    games[1].daisy();
    int player = 0;
    int moves = 0;
    while (!games[0].is_over() && moves <= 2000) {
        moves++;
        int score;
        if (player == 0) {
            score = games[0].iterative_search(0, 5000, 1);
        } else {
            score = games[1].iterative_search(1, 5000, 1);
        }
        std::cout << "Expected by: " << (player == 0 ? 'A' : 'B') << ": "
                  << score << std::endl;

        std::cout << games[player].to_string() << std::endl;

        games[player ^ 1].board[0] = games[player].board[0];
        games[player ^ 1].board[1] = games[player].board[1];
        player ^= 1;
    }
    if (moves != 2001)
        std::cout << "Game won by " << (games[0].has_won(0) ? 'A' : 'B') << " in " << moves << " moves." << std::endl;
    else
        std::cout << "Game drawed " << " in " << moves << " moves." << std::endl;



}

int main() {
    demo_play();
//    Game game = Game(2, 1);
//    game.daisy();
//    std::cout << game.iterative_search(0, 1000, 1) << std::endl;
//    std::cout << game.to_string() << std::endl;
//    std::cout << game.position_evaluated/1000000.0 << "M" << std::endl;
//    return 0;
}
