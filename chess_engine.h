#pragma once

#include "chess.h"

// 'The maximum number of moves in any reachable chess position is 218' https://www.chessprogramming.org/Chess_Position
#define MAX_MOVES    256

#define PAWN_VALUE   100
#define KNIGHT_VALUE 295
#define BISHOP_VALUE 315
#define ROOK_VALUE   500
#define QUEEN_VALUE  900
#define KING_VALUE   0xffff

#define MATE_VALUE   0xffffff
#define INF_VALUE    0xfffffe

i32 piece_values[PIECE_COUNT] = { PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE, KING_VALUE };


i32 evaluate_position(position_t* pos) {
    u64* pieces = pos->pieces;
    u64* colors = pos->colors;
    i32 value = 0;
    u64 white_mask, black_mask;
    i32 piece_count;
    for(i32 i = 0; i < PIECE_COUNT; i++){
        white_mask = pieces[i] & colors[CWHITE];
        black_mask = pieces[i] & colors[CBLACK];
        piece_count = bit_count(white_mask) - bit_count(black_mask);
        value += piece_values[i] * piece_count;
    }
    return value;
}

move_t moves[MAX_MOVES] = { 0 };


i32 mini(position_t* pos, i32 depth);

i32 maxi(position_t* pos, i32 depth){
    if(depth == 0){
        return evaluate_position(pos);
    }
    i32 score, max = -INF_VALUE;
    i32 moves_count = get_moves(pos, moves);
    for(i32 i = 0; i < moves_count; i++){
        score = mini(pos, depth - 1);
        if(score > max){
            max = score;
        }
    }
    return max;
}

i32 mini(position_t* pos, i32 depth){
    if(depth == 0){
        return -evaluate_position(pos);
    }
    i32 score, min = INF_VALUE;
    i32 moves_count = get_moves(pos, moves);
    for(i32 i = 0; i < moves_count; i++){
        score = mini(pos, depth - 1);
        if(score < min){
            min = score;
        }
    }
}