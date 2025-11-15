#pragma once

#include "common.h"
#include "tables.h"

#include "raylib.h"


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


#define PUT_BITS(value, mask)      ((value) |=  (mask))
#define AND_BITS(value, mask)      ((value) &=  (mask))
#define TOGGLE_BITS(value, mask)   ((value) ^=  (mask))
#define CLEAR_BITS(value, mask)    ((value) &= ~(mask))
#define GET_BITS(value, mask)      ((value) &   (mask))

#define PUT_BIT(value, index)    PUT_BITS((value),    1ull << (index))
#define TOGGLE_BIT(value, index) TOGGLE_BITS((value), 1ull << (index))
#define CLEAR_BIT(value, index)  CLEAR_BITS((value),  1ull << (index))
#define GET_BIT(value, index)    GET_BITS((value),    1ull << (index))

#define EXPAND_FULL(value) (~(value) + 1ull)

#define PAWN_VALUE   100
#define KNIGHT_VALUE 295
#define BISHOP_VALUE 315
#define ROOK_VALUE   500
#define QUEEN_VALUE  900
#define KING_VALUE   0xffffffff
i64 piece_values[PIECE_COUNT] = { PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE, KING_VALUE };


#define LSB_ZEROS(val) __builtin_ctzll(val)


u64 pieces[PIECE_COUNT] = { 0 };
u64 colors[CCOLOR_COUNT] = { 0 };

// OOO - queenside, OO - kingside
#define CASTLE_OOO 0
#define CASTLE_OO  1
bool castling_rights[2][2] = { { true, true }, { true, true } };
// #define OOO_POS 0x0200000000000002ULL
// #define OO_POS  0x4000000000000040ULL
u64 castle_pos[2] = { 0x0200000000000002ULL, 0x4000000000000040ULL };
u64 castle_masks[2][2] = { { 0x000000000000000EULL, 0x0000000000000060ULL }, { 0x0E00000000000000ULL, 0x6000000000000000ULL } };

u64 enpassantable = 0;

u64 all_attacks[2] = { 0, 0 };

bool blacks_turn = false;


i32 selected_index = NUL_INDEX;
i32 last_turn[2] = { NUL_INDEX, NUL_INDEX };
piece_t selected_piece = NO_PIECE;
piece_t dragged_piece = NO_PIECE;
bool selected_black = false;
u64 selected_turns = 0;

