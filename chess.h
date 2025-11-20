#pragma once

#include "common.h"
#include "tables.h"


/*
8   56 57 58 59 60 61 62 63 
7   48 49 50 51 52 53 54 55 
6   40 41 42 43 44 45 46 47 
5   32 33 34 35 36 37 38 39 
4   24 25 26 27 28 29 30 31 
3   16 17 18 19 20 21 22 23 
2    8  9 10 11 12 13 14 15 
1    0  1  2  3  4  5  6  7

    A  B  C  D  E  F  G  H
*/

typedef enum {
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    PIECE_COUNT
} piece_e;
typedef i32 piece_t;

#define NO_PIECE    -1
#define ANY_PIECE    0

// #define PAWN   0
// #define KNIGHT 1
// #define BISHOP 2
// #define ROOK   3
// #define QUEEN  4
// #define KING   5

typedef enum{
    CWHITE = 0,
    CBLACK = 1,
    CCOLOR_COUNT
} color_e;

#define NUL_INDEX -1


#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

#define RANK_18 0xFF000000000000FFULL

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

#define FILE_AB 0x0303030303030303ULL
#define FILE_GH 0xC0C0C0C0C0C0C0C0ULL
#define FILE_AH 0x8181818181818181ULL

#define EDGES  0xFF818181818181FFULL


#define put_bits(value, mask)      ((value) |=  (mask))
#define and_bits(value, mask)      ((value) &=  (mask))
#define toggle_bits(value, mask)   ((value) ^=  (mask))
#define clear_bits(value, mask)    ((value) &= ~(mask))
#define get_bits(value, mask)      ((value) &   (mask))

#define put_bit(value, index)    put_bits((value),    1ull << (index))
#define toggle_bit(value, index) toggle_bits((value), 1ull << (index))
#define clear_bit(value, index)  clear_bits((value),  1ull << (index))
#define get_bit(value, index)    get_bits((value),    1ull << (index))

#define expand_full(value) (~(value) + 1)


#define lsb_zeros(val) __builtin_ctzll(val)
#define bit_count(val) __builtin_popcountll(val)
#define zero_array(val) memset(val, 0, sizeof(val))

#define sign(val) (((val) > 0) - ((val) < 0))

void print_mask(u64 val){
    u8* vals = (u8*)&val;
    for(i32 i = 0; i < 8; i++){
        for(i32 j = 0; j < 8; j++){
            printf("%d", (vals[7 - i] >> j) & 1);
        }
        printf("\n");
    }
}

u64 rays[64][64];


u64 pieces[PIECE_COUNT] = { 0 };
u64 colors[CCOLOR_COUNT] = { 0 };


typedef enum{
    CASTLE_OOO = 0, // queenside
    CASTLE_OO       // kingside
} castling_e;
bool lost_castling_rights[CCOLOR_COUNT][2] = { 0 };
u64 castle_pos[CCOLOR_COUNT][2] = { { 0x0000000000000002ULL, 0x0000000000000040ULL }, { 0x0200000000000000ULL, 0x4000000000000000ULL } };
u64 castle_masks[CCOLOR_COUNT][2] = { { 0x000000000000000EULL, 0x0000000000000060ULL }, { 0x0E00000000000000ULL, 0x6000000000000000ULL } };

u64 enpassantable[CCOLOR_COUNT] = { 0, 0 };

u64 all_pieces = 0;
u64 empties = 0;

u64 pinneds[CCOLOR_COUNT] = { 0 };
u64 pinned_masks[CCOLOR_COUNT] = { 0 };

u64 check_masks[CCOLOR_COUNT] = { 0 };

u64 attacks[CCOLOR_COUNT][PIECE_COUNT] = { 0 };
u64 all_attacks[CCOLOR_COUNT] = { 0 };

u64 pseudo_attacks[CCOLOR_COUNT][PIECE_COUNT];
u64 all_pseudo_attacks[CCOLOR_COUNT];

bool blacks_turn = false;

i32 draw_timer = 0;
typedef struct {
    u64 pieces[PIECE_COUNT];
    u64 colors[CCOLOR_COUNT];
    u64 enpassantable;
}  draw_position_t;
draw_position_t draw_positions[50] = { 0 };
i32 draw_positions_count = 0;
i32 draw_counts[50] = { 0 };
bool draw_flag = false;


