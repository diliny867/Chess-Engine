
#include "common.h"

/*
56 57 58 59 60 61 62 63 
48 49 50 51 52 53 54 55 
40 41 42 43 44 45 46 47 
32 33 34 35 36 37 38 39 
24 25 26 27 28 29 30 31 
16 17 18 19 20 21 22 23 
 8  9 10 11 12 13 14 15 
 0  1  2  3  4  5  6  7 
*/

typedef enum {
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} piece_t;

typedef enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
    LEFT_UP,
    LEFT_DOWN,
    RIGHT_UP,
    RIGHT_DOWN
} direction_t;


#define WHITE 0
#define BLACK 1

#define RANK_1 0x00000000000000ff
#define RANK_2 0x000000000000ff00
#define RANK_3 0x0000000000ff0000
#define RANK_4 0x00000000ff000000
#define RANK_5 0x000000ff00000000
#define RANK_6 0x0000ff0000000000
#define RANK_7 0x00ff000000000000
#define RANK_8 0xff00000000000000

#define FILE_A 0x0101010101010101
#define FILE_B 0x0202020202020202
#define FILE_C 0x0303030303030303
#define FILE_D 0x0404040404040404
#define FILE_E 0x0505050505050505
#define FILE_F 0x0606060606060606
#define FILE_G 0x0707070707070707
#define FILE_H 0x0808080808080808

#define PUT_BIT(value, mask)      ((value) |=  (mask))
#define TOGGLE_BIT(value, mask)   ((value) ^=  (mask))
#define CLEAR_BIT(value, mask)    ((value) &= ~(mask))
#define GET_BIT(value, mask)      ((value) &   (mask))

#define PUT_BITI(value, index)    PUT_BIT((value),    1 << (index))
#define TOGGLE_BITI(value, index) TOGGLE_BIT((value), 1 << (index))
#define CLEAR_BITI(value, index)  CLEAR_BIT((value),  1 << (index))
#define GET_BITI(value, index)    GET_BIT((value),    1 << (index))

#define PAWN_VALUE   100
#define KNIGHT_VALUE 295
#define BISHOP_VALUE 315
#define ROOK_VALUE   500
#define QUEEN_VALUE  900
#define KING_VALUE   0xffffffff
i64 piece_values[6] = { PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE, KING_VALUE };


#define LSB_ZEROS(val) __builtin_ctzll(val)


// i8 direction_index[8] = { -1, 1, 8, -8, 7, -9, 9, -7 }; 

u64 directions[2][8] = { 0 };    // white / black
u64 pieces[2][6] = { 0 };        // white / black

bool blacks_turn = false;


u64 full_mask8(u64 arr[]){
    return arr[0] | arr[1] | arr[2] | arr[3] | arr[4] | arr[5] | arr[6] | arr[7];
}
u64 full_mask6(u64 arr[]){
    return arr[0] | arr[1] | arr[2] | arr[3] | arr[4] | arr[5];
}
u64 get_attacks(bool black) {
    return full_mask8(directions[black]);
}
u64 get_pieces(bool black) {
    return full_mask6(pieces[black]);
}
u64 get_all_pieces() {
    return get_pieces(0) | get_pieces(1);
}

#define PIECE_HERE(dirs, mask, piece) ((!!(dirs[(piece)]&(mask)))*(piece))
piece_t get_piece(u8 index){
    u64 mask = 1 << index;
    u64* dirs = directions[blacks_turn];
    return PIECE_HERE(dirs, mask, PAWN) + PIECE_HERE(dirs, mask, KNIGHT) + PIECE_HERE(dirs, mask, BISHOP) + PIECE_HERE(dirs, mask, ROOK) + PIECE_HERE(dirs, mask, QUEEN) + PIECE_HERE(dirs, mask, KING);
} 
void put_piece(piece_t piece, u8 index){
    PUT_BITI(directions[blacks_turn][piece], index);
}
void _move_piece(piece_t piece, u8 from, u8 to){
    CLEAR_BITI(directions[blacks_turn][piece], from);
    PUT_BITI(directions[blacks_turn][piece], to);
}
void move_piece(u8 from, u8 to){
    piece_t piece = get_piece(from);
    _move_piece(piece, from, to);
}

void m_put_piece(piece_t piece, u8 mask){
    PUT_BIT(directions[blacks_turn][piece], mask);
}
void m_move_piece(piece_t piece, u64 m_from, u64 m_to){
    CLEAR_BIT(directions[blacks_turn][piece], m_from);
    PUT_BIT(directions[blacks_turn][piece], m_to);
}


u32 pop_lsb(u64* val){
    u32 i = LSB_ZEROS(*val);
    *val &= *val - 1;
    return i;
}
u32 extract_piece(u64* piece_mask){
    return pop_lsb(piece_mask);
}
i64 evaluate_values() {
    i64 value = 0;
    u64 piece_mask;
    u32 piece_index;
    for(i32 i = 0; i <= KING; i++){
        piece_mask = directions[!blacks_turn][i];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value += piece_values[i];
        }
        piece_mask = directions[blacks_turn][i];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value -= piece_values[i];
        }
    }
    return value;
}