bool in_promotion = false;
i32 promoted_to = NUL_INDEX;
i32 promotion_indexes[4];
piece_t promotion_pieces[4] = { QUEEN, KNIGHT, ROOK, BISHOP };


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
//                 PUT_BIT(diagonal, starting_bit + i * 9);
//             }
//             starting_bit = r > 0 ? r << 3 + 7 : f;
//             for(i32 i = 0; i < anti_diagonal_len; i++){
//                 PUT_BIT(anti_diagonal, starting_bit + i * 7);
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
    i32 i = LSB_ZEROS(*val);
    *val &= *val - 1;
    return i;
}
force_inline i32 extract_piece(u64* piece_mask){
    return pop_lsb(piece_mask);
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

force_inline u64 get_all_pieces() {
    return colors[CWHITE] | colors[CBLACK];
}

#define PIECE_CHECK(pcs, mask, piece)  (!!(pcs[(piece)] & (mask)))
#define PIECE_HERE(pcs, mask, piece)   (PIECE_CHECK(pcs, mask, piece) * (piece))
force_inline piece_t find_piece(bool black, i32 index){
    u64 mask = colors[black] & (1ull << index);
    if(mask == 0){
        return NO_PIECE;
    }
    i32 val = PIECE_HERE(pieces, mask, PAWN) + PIECE_HERE(pieces, mask, KNIGHT) + PIECE_HERE(pieces, mask, BISHOP) 
            + PIECE_HERE(pieces, mask, ROOK) + PIECE_HERE(pieces, mask, QUEEN)  + PIECE_HERE(pieces, mask, KING);
            // + (mask == 0) * NO_PIECE;
    return val;
}

force_inline piece_t find_piece_all(i32 index, bool* black){
    piece_t piece = find_piece(CWHITE, index);
    if(piece != NO_PIECE){
        *black = CWHITE;
        return piece;
    }
    *black = CBLACK;
    return find_piece(CBLACK, index);
}
force_inline void put_piece(bool black, piece_t piece, i32 index){
    PUT_BIT(pieces[piece], index);
    PUT_BIT(colors[black], index);
}
force_inline void move_piece(bool black, piece_t piece, i32 from, i32 to){
    CLEAR_BIT(pieces[piece], from);
    CLEAR_BIT(colors[black], from);
    u64 to_mask = 1ull << to;
    CLEAR_BITS(colors[!black], to_mask);
    for(i32 i = 0; i < PIECE_COUNT; i++){
        CLEAR_BITS(pieces[i], to_mask);
    }
    PUT_BITS(pieces[piece], to_mask);
    PUT_BITS(colors[black], to_mask);
}
force_inline void move_piece_unknown(bool black, i32 from, i32 to){
    piece_t piece = find_piece(black, from);
    assert(piece != NO_PIECE);
    move_piece(black, piece, from, to);
}

force_inline void put_piece_m(bool black, piece_t piece, u64 mask){
    PUT_BITS(pieces[piece], mask);
    PUT_BITS(colors[black], mask);
}
force_inline void move_piece_m(bool black, piece_t piece, u64 m_from, u64 m_to){
    CLEAR_BITS(pieces[piece], m_from);
    CLEAR_BITS(colors[black], m_from);
    PUT_BITS(pieces[piece], m_to);
    PUT_BITS(colors[black], m_to);
}

force_inline u64 get_pin_mask(bool black, i32 index){ // all 1 if no pin
    return 0;
}

force_inline bool in_check(bool black, u64 attacks){
    return (pieces[KING] & colors[black]) & attacks;
}

// check with if all necessary for castling squares are free (at ==), then choose those squares (at *), then check if we can castle, then choose position 
force_inline u64 get_castling_mask(bool black, bool castle_type, u64 empties){
    return ((castle_masks[black][castle_type] & empties & ~all_attacks[!black]) == castle_masks[black][castle_type]) 
           * castle_masks[black][castle_type] & EXPAND_FULL(castling_rights[black][castle_type]) & castle_pos[castle_type];
}

u64 get_turns_m(bool black, u64 piece_mask, piece_t piece){ //todo: add pins
    u64 mask = 0;
    u64 color_pieces = colors[black];
    u64 other_pieces = colors[!black];
    u64 all_pieces = color_pieces | other_pieces;
    u64 empties = ~all_pieces;
    u64 castle_blocks;
    switch (piece){
    case PAWN: //todo: enpassant
        mask |= (((piece_mask << 7 & ~FILE_H) | (piece_mask << 9 & ~FILE_A)) &  (black - 1llu)                  //white side attacks
             |   ((piece_mask >> 7 & ~FILE_A) | (piece_mask >> 9 & ~FILE_H)) & ~(black - 1llu)) & other_pieces  //black side attacks
             |   ((piece_mask << 8) | ((piece_mask & RANK_2) << 16) & (empties << 8)) &  (black - 1llu)         //white side steps
             |   ((piece_mask >> 8) | ((piece_mask & RANK_7) >> 16) & (empties >> 8)) & ~(black - 1llu);        //black side steps
        break;
    case KNIGHT:
        mask |= (piece_mask << 6 & ~FILE_GH) | (piece_mask << 10 & ~FILE_AB) | (piece_mask << 15 & ~FILE_H) | (piece_mask << 17 & ~FILE_A)
             |  (piece_mask >> 6 & ~FILE_AB) | (piece_mask >> 10 & ~FILE_GH) | (piece_mask >> 15 & ~FILE_A) | (piece_mask >> 17 & ~FILE_H);
        break;
    case BISHOP:
        while(piece_mask){
            mask |= bishop_moves(all_pieces, extract_piece(&piece_mask)); 
        }
        break;
    case ROOK:
        while(piece_mask){
            mask |= rook_moves(all_pieces, extract_piece(&piece_mask)); 
        }
        break;
    case QUEEN:
        while(piece_mask){
            mask |= queen_moves(all_pieces, extract_piece(&piece_mask)); 
        }
        break; 
    case KING:
        mask |= get_castling_mask(black, CASTLE_OOO, empties);
        mask |= get_castling_mask(black, CASTLE_OO, empties);
        mask |= (piece_mask << 1 & ~FILE_A) | (piece_mask << 7 & ~FILE_H) | (piece_mask << 8) | (piece_mask << 9 & ~FILE_A)
             |  (piece_mask >> 1 & ~FILE_H) | (piece_mask >> 7 & ~FILE_A) | (piece_mask >> 8) | (piece_mask >> 9 & ~FILE_H);
        break;
    }
    return mask & ~color_pieces;
}
u64 get_color_turns_m(bool black){
    u64 color = colors[black];
    u64 mask = 0;
    for(piece_t piece = PAWN; piece <= KING; piece++){
        mask |= get_turns_m(black, pieces[piece] & color, piece);
    }
    return mask;
}
force_inline u64 get_index_turns_m(bool black, i32 index, piece_t piece){
    return get_turns_m(black, 1ull << index, piece);
}

force_inline void promote(bool black, i32 index, piece_t piece){
    CLEAR_BIT(pieces[PAWN], index);
    PUT_BIT(pieces[piece], index);
}

i64 evaluate_values() {
    i64 value = 0;
    u64 piece_mask;
    i32 piece_index;
    for(i32 i = 0; i <= KING; i++){
        piece_mask = pieces[i] & colors[CWHITE];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value += piece_values[i];
        }
        piece_mask = pieces[i] & colors[CBLACK];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value -= piece_values[i];
        }
    }
    return value;
}