typedef enum {
    MOVE,
    CAPTURE,
    PUSH,
    DOUBLE_PUSH,
    ENPASSANT,
    CASTLING
} move_type_e;
// typedef struct { // can compress it to one int
//     move_type_e move_type;
//     piece_t piece;
//     bool black;
//     i32 from;
//     i32 to;
//     union {
//         struct {
//             i32 pawn_index;
//         } double_push;
//         struct {
//             i32 pawn_index;
//         } enpassant;
//         struct {
//             i32 rook_from;
//             i32 rook_to;
//         } castling;
//     };
// } move_t;
typedef u64 move_t;
// bits: from: 6 bits  to: 6 bits  type: 3 bits  color: 1 bit  piece: 3 bits  spec1: 3 bits  spec2: 3 bits
typedef enum {
    MOVE_FROM_BIT  = 0x3F,      // 0x3F << 0
    MOVE_TO_BIT    = 0xFC0,     // 0x3F << 6
    MOVE_TYPE_BIT  = 0x7000,    // 0x7 << 12
    MOVE_COLOR_BIT = 0x8000,    // 0x1 << 15
    MOVE_PIECE_BIT = 0x70000,   // 0x7 << 16
    MOVE_SPEC1_BIT = 0x380000,  // 0x7 << 19
    MOVE_SPEC2_BIT = 0x1C00000, // 0x7 << 22
} move_bits;

#define move_get_from( move)  (((move) >> 0ull)  & 0x3F)
#define move_get_to(   move)  (((move) >> 6ull)  & 0x3F)
#define move_get_type( move)  (((move) >> 12ull) & 0x7)
#define move_get_color(move)  (((move) >> 15ull) & 0x1)
#define move_get_piece(move)  (((move) >> 16ull) & 0x7)
#define move_get_spec1(move)  (((move) >> 19ull) & 0x7)
#define move_get_spec2(move)  (((move) >> 22ull) & 0x7)

#define move_set_from( move, val)  ((move) = ((move) & ~MOVE_FROM_BIT)  | ((u64)(val) << 0ull))
#define move_set_to(   move, val)  ((move) = ((move) & ~MOVE_TO_BIT)    | ((u64)(val) << 6ull))
#define move_set_type( move, val)  ((move) = ((move) & ~MOVE_TYPE_BIT)  | ((u64)(val) << 12ull))
#define move_set_color(move, val)  ((move) = ((move) & ~MOVE_COLOR_BIT) | ((u64)(val) << 15ull))
#define move_set_piece(move, val)  ((move) = ((move) & ~MOVE_PIECE_BIT) | ((u64)(val) << 16ull))
#define move_set_spec1(move, val)  ((move) = ((move) & ~MOVE_SPEC1_BIT) | ((u64)(val) << 19ull))
#define move_set_spec2(move, val)  ((move) = ((move) & ~MOVE_SPEC2_BIT) | ((u64)(val) << 22ull))


typedef struct {
    u64 pieces[PIECE_COUNT];
    u64 colors[CCOLOR_COUNT];
    bool lost_castling_rights[CCOLOR_COUNT][2];
    u64 enpassantable[CCOLOR_COUNT];
    u64 pinneds[CCOLOR_COUNT];
    u64 pinned_masks[CCOLOR_COUNT];
    u64 check_masks[CCOLOR_COUNT];
    bool blacks_turn;
} position_t;

// // without edge squares
// u64 bishop_masks[64];
// u64 rook_masks[64];
// void gen_slider_masks(){
//     // generate bishop diagonal attack masks
//     u64 diagonal, anti_diagonal, diagonal_len, anti_diagonal_len, starting_bit;
//     for(i32 r = 0; r < 8; r++){
//         for(i32 f = 0; f < 8; f++){
//             // how much bits are in diagonal
//             diagonal_len = 8 - ((r > f) ? r - f : f - r);
//             anti_diagonal_len = (((r + f) < 8) ? 1 + r + f : 15 - (r + f));
//
//             // go in diagonal directions
//             starting_bit = r > 0 ? r << 3 : f;
//             for(i32 i = 0; i < diagonal_len; i++){
//                 put_bit(diagonal, starting_bit + i * 9);
//             }
//             starting_bit = r > 0 ? r << 3 + 7 : f;
//             for(i32 i = 0; i < anti_diagonal_len; i++){
//                 put_bit(anti_diagonal, starting_bit + i * 7);
//             }        
//
//             bishop_masks[(r << 3) + f] = (diagonal | anti_diagonal) & ~EDGES;
//         }
//     }
//
//     // generate rook horizontal/vertical attack masks
//     u64 rank, file, row;
//     for(i32 r = 0; r < 8; r++){
//         for(i32 f = 0; f < 8; f++){
//             rank = RANK_1 << (r << 3);
//             file = (FILE_A << f) | (FILE_A >> (8 - f));
//             rook_masks[(r << 3) + f] = (rank | file) & ~EDGES;
//         }
//     }
// }

