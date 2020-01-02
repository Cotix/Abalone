//
// Created by noah on 22-12-19.
//

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "Game.h"

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define SHIFT(X, Y)  ((Y) > 0 ? (X)<<(Y) : (X)>>(-Y))

const int ROW_COUNT = 9;
const int ROW_SIZE = 11;
const int UP_LEFT = ROW_SIZE;
const int UP_RIGHT = ROW_SIZE-1;
const int LEFT = 1;
const int RIGHT = -1;
const int DOWN_LEFT = -ROW_SIZE+1;
const int DOWN_RIGHT = -ROW_SIZE;
const int DIRECTIONS[] = {UP_LEFT, UP_RIGHT, LEFT, RIGHT, DOWN_LEFT, DOWN_RIGHT};
const int DIRECTION_COUNT = 6;
const int TANGENT_DIRECTIONS[] = {LEFT, UP_RIGHT, 0, LEFT, UP_LEFT, 0, UP_LEFT, UP_RIGHT, 0, UP_LEFT, UP_RIGHT, 0, LEFT, DOWN_RIGHT, 0, LEFT, DOWN_LEFT};
const __uint128_t ONE = 1;
const __uint128_t MSB = ONE<<127;

__uint128_t *transpositions;
// 8G = 268435367
// 4G = 134217689
// 1024MB = 33554393
// 128MB = 4194301
// 32MB = 1048573
// 8MB = 262139
// 2MB = 65521
// 32768 per mb
// There are two transposition tables
// One that is meant to be accessed often, and should be smaller to prevent a lot of swapping
// And one that is used for transpositions that took a lot of work and can be bigger (But not too big)
// Somehow a huge transposition table actually decreases performance. This probably has to do with page swapping
uint64_t transposition_size_short = 4194301; //
uint64_t transposition_size_long = 134217689; // RAM
uint64_t transposition_size = transposition_size_short+transposition_size_long;
const __uint128_t PLAYING_FIELD =
        (((__uint128_t) 0b00000111110) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00001111110) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00011111110) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00111111110) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b01111111110) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b01111111100) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b01111111000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b01111110000) << ROW_SIZE * 1)  +
        (((__uint128_t) 0b01111100000) << ROW_SIZE * 0);

Game::Game(unsigned int players) {
    assert(players == 2);
    transpositions = static_cast<__uint128_t *>(malloc(transposition_size*sizeof(__uint128_t)*2));
    memset(transpositions, 0, sizeof(__uint128_t) * transposition_size*2);
    this->board[2] = 0;
    this->board[3] = 0;
    this->players = players;
    if (players == 2) {
        this->board[0] =
                0b01111100000 |
                (((__uint128_t) 0b01111110000) << ROW_SIZE * 1) |
                (((__uint128_t) 0b00011100000) << ROW_SIZE * 2);

        this->board[1] =
                (((__uint128_t) 0b00000111110) << ROW_SIZE * 8) |
                (((__uint128_t) 0b00001111110) << ROW_SIZE * 7) |
                (((__uint128_t) 0b00000111000) << ROW_SIZE * 6);
        this->piece_count[0] = 14;
        this->piece_count[1] = 14;
    }
}

void Game::daisy() {
    this->board[0] =
            (((__uint128_t) 0b00001100000) << ROW_SIZE * 0) |
            (((__uint128_t) 0b00101010000) << ROW_SIZE * 1) |
            (((__uint128_t) 0b00000110000) << ROW_SIZE * 2) |
            (((__uint128_t) 0b00001100000) << ROW_SIZE * 6) |
            (((__uint128_t) 0b00001010100) << ROW_SIZE * 7) |
            (((__uint128_t) 0b00000110000) << ROW_SIZE * 8);

    this->board[1] =
            (((__uint128_t) 0b01100000000) << ROW_SIZE * 0) |
            (((__uint128_t) 0b01010100000) << ROW_SIZE * 1) |
            (((__uint128_t) 0b00110000000) << ROW_SIZE * 2) |
            (((__uint128_t) 0b00000001100) << ROW_SIZE * 6) |
            (((__uint128_t) 0b00000101010) << ROW_SIZE * 7) |
            (((__uint128_t) 0b00000000110) << ROW_SIZE * 8);
    assert((this->board[0] & this->board[1]) == 0);
    assert(((this->board[0] | this->board[1]) & ~PLAYING_FIELD) == 0);
    this->piece_count[0] = 14;
    this->piece_count[1] = 14;
}