void setup_turn(){
    all_attacks[CWHITE] = get_color_turns_m(CWHITE);
    all_attacks[CBLACK] = get_color_turns_m(CBLACK);

}

void clear_board(){
    memset(pieces, 0, sizeof(pieces));
    memset(colors, 0, sizeof(colors));
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

}


i32 mouse_x = 0;
i32 mouse_y = 0;
bool mouse_left = false;
bool mouse_right = false;
i32 hover_index = NUL_INDEX;

// 128x128
Texture2D piece_textures[CCOLOR_COUNT][PIECE_COUNT];

Sound capture_sound;
Sound move_sound;
Sound notify_sound;

#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    800

#define SQUARE_SIDE      100
#define HALF_SQUARE_SIDE 50

#define COLOR_BRIGHT   (Color){0xFF, 0xCF, 0x9F, 0xFF}
#define COLOR_DARK     (Color){0xD2, 0x8C, 0x45, 0xFF}

#define SELECTED_TINT  (Color){0x71, 0x81, 0x68, 0x7F}
#define SELECTED_HOVER (Color){0x71, 0x81, 0x68, 0x5F}
#define SELECTED_MID   (Color){0x71, 0x81, 0x68, 0x4F}
#define SELECTED_BLANK (Color){0x71, 0x81, 0x68, 0x00}
#define SELECTED_COLOR (Color){0x71, 0x81, 0x68, 0xFF}

#define MOVE_HIGHLIGHT (Color){0xF7, 0xF1, 0x1D, 0x50}
#define MOVE_HIGHLIGHT2 (Color){0x8B, 0xCE, 0xF7, 0x6F}

Texture2D load_texture(char* file_path){
    Texture2D texture = LoadTexture(file_path);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    return texture;
}

void load_assets(){
    piece_textures[CWHITE][PAWN]   = load_texture("resources/white-pawn.png");
    piece_textures[CBLACK][PAWN]   = load_texture("resources/black-pawn.png");
    piece_textures[CWHITE][KNIGHT] = load_texture("resources/white-knight.png");
    piece_textures[CBLACK][KNIGHT] = load_texture("resources/black-knight.png");
    piece_textures[CWHITE][BISHOP] = load_texture("resources/white-bishop.png");
    piece_textures[CBLACK][BISHOP] = load_texture("resources/black-bishop.png");
    piece_textures[CWHITE][ROOK]   = load_texture("resources/white-rook.png");
    piece_textures[CBLACK][ROOK]   = load_texture("resources/black-rook.png");
    piece_textures[CWHITE][QUEEN]  = load_texture("resources/white-queen.png");
    piece_textures[CBLACK][QUEEN]  = load_texture("resources/black-queen.png");
    piece_textures[CWHITE][KING]   = load_texture("resources/white-king.png");
    piece_textures[CBLACK][KING]   = load_texture("resources/black-king.png");

    capture_sound = LoadSound("resources/capture.mp3");
    move_sound    = LoadSound("resources/move-self.mp3");
    notify_sound  = LoadSound("resources/notify.mp3");
}