extern u64 bishop_magics[64];
extern u64 rook_magics[64];
extern u32 bishop_shifts[64];
extern u32 rook_shifts[64];
extern u64 bishop_masks[64];
extern u64 rook_masks[64];

u64 bishop_table[64][512];
u64 rook_table[64][4096];


force_inline i32 pop_lsb(u64* val){
    i32 i = lsb_zeros(*val);
    *val &= *val - 1;
    return i;
}
force_inline i32 extract_piece(u64* piece_mask){
    return pop_lsb(piece_mask);
}
force_inline i32 mask_to_index(u64 piece_mask){
    return lsb_zeros(piece_mask);
}

u64 next_subset(u64 subset, u64 set){
    return (subset - set) & set;
}

i32 get_dist_to_edge(i32 index, i32 direction){
    i32 r = index >> 3;
    i32 f = index & 7;
    switch (direction){
    case 1:
        return 7 - f;
    case 8: 
        return 7 - r;
    case -1: 
        return f;
    case -8: 
        return r;
    case 7: 
        return min(7 - r, f);
    case 9: 
        return min(7 - r, 7 - f);
    case -7: 
        return min(r, 7 - f);
    case -9: 
        return min(r, f);
    }
    return 0;
}
u64 gen_slider_attack(i32 index, i32 directions[4], u64 mask){
    u64 attack = 0;
    u64 val, direction, iters, shift;
    for(i32 i = 0; i < 4; i++){
        direction = directions[i];
        iters = get_dist_to_edge(index, direction);
        shift = index;
        for(i32 j = 0; j < iters; j++){
            shift += direction;
            val = 1ull << shift;
            attack |= val;
            if((val & mask) != 0){
                break;
            }
        }
    }
    return attack;
}
void fill_bishop_table(){
    u64 mask, hash, shift, magic, sub, *table;
    for(i32 i = 0; i < 64; i++){
        mask = bishop_masks[i];
        magic = bishop_magics[i];
        shift = bishop_shifts[i];
        table = bishop_table[i];
        sub = 0;
        for(i32 j = 0; j < 512; j++){
            hash = (sub * magic) >> shift;
            table[hash] = gen_slider_attack(i, (i32[4]){ 7, 9, -9, -7 }, sub);
            sub = next_subset(sub, mask);
        }
    }
}
void fill_rook_table(){
    u64 mask, hash, shift, magic, sub, *table;
    for(i32 i = 0; i < 64; i++){
        mask = rook_masks[i];
        magic = rook_magics[i];
        shift = rook_shifts[i];
        table = rook_table[i];
        sub = 0;
        for(i32 j = 0; j < 4096; j++){
            hash = (sub * magic) >> shift;
            table[hash] = gen_slider_attack(i, (i32[4]){ 1, 8, -8, -1 }, sub);
            sub = next_subset(sub, mask);
        }
    }
}

i32 get_direction(i32 from, i32 to){
    if(from == to){
        return 0;
    }
    i32 xf = from & 7;
    i32 yf = from >> 3;
    i32 xt = to & 7;
    i32 yt = to >> 3;
    
    i32 diffx = xt - xf;
    i32 diffy = yt - yf;
    
    i32 direction = 0;
    if(abs(diffx) == abs(diffy)){
        if(diffx > 0 && diffy > 0){
            direction = 9;
        }else if(diffx < 0 && diffy < 0){
            direction = -9;
        }
        if(diffx > 0 && diffy < 0){
            direction = -7;
        }else if(diffx < 0 && diffy > 0){
            direction = 7;
        }
    }else if((diffx == 0) ^ (diffy == 0)){
        if(diffx > 0){
            direction = 1;
        }else if(diffx < 0){
            direction = -1;
        }
        if(diffy > 0){
            direction = 8;
        }else if(diffy < 0){
            direction = -8;
        }
    }
    return direction;
}
u64 gen_ray(i32 from, i32 to){ // non horizontal/vertical/diagonal will be 0
    i32 direction = get_direction(from, to);
    if(direction == 0){
        return 0;
    }
    u64 mask = 0;
    while(from != to){
        put_bit(mask, from);
        from += direction;
    }
    return mask;
}
void fill_rays_table(){
    for(i32 i = 0; i < 64; i++){
        for(i32 j = 0; j < 64; j++){
            rays[i][j] = gen_ray(i, j);
        }
    }
}

