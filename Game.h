//
// Created by noah on 22-12-19.
//

/*
 *          x x x x x
 *         x x x x x x
 *        x x x x x x x
 *       x x x x x x x x
 *      x x x x x x x x x
 *       x x x x x x x x
 *        x x x x x x x
 *         x x x x x x
 *          x x x x x
 *
 *
 *     . . . .  .x x x x x .
 *     . . . . x x x x x x .
 *     . . . x x x x x x x .
 *     . . x x x x x x x x .
 *     . x x x x x x x x x .
 *     . x x x x x x x x . .
 *     . x x x x x x x . . .
 *     . x x x x x x . . . .
 *     . x x x x x . . . . .
 *    10 9 8 7 6 5 4 3 2 1 0
 */

#ifndef ABALONE_GAME_H
#define ABALONE_GAME_H


#define FLAG_EXACT 0
#define FLAG_LOWER 1
#define FLAG_UPPER 2

#include <cstdint>
#include <string>

struct TranspositionData {
    unsigned int padding: 6;
    unsigned int player: 1;
    unsigned int stub: 1;
    unsigned int depth: 8;
    char score: 8;
    unsigned int flag: 2;
    unsigned int work: 5;

    TranspositionData(int player, int depth, char score, int stub, int flag, int work) {
        this->player = player;
        this->depth = depth;
        this->score = score;
        this->stub = stub;
        this->flag = flag;
        this->work = work;
    }
};


class Game {
    unsigned int players;
    std::string _print_bits(unsigned int from, unsigned int to);
    public:

    __uint128_t get_neighbours(__uint128_t position, const int group_size, const __uint128_t group);
    __uint128_t get_groups(__uint128_t position, const int group_size, const int direction);
    __uint128_t make_groups(__uint128_t position, const int group_size, const int direction);
    int generate_moves(int player, int depth, int alpha, int beta);
    __uint128_t get_sumitos(__uint128_t board, __uint128_t opponent, const int group_size, const int direction);
    int evaluate(int player, int depth, int alpha, int beta);
    unsigned int piece_count[4];

    Game(unsigned int players);

    std::string to_string();

    __uint128_t board[4];
    uint64_t position_evaluated = 0;

    uint64_t hash();

    TranspositionData trans_get(int player, bool check_transform);

    void trans_set(TranspositionData data);

    int get_score();

    void daisy();

    int heuristic(int player);

    TranspositionData trans_get_data(uint64_t idx, int player);

    void random_position(int stones);

    void transform();
};



#endif //ABALONE_GAME_H