void index_to_position(i32 index, i32* x, i32* y){
    *x = (index & 7) * SQUARE_SIDE;
    *y = SCREEN_HEIGHT - SQUARE_SIDE - ((index >> 3) * SQUARE_SIDE);
}
i32 position_to_index(i32 x, i32 y){
    i32 index = x / SQUARE_SIDE + (((SCREEN_HEIGHT - y) / SQUARE_SIDE) << 3);
    return index;
}
void draw_square(i32 x, i32 y, Color color){
    DrawRectangle(x, y, SQUARE_SIDE, SQUARE_SIDE, color);
}
void draw_square_by_index(i32 index, Color color){
    i32 x, y;
    index_to_position(index, &x, &y);
    draw_square(x, y, color);
}
void draw_piece(i32 x, i32 y, bool black, piece_t piece){
    //DrawTexture(piece_textures[black][piece], x, y, WHITE);
    DrawTexturePro(piece_textures[black][piece], (Rectangle){ 0.f, 0.f, 128.f, 128.f }, (Rectangle){ (f32)x, (f32)y, SQUARE_SIDE, SQUARE_SIDE }, (Vector2){ 0.f, 0.f }, 0, WHITE);
}
void draw_piece_by_index(i32 index, bool black, piece_t piece){
    i32 x, y;
    index_to_position(index, &x, &y);
    draw_piece(x, y, black, piece);
}
void draw_pieces_mask(u64 mask, bool black, piece_t piece){
    i32 index;
    while(mask){
        index = extract_piece(&mask);
        draw_piece_by_index(index, black, piece);
    }
}
void draw_attack(i32 x, i32 y){
    DrawCircleGradient(x, y, 15, SELECTED_MID, SELECTED_COLOR);
}
void draw_attacks(u64 mask){
    i32 index, x, y;
    while(mask){
        index = extract_piece(&mask);
        index_to_position(index, &x, &y);
        draw_attack(x + HALF_SQUARE_SIDE, y + HALF_SQUARE_SIDE);
    }
}
void draw_pieces(){
    for(i32 i = 0; i < CCOLOR_COUNT; i++){
        for(i32 j = PAWN; j < PIECE_COUNT; j++){
            draw_pieces_mask(pieces[j] & colors[i], i, j);
        }
    }
}
void draw_selected(){
    if(last_turn[0] != NUL_INDEX && last_turn[1] != NUL_INDEX){
        draw_square_by_index(last_turn[0], MOVE_HIGHLIGHT);
        draw_square_by_index(last_turn[1], MOVE_HIGHLIGHT);
    }
    if(selected_index != NUL_INDEX){
        draw_square_by_index(selected_index, SELECTED_TINT);
        draw_attacks(selected_turns);
    }
    if(dragged_piece != NO_PIECE){   
        if(GET_BIT(selected_turns, hover_index)){
            draw_square_by_index(hover_index, SELECTED_HOVER);
        }
        draw_piece(mouse_x - HALF_SQUARE_SIDE, mouse_y - HALF_SQUARE_SIDE, selected_black, dragged_piece);
    }
}
void draw_promotion(){
    if(!in_promotion){
        return;
    }
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SELECTED_HOVER);
    i32 index;
    for(i32 i = 0; i < 4; i++){
        index = promotion_indexes[i];
        draw_piece_by_index(index, blacks_turn, promotion_pieces[i]);
        if(index == hover_index){
            draw_square_by_index(index, SELECTED_TINT);
        }
    }
}
void draw_board() {
    Color color;
    for(i32 i = 0; i < 64; i++){
        color = ((i + !(i & 8)) & 1) ? COLOR_DARK : COLOR_BRIGHT;
        draw_square_by_index(i, color);
    }
    char text_letter[2] = { 'a', '\0' };
    char text_number[2] = { '1', '\0' };
    int font_size = 20;
    for(i32 i = 0; i < 8; i++){
        color = (i & 1) ? COLOR_DARK : COLOR_BRIGHT; 
        DrawText(text_letter, 85 + i * SQUARE_SIDE, SCREEN_HEIGHT - 5 - font_size                     , font_size, color);
        DrawText(text_number, 5                   , SCREEN_HEIGHT - (75 + i * SQUARE_SIDE) - font_size, font_size, color);
        text_letter[0]++;
        text_number[0]++;
    }
}

void chess_init(){

    setup_board();
    setup_turn();

    fill_bishop_table();
    fill_rook_table();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");

    InitAudioDevice();

    SetTargetFPS(60);

    load_assets();
}