// resets on pawn move, capture, castling
void reset_draw_timer(){
    memset(draw_counts, 0, sizeof(draw_counts[0]) * draw_positions_count);
    draw_timer = 0;
}
bool push_draw_position(bool black){ // return true if draw
    if(++draw_timer >= 50){
        return true;
    }
    draw_position_t draw_position;
    memcpy(draw_position.pieces, pieces, sizeof(pieces));
    memcpy(draw_position.colors, colors, sizeof(colors));
    draw_position.enpassantable = enpassantable[black];
    for(i32 i = (i32)black; i < draw_positions_count; i += 2){
        if(memcmp(&draw_positions[i], &draw_position, sizeof(draw_position_t))){
            draw_counts[i]++;
            return draw_counts[i] >= 2; // 2 because we dont increment when pushing new position
        }
    }
    draw_positions[draw_positions_count++] = draw_position;
    return false;
}

force_inline u64 bishop_moves(u64 board, i32 index){
    u64 mask = board & bishop_masks[index];
    u64 hash = (mask * bishop_magics[index]) >> bishop_shifts[index];
    return bishop_table[index][hash];
}
force_inline u64 rook_moves(u64 board, i32 index){
    u64 mask = board & rook_masks[index];
    u64 hash = (mask * rook_magics[index]) >> rook_shifts[index];
    return rook_table[index][hash];
}
force_inline u64 queen_moves(u64 board, i32 index){
    return bishop_moves(board, index) | rook_moves(board, index);
}

force_inline bool index_occupied(i32 index){
    return get_bit(all_pieces, index);
}

force_inline i32 find_piece(bool black, piece_t piece){
    return mask_to_index(pieces[piece] & colors[black]);
}

force_inline bool check_index_piece(piece_t piece, i32 index){
    return !!get_bit(pieces[piece], index);
}

#define piece_check(mask, piece)  (!!(pieces[(piece)] & (mask)))
#define piece_value_here(mask, piece)   (piece_check(mask, piece) * (piece))
force_inline piece_t piece_at(bool black, i32 index){
    u64 mask = get_bit(colors[black], index);
    if(mask == 0){
        return NO_PIECE;
    }
    i32 val = piece_value_here(mask, PAWN) + piece_value_here(mask, KNIGHT) + piece_value_here(mask, BISHOP) 
            + piece_value_here(mask, ROOK) + piece_value_here(mask, QUEEN)  + piece_value_here(mask, KING);
    return val;
}

force_inline piece_t piece_at_all(i32 index, bool* black){
    piece_t piece = piece_at(CWHITE, index);
    if(piece != NO_PIECE){
        *black = CWHITE;
        return piece;
    }
    *black = CBLACK;
    return piece_at(CBLACK, index);
}
force_inline void clear_piece(bool black, piece_t piece, i32 index){
    clear_bit(pieces[piece], index);
    clear_bit(colors[black], index);
}
force_inline void put_piece(bool black, piece_t piece, i32 index){
    put_bit(pieces[piece], index);
    put_bit(colors[black], index);
}
force_inline void move_piece(bool black, piece_t piece, i32 from, i32 to){
    u64 from_mask = 1ull << from;
    u64 to_mask   = 1ull << to;
    clear_bits(pieces[piece], from_mask);
    clear_bits(colors[black], from_mask);
    clear_bits(colors[!black], to_mask);
    for(i32 i = 0; i < PIECE_COUNT; i++){
        clear_bits(pieces[i], to_mask);
    }
    put_bits(pieces[piece], to_mask);
    put_bits(colors[black], to_mask);
}
force_inline void move_piece2(bool black, piece_t piece, piece_t other_piece, i32 from, i32 to){
    u64 from_mask = 1ull << from;
    u64 to_mask   = 1ull << to;
    clear_bits(pieces[piece], from_mask);
    clear_bits(colors[black], from_mask);
    clear_bits(colors[!black], to_mask);
    clear_bits(pieces[other_piece], to_mask);
    put_bits(pieces[piece], to_mask);
    put_bits(colors[black], to_mask);
}
force_inline void move_piece_unknown(bool black, i32 from, i32 to){
    piece_t piece = piece_at(black, from);
    assert(piece != NO_PIECE);
    move_piece(black, piece, from, to);
}

