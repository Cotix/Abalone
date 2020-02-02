#include <iostream>
#include <unistd.h>
#include <cstring>

#include "Game.h"
#include "util.h"

void demo_play() {
    srand(time(0));
    int score_normal = 0;
    int score_experimental = 0;
    for (int game = 0; game != 10; ++game) {
        Game games[2] = {Game(2, game&1), Game(2, (game&1)^1)};

        games[0].daisy();
        games[1].daisy();
        int player = 0;
        int moves = 0;
        while (!games[0].is_over() && moves <= 600) {
            moves++;
            int score;
            score = games[player].iterative_search(player, 1000, 1);
//            std::cout << "Expected by: " << (player == 0 ? 'A' : 'B') << ": "
//                      << score << std::endl;
//            std::cout << "Searched depth: " << games[player].last_depth << std::endl;
//            std::cout << games[player].to_string() << std::endl;
            games[player ^ 1].board[0] = games[player].board[0];
            games[player ^ 1].board[1] = games[player].board[1];
            player ^= 1;
        }
        if (moves != 601) {
            if (games[0].has_won((game&1)^1)) {
                std::cout << "Game won by experimental agent " << " in " << moves << " moves." << std::endl;
                score_experimental++;
            } else {
                std::cout << "Game won by regular agent " << " in " << moves << " moves." << std::endl;
                score_normal++;
            }
        } else {
            std::cout << "Game drawed!" << std::endl;
        }
    }
    std::cout << "Experimental vs Regular ended in " << score_experimental << "-" << score_normal << std::endl;


}

void stdin_play() {
    Game game = Game(2, 1);
    while (!game.is_over()) {
        std::string line;
        std::getline(std::cin, line);
        game.board[0] = str_uint128(line);
        std::getline(std::cin, line);
        game.board[1] = str_uint128(line);
        std::cerr << "Received move:"  << std::endl;
        std::cerr << game.to_string() << std::endl;
        int score = game.iterative_search(1, 2000, 1);
        std::cerr << "Expected score: " << score << std::endl;
        std::cerr << game.to_string() << std::endl;
        std::cout << uint128_str(game.board[0]) << std::endl << uint128_str(game.board[1]) << std::endl;
    }
    std::cerr << game.has_won(1) ? "Game won!" : "Game lost";
}

int main() {
//    demo_play();
    stdin_play();
}