void chess_clean(){
    for(i32 i = 0; i < CCOLOR_COUNT; i++){
        for(i32 j = PAWN; j < PIECE_COUNT; j++){
            UnloadTexture(piece_textures[i][j]);
        }
    }

    UnloadSound(capture_sound);
    UnloadSound(move_sound);
    UnloadSound(notify_sound);

    CloseAudioDevice();

    CloseWindow();
}

bool can_move(i32 new_index){
    return GET_BIT(selected_turns, new_index) && selected_black == blacks_turn;
}

bool check_promotion(i32 new_index){
    if(selected_piece == PAWN && GET_BIT(pieces[PAWN] & colors[blacks_turn] & RANK_18, new_index)){
        in_promotion = true;
        promoted_to = new_index;
        for(i32 i = 0; i < 4; i++){
            promotion_indexes[i] = new_index + (blacks_turn ? 8 : -8) * i;
        }
        return true;
    }
    return false;
}

void end_turn(){
    blacks_turn = !blacks_turn;
    setup_turn();
}

void deselect_piece(){
    selected_index = NUL_INDEX;
    selected_piece = NO_PIECE;
    dragged_piece = NO_PIECE;
}
void select_piece(i32 index){
    selected_index = index;
    selected_piece = find_piece_all(selected_index, &selected_black);
    if(selected_black == blacks_turn){
        dragged_piece = selected_piece;
    }
    selected_turns = get_index_turns_m(selected_black, selected_index, selected_piece);
}

bool move_is_castling(piece_e piece, i32 from, i32 to){
    if(piece != KING){
        return false;
    }
    i32 diff = abs(from - to);
    return diff == 3 || diff == 2; // pretty rudimentary castling checking
}

void do_move(i32 new_index){
    if(GET_BIT(colors[!blacks_turn], new_index)){
        PlaySound(capture_sound);
    }else{
        PlaySound(move_sound);
    }
    
    last_turn[0] = selected_index;
    last_turn[1] = new_index;

    move_piece(blacks_turn, selected_piece, selected_index, new_index);

    if(move_is_castling(selected_piece, selected_index, new_index)){
        if(new_index < selected_index){
            move_piece(blacks_turn, ROOK, new_index - 1, new_index + 1);
        }else{
            move_piece(blacks_turn, ROOK, new_index + 1, new_index - 1);
        }
    }

    if(selected_piece == KING){
        castling_rights[blacks_turn][CASTLE_OOO] = false;
        castling_rights[blacks_turn][CASTLE_OO] = false;
    }
    if(selected_piece == ROOK){
        if(selected_index == 0 || selected_index == 56){
            castling_rights[blacks_turn][CASTLE_OOO] = false;
        }
        else if(selected_index == 7 || selected_index == 63){
            castling_rights[blacks_turn][CASTLE_OO] = false;
        }
    }

    if(!check_promotion(new_index)){
        end_turn();
    }

    deselect_piece();
}

void poll_events(){
    mouse_x = GetMouseX();
    mouse_y = GetMouseY();
    mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mouse_right = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    hover_index = position_to_index(mouse_x, mouse_y);

    i32 new_index = hover_index;
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        deselect_piece();
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(new_index != selected_index && selected_piece != NO_PIECE && can_move(new_index)){
            do_move(new_index);
        }else{
            if(in_promotion){
                for(i32 i = 0; i < 4; i++){
                    if(hover_index == promotion_indexes[i]){
                        promote(blacks_turn, promoted_to, promotion_pieces[i]);

                        deselect_piece();

                        in_promotion = false;
                        end_turn();

                        break;
                    }
                }
            }else{
                select_piece(new_index);
            }
        }
    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        if(new_index != selected_index && dragged_piece != NO_PIECE && can_move(new_index)){
            do_move(new_index);
        }
        dragged_piece = NO_PIECE;
    }
    
    if(IsKeyPressed(KEY_SPACE)){
        printf("%d %d %d\n", selected_index, selected_piece, (i32)selected_black);
    }
}

void chess_run(){
    while (!WindowShouldClose()){
        poll_events();

        BeginDrawing();
        
            ClearBackground(RAYWHITE);

            draw_board();
            draw_pieces();
            draw_selected();
            draw_promotion();
            

        EndDrawing();
    }

    chess_clean();
}