force_inline void put_piece_m(bool black, piece_t piece, u64 mask){
    put_bits(pieces[piece], mask);
    put_bits(colors[black], mask);
}
force_inline void move_piece_m(bool black, piece_t piece, u64 m_from, u64 m_to){
    clear_bits(pieces[piece], m_from);
    clear_bits(colors[black], m_from);
    put_bits(pieces[piece], m_to);
    put_bits(colors[black], m_to);
}

bool in_check(bool black, u64 mask){
    return (pieces[KING] & colors[black]) & mask;
}
u64 get_check_mask(bool black){
    u64 color_pieces = colors[black];
    u64 other_pieces = colors[!black];
    u64 king_mask = pieces[KING] & color_pieces;
    i32 king_index = mask_to_index(king_mask);

    // seems like only max of two types of pieces can double check, so can get away with xor
    if(king_mask & ~(pseudo_attacks[!black][PAWN] ^ pseudo_attacks[!black][KNIGHT] ^ pseudo_attacks[!black][BISHOP] ^ pseudo_attacks[!black][ROOK] ^ pseudo_attacks[!black][QUEEN])){
        return all_pseudo_attacks[!black];
    }

    u64 bishops_mask, rooks_mask;

    bishops_mask = (pieces[BISHOP] | pieces[QUEEN]) & other_pieces & bishop_moves(all_pieces, king_index);  
    rooks_mask   = (pieces[ROOK]   | pieces[QUEEN]) & other_pieces & rook_moves(  all_pieces, king_index);
    // if(bishops_mask && rooks_mask){
    //     return ~all_pseudo_attacks[!black];
    // }

    u64 mask = 0;
    
    while(bishops_mask){
        mask |= rays[extract_piece(&bishops_mask)][king_index]; 
    }
    while(rooks_mask){
        mask |= rays[extract_piece(&rooks_mask)][king_index]; 
    }

    return ~mask;
}
void calculate_check_evasion_mask(bool black){
    if(in_check(black, all_pseudo_attacks[!black])){
        check_masks[black] = get_check_mask(black);
    }else {
        check_masks[black] = 0;
    }
}

u64 bishop_xray(bool black, i32 index){
    u64 blockers = colors[black]; // no point in also checking enemy pieces
    u64 mask = bishop_moves(all_pieces, index); 
    blockers &= mask; // take only attacked pieces
    return mask ^ bishop_moves(all_pieces ^ blockers, index); // out of initially attacked get potential positions for pinners from moves without blocking pieces
}
u64 rook_xray(bool black, i32 index){
    u64 blockers = colors[black];
    u64 mask = rook_moves(all_pieces, index); 
    blockers &= mask;
    return mask ^ rook_moves(all_pieces ^ blockers, index); 
}
void calculate_pinned(bool black){
    u64 color_pieces = colors[black];
    u64 other_pieces = colors[!black];
    u64 pins, pinners, pinned = 0, pin_rays = 0;
    i32 index, king_index = mask_to_index(pieces[KING] & color_pieces);
    
    pinners = bishop_xray(black, king_index) & ((pieces[BISHOP] | pieces[QUEEN]) & other_pieces);
    while(pinners){
        index = pop_lsb(&pinners);
        pin_rays |= rays[index][king_index];
        pins = all_pieces & rays[index][king_index];
        pinned |= (bit_count(pins) == 2) * (pins & color_pieces); // choose pin if it is only one in line to king (source piece included in pins)
    }

    pinners = rook_xray(black, king_index) & ((pieces[ROOK] | pieces[QUEEN]) & other_pieces);
    while(pinners){
        index = pop_lsb(&pinners);
        pin_rays |= rays[index][king_index];
        pins = all_pieces & rays[index][king_index];
        pinned |= (bit_count(pins) == 2) * (pins & color_pieces); // choose pin if it is only one in line to king (source piece included in pins)
    }

    pinneds[black] = pinned;
    pinned_masks[black] = pin_rays;
}