void Game::random_position(int stones) {
    this->piece_count[0] = stones;
    this->piece_count[1] = stones;
    this->board[0] = 0;
    this->board[1] = 0;
    for (int i = 0; i != stones; ++i) {
        for (int p = 0; p != 2; ++p) {
            __uint128_t stone = 0;
            while ((stone & PLAYING_FIELD & (~(this->board[0] | this->board[1]))) == 0)
                stone = ONE << (rand() % 100);
            this->board[p] |= stone;
        }
    }
}

std::string Game::_print_bits(unsigned int from, unsigned int count) {
    std::string result = std::string();

    for (unsigned int i = from; i != from-count; --i) {
        if ((this->board[0]&(ONE<<i)) != 0) result += "A ";
        else if ((this->board[1]&(ONE<<i)) != 0) result += "B ";
        else result += ". ";
    }
    return result;
}

std::string Game::to_string() {
    std::string result = std::string();
    result += "    "; result += this->_print_bits(ROW_SIZE*8+5, 5); result += "\n";
    result += "   "; result += this->_print_bits(ROW_SIZE*7+6, 6); result += "\n";
    result += "  "; result += this->_print_bits(ROW_SIZE*6+7, 7); result += "\n";
    result += " "; result += this->_print_bits(ROW_SIZE*5+8, 8); result += "\n";
    result += ""; result += this->_print_bits(ROW_SIZE*4+9, 9); result += "\n";
    result += " "; result += this->_print_bits(ROW_SIZE*3+9, 8); result += "\n";
    result += "  "; result += this->_print_bits(ROW_SIZE*2+9, 7); result += "\n";
    result += "   "; result += this->_print_bits(ROW_SIZE*1+9, 6); result += "\n";
    result += "    "; result += this->_print_bits(ROW_SIZE*0+9, 5); result += "\n";
    return result;
}
inline __uint8_t reverse_byte(__uint8_t b) {
    return ((b * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}


inline __uint128_t flip_board(__uint128_t board) {
    unsigned int rows[9] = {0,0,0,0,0,0,0,0,0};
    rows[0] = reverse_byte((board>>(ROW_SIZE * 0 + 5)) & 0xFF) << 2;
    rows[1] = reverse_byte((board>>(ROW_SIZE * 1 + 4)) & 0xFF) << 2;
    rows[2] = reverse_byte((board>>(ROW_SIZE * 2 + 3)) & 0xFF) << 2;
    rows[3] = reverse_byte((board>>(ROW_SIZE * 3 + 2)) & 0xFF) << 2;


    unsigned int row = (board>>(ROW_SIZE * 4 + 1));
    unsigned int first_bit = row&0x100;
    row &= 0xFF;
    rows[4] = ((reverse_byte(row) << 1) | (first_bit >> 8)) << 1;


    rows[5] = reverse_byte((board>>(ROW_SIZE * 5 + 1)) & 0xFF) << 1;
    rows[6] = reverse_byte((board>>(ROW_SIZE * 6 + 1)) & 0xFF);
    rows[7] = reverse_byte((board>>(ROW_SIZE * 7 + 1)) & 0xFF) >> 1;
    rows[8] = reverse_byte((board>>(ROW_SIZE * 8 + 1)) & 0xFF) >> 2;

    return (((__uint128_t) rows[0]) << ROW_SIZE*0) |
            (((__uint128_t) rows[1]) << ROW_SIZE*1) |
            (((__uint128_t) rows[2]) << ROW_SIZE*2) |
            (((__uint128_t) rows[3]) << ROW_SIZE*3) |
            (((__uint128_t) rows[4]) << ROW_SIZE*4) |
            (((__uint128_t) rows[5]) << ROW_SIZE*5) |
            (((__uint128_t) rows[6]) << ROW_SIZE*6) |
            (((__uint128_t) rows[7]) << ROW_SIZE*7) |
            (((__uint128_t) rows[8]) << ROW_SIZE*8);
};

void Game::transform() {
    __uint128_t orig[2] = {this->board[0], this->board[1]};
    __uint128_t boards[2] = {flip_board(this->board[0]), flip_board(this->board[1])};
    this->board[0] = boards[0];
    this->board[1] = boards[1];
}

inline __uint128_t Game::get_neighbours(__uint128_t position, const int group_size, const __uint128_t group) {
    __uint128_t result = 0;
    for (int i = 0; i != DIRECTION_COUNT; ++i) {
        __uint128_t tmp = position;
        for (int j = 0; j != group_size; ++j) {
            tmp &= group;
            tmp = (SHIFT(tmp, DIRECTIONS[i])) & PLAYING_FIELD;
        }
        result |= tmp;
    }
    return result;
}

inline __uint128_t Game::get_groups(__uint128_t position, const int group_size, const int direction) {
    __uint128_t result = position;
    for (int j = 1; j < group_size; ++j) {
        result &= SHIFT(result, direction);
    }
    return result;
}

inline __uint128_t Game::make_groups(__uint128_t position, const int group_size, const int direction) {
    __uint128_t result = position;
    for (int j = 1; j < group_size; ++j) {
        result |= SHIFT(result, direction);
    }
    return result;
}

inline __uint128_t Game::get_sumitos(__uint128_t board, __uint128_t opponent, const int group_size, const int direction) {
    __uint128_t result = board;
    for (int j = 1; j < group_size; ++j) {
        result = (result & SHIFT(result, direction)) & PLAYING_FIELD;
    }
    result = (opponent & SHIFT(result, direction));
    if (group_size == 3) {
        result = SHIFT(result, direction);
    }
    result = (result & SHIFT(~(board|opponent)&~PLAYING_FIELD, -direction));
    return result;
}

inline TranspositionData Game::trans_get_data(uint64_t idx, int player) {
    __uint128_t b1 = transpositions[idx];
    __uint128_t b2 = transpositions[idx + 1];
    int d = (transpositions[idx]>>(128ul-32ul));
    TranspositionData data = *((TranspositionData*) &d);
    if ((b1&PLAYING_FIELD) == (this->board[0]&PLAYING_FIELD) &&
        (b2&PLAYING_FIELD) == (this->board[1]&PLAYING_FIELD) &&
        data.player == player) {
        return data;
    } else if (b1 != 0 && b2 != 0) {
        //colision
    }
    return TranspositionData(0, 0, 0, 0, 0, 0);
}

inline TranspositionData Game::trans_get(int player, bool check_transform) {
    uint64_t hash = this->hash();

    uint64_t idx_short = (hash % transposition_size_short) * 2;
    uint64_t idx_long = (hash % transposition_size_long) * 2 + transposition_size_short*2;
    TranspositionData data = this->trans_get_data(idx_short, player);

    if (data.depth > 0) {
        return data;
    } else {
        data = this->trans_get_data(idx_long, player);
        if (data.depth > 0) return data;
    }

    if (data.depth == 0 && check_transform) {
        this->transform();
        hash = this->hash();

        idx_short = (hash % transposition_size_short) * 2;
        idx_long = (hash % transposition_size_long) * 2 + transposition_size_short*2;
        data = this->trans_get_data(idx_short, player);

        if (data.depth == 0) {
            data = this->trans_get_data(idx_long, player);
        }
        this->transform();
    }

    return data;
}

inline int count_bits(__uint128_t v) {
    v = v;
    v = v - ((v >> 1) & (__uint128_t)~(__uint128_t)0/3);                           // temp
    v = (v & (__uint128_t)~(__uint128_t)0/15*3) + ((v >> 2) & (__uint128_t)~(__uint128_t)0/15*3);      // temp
    v = (v + (v >> 4)) & (__uint128_t)~(__uint128_t)0/255*15;                      // temp
    return (__uint128_t)(v * ((__uint128_t)~(__uint128_t)0/255)) >> (sizeof(__uint128_t) - 1) * 8;
}

inline int Game::get_score() {
    return count_bits(this->board[0]) - count_bits(this->board[1]);
}

inline void Game::trans_set(TranspositionData data) {
    uint64_t idx_short = (this->hash() % transposition_size_short) * 2;
    uint64_t idx_long = (this->hash() % transposition_size_long) * 2 + transposition_size_short*2;

    transpositions[idx_short] = this->board[0]&PLAYING_FIELD;
    transpositions[idx_short+1] = this->board[1]&PLAYING_FIELD;
    transpositions[idx_short] |= (*((__uint128_t*) &data)) << (128ul-32ul);

    TranspositionData existing = this->trans_get_data(idx_long, data.player);
    if (existing.work < data.work) {
        transpositions[idx_long] = this->board[0] & PLAYING_FIELD;
        transpositions[idx_long + 1] = this->board[1] & PLAYING_FIELD;
        transpositions[idx_long] |= (*((__uint128_t *) &data)) << (128ul - 32ul);
    }
}


int Game::generate_moves(int player, int depth, int alpha, int beta){
    __uint128_t board = this->board[player];
    __uint128_t enemy = this->board[player^1];
    __uint128_t invalid = board | enemy | ~PLAYING_FIELD;
    __uint128_t valid = ~invalid;
    int tmp = 0;

    int best = -15;
    // sumitos
    for (int group_size = 3; group_size >= 2; --group_size) {
        const int* direction = DIRECTIONS;
        for (; direction != &DIRECTIONS[DIRECTION_COUNT]; ++direction) {
            __uint128_t moves = this->get_sumitos(board, enemy, group_size, *direction);

            while (moves != 0) {
                __uint128_t move = (moves & ~(moves - 1));
                __uint128_t mask = this->make_groups(move, group_size*2 - 1, -*direction);
                __uint128_t my_source = board & mask;
                __uint128_t my_target = SHIFT(my_source, *direction);
                __uint128_t op_source = enemy & mask;
                __uint128_t op_target = SHIFT(op_source, *direction);
                this->board[player] = (this->board[player] ^ my_source ^ my_target) & PLAYING_FIELD;
                this->board[player^1] = (this->board[player^1] ^ op_source ^ op_target) & PLAYING_FIELD;
                tmp = this->evaluate(player, depth, alpha, beta);
                best = MAX(best, tmp);
                alpha = MAX(alpha, tmp);
                this->board[player] = (this->board[player] ^ my_source ^ my_target) & PLAYING_FIELD;
                this->board[player^1] = (this->board[player^1] ^ op_source ^ op_target) & PLAYING_FIELD;
                if (alpha >= beta) {
                    return alpha;
                }
                moves = moves ^ move;
            }
        }
    }

    // Inline moves
    for (int group_size = 3; group_size >= 1; --group_size) {
        __uint128_t moves = this->get_neighbours(board, group_size, board) & valid;
        while (moves != 0) {
            __uint128_t move = (moves & ~(moves - 1));
            __uint128_t sources = this->get_neighbours(move, group_size, board|move) & board;
            while (sources != 0) {
                __uint128_t piece = (sources & ~(sources - 1));
                this->board[player] ^= move ^ piece;
                tmp = this->evaluate(player, depth, alpha, beta);
                best = MAX(best, tmp);
                alpha = MAX(alpha, tmp);
                this->board[player] ^= move ^ piece;
                if (alpha >= beta) {
                    return alpha;
                }
                sources = sources ^ piece;
            }
            moves = moves ^ move;
        }
    }


    // Sideway moves
    for (int group_size = 3; group_size >= 2; --group_size) {
        const int* direction = DIRECTIONS;
        const int* tangent = TANGENT_DIRECTIONS;
        for (; direction != &DIRECTIONS[DIRECTION_COUNT]; ++direction) {
            __uint128_t neighbours = (SHIFT(board, *direction) & valid);
            while(*tangent != 0) {
                __uint128_t moves = this->get_groups(neighbours, group_size, *tangent);

                while (moves != 0) {
                    __uint128_t move = (moves & ~(moves - 1));
                    __uint128_t targets = this->make_groups(move, group_size, -*tangent);
                    __uint128_t sources = SHIFT(targets, -*direction);
                    this->board[player] ^= targets ^ sources;
                    tmp = this->evaluate(player, depth, alpha, beta);
                    best = MAX(best, tmp);
                    alpha = MAX(alpha, tmp);
                    this->board[player] ^= targets ^ sources;
                    if (alpha >= beta) {
                        return alpha;
                    }
                    moves = moves ^ move;
                }
                tangent++;
            }
            tangent++;
        }
    }


    return best;
}

int Game::heuristic(int player) {
    int score = this->get_score() * (1 - 2*player)*10;
    int threegroups = count_bits(this->get_neighbours(this->board[player], 3, this->board[player]));
    threegroups = threegroups * 15 / this->piece_count[player];
    int enemygroups = count_bits(this->get_neighbours(this->board[player^1], 3, this->board[player^1]));
    enemygroups = enemygroups * 15 / this->piece_count[player^1];
    score += threegroups - enemygroups;
    return score;
}

int log2(uint64_t v) {
    if ((v&0xFFFFFFFF00000000ul) != 0) return 31;
    int r;      // result goes here

    static const int MultiplyDeBruijnBitPosition[32] =
            {
                    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
                    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
            };

    v |= v >> 1; // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

int Game::evaluate(int player, int depth, int alpha, int beta) {
    this->position_evaluated++;
    TranspositionData data = this->trans_get(player, false);
    int orig_alpha = alpha;
    int orig_beta = beta;
    if (data.depth != 0 && data.depth >= depth && data.stub == 0) {
        if (data.flag == FLAG_EXACT) {
            return data.score;
        } else if (data.flag == FLAG_LOWER) {
            alpha = MAX(alpha, data.score);
        } else if (data.flag == FLAG_UPPER) {
            beta = MIN(beta, data.score);
        }
        if (alpha >= beta) {
            return data.score;
        }
    }

    if (data.depth > depth && data.stub == 1) {
        // Draw!
//        TranspositionData draw_result(player, depth, 0, 0, FLAG_LOWER, 0);
//        this->trans_set(draw_result);
        return 0;
    }

    int score = 0;
    int work = this->position_evaluated;
    if (depth == 1) {
//        std::cout << this->to_string() << std::endl;
        score = this->heuristic(player);
    } else if (this->piece_count[player^1] - count_bits(this->board[player^1]) >= 6){
        return 127;
    } else {
        TranspositionData stub_result(player, depth, 0, 1, FLAG_EXACT, 10);
        this->trans_set(stub_result);
        score = -this->generate_moves(player ^ 1, depth - 1, -beta, -alpha);
        if (this->trans_get(player, false).stub == 1) {
            TranspositionData clear_result(player, 0, 0, 0, 0, 0);
            this->trans_set(clear_result);
        }
    }
    work = (this->position_evaluated - work) >> 10; // Threshold low work positions
    int flag = score <= orig_alpha ? FLAG_UPPER : (score >= beta ? FLAG_LOWER : FLAG_EXACT);
    TranspositionData result(player, depth, score, 0, flag, log2(work));
    this->trans_set(result);

    return score;
}

inline uint64_t hash128to64(__uint128_t x) {
    x = ((x>>64)*0xcc9e2d51) ^ (x);
    return  x;
}

inline uint64_t Game::hash() {
    return (hash128to64(this->board[0])>>3) ^ hash128to64(this->board[1]);
}