u64 pawns_attack(bool black, u64 piece_mask){
    u64 pawn_attackable = colors[!black] | enpassantable[!black];
    return (((piece_mask << 7 & ~FILE_H) | (piece_mask << 9 & ~FILE_A)) &  (black - 1llu)                      //white side attacks
         |  ((piece_mask >> 7 & ~FILE_A) | (piece_mask >> 9 & ~FILE_H)) & ~(black - 1llu)) & pawn_attackable;  //black side attacks
}
u64 pawns_steps(bool black, u64 piece_mask){
    u64 color_pieces = colors[black];
    return ((((piece_mask << 8) | ((piece_mask & RANK_2) << 16) & (empties << 8)) &  (black - 1llu)                              //white side steps
         |   ((piece_mask >> 8) | ((piece_mask & RANK_7) >> 16) & (empties >> 8)) & ~(black - 1llu)) & empties) & ~color_pieces; //black side steps
}
u64 knights_attack(u64 piece_mask){
    return (piece_mask << 6 & ~FILE_GH) | (piece_mask << 10 & ~FILE_AB) | (piece_mask << 15 & ~FILE_H) | (piece_mask << 17 & ~FILE_A)
         | (piece_mask >> 6 & ~FILE_AB) | (piece_mask >> 10 & ~FILE_GH) | (piece_mask >> 15 & ~FILE_A) | (piece_mask >> 17 & ~FILE_H);
}
u64 bishops_attack(u64 board, u64 piece_mask){
    u64 mask = 0;
    while(piece_mask){
        mask |= bishop_moves(board, extract_piece(&piece_mask)); 
    }
    return mask;
}
u64 rooks_attack(u64 board, u64 piece_mask){
    u64 mask = 0;
    while(piece_mask){
        mask |= rook_moves(board, extract_piece(&piece_mask)); 
    }
    return mask;
}
u64 queens_attack(u64 board, u64 piece_mask){
    u64 mask = 0;
    while(piece_mask){
        mask |= queen_moves(board, extract_piece(&piece_mask)); 
    }
    return mask;
}
u64 kings_attack(u64 piece_mask){
    return (piece_mask << 1 & ~FILE_A) | (piece_mask << 7 & ~FILE_H) | (piece_mask << 8) | (piece_mask << 9 & ~FILE_A)
         | (piece_mask >> 1 & ~FILE_H) | (piece_mask >> 7 & ~FILE_A) | (piece_mask >> 8) | (piece_mask >> 9 & ~FILE_H);
}

u64 get_pseudo_legal_attack(bool black, u64 piece_mask, piece_t piece){
    //u64 color_pieces = colors[black];
    u64 other_pieces = colors[!black];
    u64 board;
    
    u64 mask = 0;
    switch (piece){
    case PAWN:
        mask  = pawns_attack(black, piece_mask);
        break;
    case KNIGHT:
        mask  = knights_attack(piece_mask);
        break;
    case BISHOP: // exclude enemy king from calculating attacks for sliders
        board = all_pieces ^ (pieces[KING] & other_pieces);
        mask  = bishops_attack(board, piece_mask);
        break;
    case ROOK:
        board = all_pieces ^ (pieces[KING] & other_pieces);
        mask  = rooks_attack(board, piece_mask);
        break;
    case QUEEN:
        board = all_pieces ^ (pieces[KING] & other_pieces);
        mask  = queens_attack(board, piece_mask);
        break; 
    case KING:
        mask  = kings_attack(piece_mask);
        break;
    }
    //return mask & ~color_pieces;
    return mask; // dont & ~color_pieces so king cant capture defended piece
}

u64 get_legal_attack(bool black, u64 piece_mask, piece_t piece){ // todo: check
    u64 color_pieces = colors[black];
    //u64 other_pieces = colors[!black];
    u64 pinned = pinneds[black] & pieces[piece];
    u64 pinned_mask = pinned_masks[black];
    u64 nonpinned = piece_mask & ~pinned;
    u64 check_mask = ~check_masks[black];
    
    u64 mask = 0;
    switch (piece){
    case PAWN:
        mask  = pawns_attack(black, nonpinned);
        mask |= pawns_attack(black, pinned) & pinned_mask;
        break;
    case KNIGHT:
        mask  = knights_attack(nonpinned);
        mask |= knights_attack(pinned) & pinned_mask;
        break;
    case BISHOP:
        mask  = bishops_attack(all_pieces, nonpinned);
        mask |= bishops_attack(all_pieces, pinned) & pinned_mask;
        break;
    case ROOK:
        mask  = rooks_attack(all_pieces, nonpinned);
        mask |= rooks_attack(all_pieces, pinned) & pinned_mask;
        break;
    case QUEEN:
        mask  = queens_attack(all_pieces, nonpinned);
        mask |= queens_attack(all_pieces, pinned) & pinned_mask;
        break; 
    case KING:
        mask  = kings_attack(piece_mask); // king cant get pinned
        check_mask = ~all_pseudo_attacks[!black];
        break;
    }
    return mask & ~color_pieces & check_mask;
}

void calculate_pseudo_legal_attacks(bool black){
    u64 color_pieces = colors[black];
    for(i32 i = 0; i < PIECE_COUNT; i++){
        pseudo_attacks[black][i] = get_pseudo_legal_attack(black, pieces[i] & color_pieces, i);
    }
    all_pseudo_attacks[black] = 0;
    for(i32 i = 0; i < PIECE_COUNT; i++){
        all_pseudo_attacks[black] |= pseudo_attacks[black][i];
    }
}
void calculate_attacks(bool black){
    u64 color_pieces = colors[black];
    for(i32 i = 0; i < PIECE_COUNT; i++){
        attacks[black][i] = get_legal_attack(black, pieces[i] & color_pieces, i);
    }
    all_attacks[black] = 0;
    for(i32 i = 0; i < PIECE_COUNT; i++){
        all_attacks[black] |= attacks[black][i];
    }
}
force_inline void calculate_all_attacks(){
    // calculate_pseudo_legal_attacks(CWHITE); // first
    // calculate_pseudo_legal_attacks(CBLACK);

    // calculate_pinned(CWHITE);
    // calculate_pinned(CBLACK);

    // calculate_check_evasion_mask(CWHITE);
    // calculate_check_evasion_mask(CBLACK);

    // calculate_attacks(CWHITE); // last
    // calculate_attacks(CBLACK);


    // can skip calculation for both sides
    calculate_pseudo_legal_attacks(!blacks_turn); // first
    calculate_pseudo_legal_attacks(blacks_turn);  // for correct enemy turn visualisation
    calculate_pinned(blacks_turn);                // for legal turns
    calculate_check_evasion_mask(blacks_turn);    // for legal turns
    // calculate_attacks(blacks_turn);            // last
}

// check if we can castle, then check with if all necessary for castling squares are free (at ==), then choose those squares (at *), then choose position 
force_inline u64 get_castling_mask(bool black, bool castle_type){ // castling dont care if pinned piece cant go to attack, it still doest work, so can use pseudo attacks 
    return (!lost_castling_rights[black][castle_type] && ((castle_masks[black][castle_type] & empties & ~all_pseudo_attacks[!black]) == castle_masks[black][castle_type])) 
           * castle_pos[black][castle_type];
}

u64 get_legal_turns(bool black, u64 piece_mask, piece_t piece){ //todo: add pins
    u64 color_pieces = colors[black];
    u64 pinned = pinneds[black] & pieces[piece];
    u64 pinned_mask = pinned_masks[black];
    u64 nonpinned = piece_mask & ~pinned;
    u64 check_mask = ~check_masks[black];

    u64 mask = get_legal_attack(black, piece_mask, piece);
    if(piece == PAWN){
        mask |= (pawns_steps(black, nonpinned)
             |   pawns_steps(black, pinned) & pinned_mask) & check_mask;
    }else if(piece == KING && !in_check(black, all_pseudo_attacks[!black])){
        mask |= (get_castling_mask(black, CASTLE_OOO)
             |   get_castling_mask(black, CASTLE_OO)) & ~color_pieces;
    }
    return mask;
}
u64 get_color_legal_turns(bool black){
    u64 color = colors[black];
    u64 mask = 0;
    for(piece_t piece = PAWN; piece <= KING; piece++){
        mask |= get_legal_turns(black, pieces[piece] & color, piece);
    }
    return mask;
}
force_inline u64 get_index_legal_turns(bool black, i32 index, piece_t piece){
    return get_legal_turns(black, 1ull << index, piece);
}

bool is_checkmate(bool black){
    if(!in_check(black, all_pseudo_attacks[!black])){
        return false;
    }

    // todo: do better
    return !get_color_legal_turns(black); // if any turns, then not checkmate
}
bool is_draw(){
    return draw_flag;
}
bool detect_draw(bool black){
    u64 no_kings_board = (colors[black] | colors[!black]) ^ pieces[KING]; // dont use all_pieces here
    i32 piece_count = bit_count(no_kings_board);
    if(piece_count == 0){
        return true;
    }else if(piece_count == 1){
        if(pieces[KNIGHT] || pieces[KNIGHT]){
            return true;
        }
    }else if(piece_count == 2){
        bool two_knights = !(no_kings_board ^ (pieces[KNIGHT] & colors[black]));
        if(two_knights){
            return true;
        }
        // bool two_bishops = !(no_kings_board ^ (pieces[BISHOP] & colors[black]));
        // if(two_bishops){ // sometimes can sometimes cant
        //     return true;
        // }
    }
    return false;
}

force_inline void promote(bool black, i32 index, piece_t piece){
    clear_bit(pieces[PAWN], index);
    put_bit(pieces[piece], index);
}

void setup_next_turn(){
    all_pieces = colors[CWHITE] | colors[CBLACK];
    empties = ~all_pieces;
    calculate_all_attacks();
}

// returns move count
i32 get_moves(position_t pos, move_t* moves){
    return 0;
}


void remove_castling_rights(bool black, piece_t piece, i32 from){
    if(piece == KING){
        lost_castling_rights[black][CASTLE_OOO] = true;
        lost_castling_rights[black][CASTLE_OO] = true;
    }
    if(piece == ROOK){
        if(from == 0 || from == 56){
            lost_castling_rights[black][CASTLE_OOO] = true;
        }
        else if(from == 7 || from == 63){
            lost_castling_rights[black][CASTLE_OO] = true;
        }
        }
}
void do_move(move_t move){
    bool black = move_get_color(move);
    piece_t piece = move_get_piece(move);
    i32 from = move_get_from(move);
    i32 to = move_get_to(move);
    move_type_e move_type = move_get_type(move);

    move_piece(black, piece, from, to);

    memset(enpassantable, 0, sizeof(enpassantable)); //clear both enpassantables, to show correct moves for other color
    switch(move_type) {
    case MOVE:
        remove_castling_rights(black, piece, from);
        if(push_draw_position(black)){
            draw_flag = true;
        }
        break;
    case CAPTURE:
        remove_castling_rights(black, piece, from);
        reset_draw_timer();
        if(detect_draw(black)){
            draw_flag = true;
        }
        break;
    case PUSH:
        reset_draw_timer();
        break;
    case DOUBLE_PUSH:
        reset_draw_timer();
        put_bit(enpassantable[black], move_get_spec1(move));
        break;
    case ENPASSANT:
        reset_draw_timer();
        clear_piece(!black, PAWN, move_get_spec1(move));
        break;
    case CASTLING:
        reset_draw_timer();
        move_piece(black, ROOK, move_get_spec1(move), move_get_spec2(move));
        break;
    }

    blacks_turn = !blacks_turn;

    setup_next_turn();
}

void clear_board(){
    memset(pieces, 0, sizeof(pieces));
    memset(colors, 0, sizeof(colors));
    memset(lost_castling_rights, 0, sizeof(lost_castling_rights));
    memset(pinneds, 0, sizeof(pinneds));
    memset(pinned_masks, 0, sizeof(pinned_masks));
    memset(check_masks, 0, sizeof(check_masks));
    memset(attacks, 0, sizeof(attacks));
    memset(all_attacks, 0, sizeof(all_attacks));
    memset(pseudo_attacks, 0, sizeof(pseudo_attacks));
    memset(all_pseudo_attacks, 0, sizeof(all_pseudo_attacks));
    memset(enpassantable, 0, sizeof(enpassantable));
    blacks_turn = false;
    draw_timer = 0;
    draw_flag = false;
}
void setup_board(){
    clear_board();
    for(i32 i = 0; i < 8; i++){
        put_piece(CWHITE, PAWN, 8 + i);
    }
    put_piece(CWHITE, ROOK, 0);
    put_piece(CWHITE, ROOK, 7);
    put_piece(CWHITE, KNIGHT, 1);
    put_piece(CWHITE, KNIGHT, 6);
    put_piece(CWHITE, BISHOP, 2);
    put_piece(CWHITE, BISHOP, 5);
    put_piece(CWHITE, QUEEN, 3);
    put_piece(CWHITE, KING, 4);

    for(i32 i = 0; i < 8; i++){
        put_piece(CBLACK, PAWN, 48 + i);
    }
    put_piece(CBLACK, ROOK, 56);
    put_piece(CBLACK, ROOK, 63);
    put_piece(CBLACK, KNIGHT, 57);
    put_piece(CBLACK, KNIGHT, 62);
    put_piece(CBLACK, BISHOP, 58);
    put_piece(CBLACK, BISHOP, 61);
    put_piece(CBLACK, QUEEN, 59);
    put_piece(CBLACK, KING, 60);

    // put_piece(CBLACK, PAWN, 48);
    // put_piece(CWHITE, KNIGHT, 57);
    // put_piece(CWHITE, KING, 4);
    // put_piece(CBLACK, KING, 60);
}

void chess_reset(){
    setup_board();
    setup_next_turn();
}

void chess_init(){
    fill_rays_table();
    fill_bishop_table();
    fill_rook_table();

    chess_reset();
}