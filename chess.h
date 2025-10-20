
#include "common.h"

#include "raylib.h"


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
} piece_e;
typedef i32 piece_t;

// #define PAWN   0
// #define KNIGHT 1
// #define BISHOP 2
// #define ROOK   3
// #define QUEEN  4
// #define KING   5

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


#define PWHITE 0
#define PBLACK 1

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
#define FILE_C 0x0404040404040404
#define FILE_D 0x0808080808080808
#define FILE_E 0x1010101010101010
#define FILE_F 0x2020202020202020
#define FILE_G 0x4040404040404040
#define FILE_H 0x8080808080808080

#define FILE_AB 0x0303030303030303
#define FILE_GH 0xC0C0C0C0C0C0C0C0


#define PUT_BIT(value, mask)      ((value) |=  (mask))
#define TOGGLE_BIT(value, mask)   ((value) ^=  (mask))
#define CLEAR_BIT(value, mask)    ((value) &= ~(mask))
#define GET_BIT(value, mask)      ((value) &   (mask))

#define PUT_BITI(value, index)    PUT_BIT((value),    1ull << (index))
#define TOGGLE_BITI(value, index) TOGGLE_BIT((value), 1ull << (index))
#define CLEAR_BITI(value, index)  CLEAR_BIT((value),  1ull << (index))
#define GET_BITI(value, index)    GET_BIT((value),    1ull << (index))

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

bool can_castle = true;

bool blacks_turn = false;

i32 selected_index = -1;
piece_t selected_piece = -1;
piece_t dragged_piece = -1;
bool selected_black = false;
u64 selected_turns = 0;


force_inline u64 full_mask8(u64 arr[]){
    return arr[0] | arr[1] | arr[2] | arr[3] | arr[4] | arr[5] | arr[6] | arr[7];
}
force_inline u64 full_mask6(u64 arr[]){
    return arr[0] | arr[1] | arr[2] | arr[3] | arr[4] | arr[5];
}
force_inline u64 get_attacks(bool black) {
    return full_mask8(directions[black]);
}
force_inline u64 get_pieces(bool black) {
    return full_mask6(pieces[black]);
}
force_inline u64 get_all_pieces() {
    return get_pieces(0) | get_pieces(1);
}

#define PIECE_CHECK(pcs, mask, piece) (!!(pcs[(piece)] & (mask)))
#define PIECE_HERE(pcs, mask, piece) (PIECE_CHECK(pcs, mask, piece) * (piece))
#define __PIECE_HERE(pcs, mask, piece) (PIECE_CHECK(pcs, mask, piece) * (piece + 1))
force_inline piece_t find_piece(bool black, i32 index){
    u64 mask = 1ull << index;
    u64* pcs = pieces[black];
    // +1 so it will be -1 if no pieces are found
    i32 val = __PIECE_HERE(pcs, mask, PAWN) + __PIECE_HERE(pcs, mask, KNIGHT) + __PIECE_HERE(pcs, mask, BISHOP) 
            + __PIECE_HERE(pcs, mask, ROOK) + __PIECE_HERE(pcs, mask, QUEEN)  + __PIECE_HERE(pcs, mask, KING);
    return val - 1;
} 
force_inline piece_t find_piece_all(i32 index, bool* black){
    piece_t piece = find_piece(PWHITE, index);
    if(piece != -1){
        *black = PWHITE;
        return piece;
    }
    *black = PBLACK;
    return find_piece(PBLACK, index);
}
force_inline void put_piece(bool black, piece_t piece, i32 index){
    PUT_BITI(pieces[black][piece], index);
}
force_inline void move_piece(bool black, piece_t piece, i32 from, i32 to){
    CLEAR_BITI(pieces[black][piece], from);
    PUT_BITI(pieces[black][piece], to);
    for(i32 i = 0; i <= KING; i++){
        CLEAR_BITI(pieces[!black][i], to);
    }
}
force_inline void move_piece_unknown(bool black, i32 from, i32 to){
    piece_t piece = find_piece(black, from);
    assert(piece != -1);
    move_piece(black, piece, from, to);
}

force_inline void put_piece_m(bool black, piece_t piece, u64 mask){
    PUT_BIT(pieces[black][piece], mask);
}
force_inline void move_piece_m(bool black, piece_t piece, u64 m_from, u64 m_to){
    CLEAR_BIT(pieces[black][piece], m_from);
    PUT_BIT(pieces[black][piece], m_to);
}

force_inline u64 get_turns_m(i32 index, bool black, piece_t piece){
    u64 mask = 0;
    u64 color_pieces = get_pieces(black);
    u64 other_pieces = get_pieces(!black);
    u64 all_pieces = color_pieces | other_pieces;
    u64 empties = ~all_pieces;
    u64 bit = 1ull << index;
    switch (piece){
    case PAWN: //todo: enpassant
        mask =  (((bit << 7 & ~FILE_H) | (bit << 9 & ~FILE_A)) &  (black - 1llu)                     //white side attacks
             |   ((bit >> 7 & ~FILE_A) | (bit >> 9 & ~FILE_H)) & ~(black - 1llu)) & other_pieces     //black side attacks
             | (((bit << 8) | ((bit & RANK_2) << 16) & (empties << 8)) &  (black - 1llu)             //white side steps
             |  ((bit >> 8) | ((bit & RANK_7) >> 16) & (empties >> 8)) & ~(black - 1llu)) & empties; //black side steps
        break;
    case KNIGHT:
        mask = ((bit << 6 & ~FILE_GH) | (bit << 10 & ~FILE_AB) | (bit << 15 & ~FILE_H) | (bit << 17 & ~FILE_A)
             |  (bit >> 6 & ~FILE_AB) | (bit >> 10 & ~FILE_GH) | (bit >> 15 & ~FILE_A) | (bit >> 17 & ~FILE_H)) & ~color_pieces;
        break;
    case BISHOP:
        
        break;
    case ROOK:

        break;
    case QUEEN:

        break;
    case KING: //todo: castling
        mask = ((bit << 1 & ~FILE_A) | (bit << 7 & ~FILE_H) | (bit << 8) | (bit << 9 & ~FILE_A)
             |  (bit >> 1 & ~FILE_H) | (bit >> 7 & ~FILE_A) | (bit >> 8) | (bit >> 9 & ~FILE_H)) & ~color_pieces;
        break;
    }
    return mask;
}
force_inline i32 pop_lsb(u64* val){
    i32 i = LSB_ZEROS(*val);
    *val &= *val - 1;
    return i;
}
force_inline i32 extract_piece(u64* piece_mask){
    return pop_lsb(piece_mask);
}
i64 evaluate_values() {
    i64 value = 0;
    u64 piece_mask;
    i32 piece_index;
    for(i32 i = 0; i <= KING; i++){
        piece_mask = directions[PWHITE][i];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value += piece_values[i];
        }
        piece_mask = directions[PBLACK][i];
        while(piece_mask){
            piece_index = extract_piece(&piece_mask);
            value -= piece_values[i];
        }
    }
    return value;
}


void clear_board(){
    memset(directions, 0, sizeof(directions));
    memset(pieces, 0, sizeof(pieces));
}
void setup_board(){
    clear_board();
    for(i32 i = 0; i < 8; i++){
        put_piece(PWHITE, PAWN, 8 + i);
    }
    put_piece(PWHITE, ROOK, 0);
    put_piece(PWHITE, ROOK, 7);
    put_piece(PWHITE, KNIGHT, 1);
    put_piece(PWHITE, KNIGHT, 6);
    put_piece(PWHITE, BISHOP, 2);
    put_piece(PWHITE, BISHOP, 5);
    put_piece(PWHITE, QUEEN, 3);
    put_piece(PWHITE, KING, 4);

    for(i32 i = 0; i < 8; i++){
        put_piece(PBLACK, PAWN, 48 + i);
    }
    put_piece(PBLACK, ROOK, 56);
    put_piece(PBLACK, ROOK, 63);
    put_piece(PBLACK, KNIGHT, 57);
    put_piece(PBLACK, KNIGHT, 62);
    put_piece(PBLACK, BISHOP, 58);
    put_piece(PBLACK, BISHOP, 61);
    put_piece(PBLACK, QUEEN, 59);
    put_piece(PBLACK, KING, 60);

    for(i32 i = 0; i <= 1; i++){
        for(i32 j = 0; j <= KING; j++){
            printf("%d\n", pieces[i][j]);
        }
    }

    put_piece(PWHITE, KNIGHT, 36);
    put_piece(PWHITE, PAWN, 20);
    put_piece(PBLACK, PAWN, 21);

    printf("\n\n\n\n\n");
}


i32 mouse_x = 0;
i32 mouse_y = 0;
bool mouse_left = false;
bool mouse_right = false;

// 128x128
Texture2D piece_textures[2][6];

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
#define SELECTED_BLANK (Color){0x71, 0x81, 0x68, 0x00}
#define SELECTED_COLOR (Color){0x71, 0x81, 0x68, 0xFF}

Texture2D load_texture(char* file_path){
    Texture2D texture = LoadTexture(file_path);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    return texture;
}

void load_assets(){
    piece_textures[PWHITE][PAWN]   = load_texture("resources/white-pawn.png");
    piece_textures[PBLACK][PAWN]   = load_texture("resources/black-pawn.png");
    piece_textures[PWHITE][KNIGHT] = load_texture("resources/white-knight.png");
    piece_textures[PBLACK][KNIGHT] = load_texture("resources/black-knight.png");
    piece_textures[PWHITE][BISHOP] = load_texture("resources/white-bishop.png");
    piece_textures[PBLACK][BISHOP] = load_texture("resources/black-bishop.png");
    piece_textures[PWHITE][ROOK]   = load_texture("resources/white-rook.png");
    piece_textures[PBLACK][ROOK]   = load_texture("resources/black-rook.png");
    piece_textures[PWHITE][QUEEN]  = load_texture("resources/white-queen.png");
    piece_textures[PBLACK][QUEEN]  = load_texture("resources/black-queen.png");
    piece_textures[PWHITE][KING]   = load_texture("resources/white-king.png");
    piece_textures[PBLACK][KING]   = load_texture("resources/black-king.png");

    capture_sound = LoadSound("resources/capture.mp3");
    move_sound = LoadSound("resources/move-self.mp3");
    notify_sound = LoadSound("resources/notify.mp3");
}

void index_to_position(i32 index, i32* x, i32* y){
    *x = (index % 8) * SQUARE_SIDE;
    *y = SCREEN_HEIGHT - SQUARE_SIDE - ((index / 8) * SQUARE_SIDE);
}
i32 position_to_index(i32 x, i32 y){
    i32 index = x / SQUARE_SIDE + ((SCREEN_HEIGHT - y) / SQUARE_SIDE) * 8;
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
    DrawCircleGradient(x, y, 15, SELECTED_BLANK, SELECTED_COLOR);
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
    for(i32 i = 0; i <= 1; i++){
        for(i32 j = 0; j <= KING; j++){
            draw_pieces_mask(pieces[i][j], i, j);
        }
    }
}
void draw_selected(){
    if(selected_index != -1){
        draw_square_by_index(selected_index, SELECTED_TINT);
        draw_attacks(selected_turns);
    }
    i32 hover_index;
    if(dragged_piece != -1){
        draw_piece(mouse_x - HALF_SQUARE_SIDE, mouse_y - HALF_SQUARE_SIDE, selected_black, dragged_piece);
        hover_index = position_to_index(mouse_x, mouse_y);
        if(GET_BITI(selected_turns, hover_index)){
            draw_square_by_index(hover_index, SELECTED_HOVER);
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
        DrawText(text_letter, 80 + i * SQUARE_SIDE, SCREEN_HEIGHT - 10 - font_size                    , font_size, color);
        DrawText(text_number, 10                  , SCREEN_HEIGHT - (70 + i * SQUARE_SIDE) - font_size, font_size, color);
        text_letter[0]++;
        text_number[0]++;
    }
}

void chess_init(){

    setup_board();

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");

    InitAudioDevice();

    SetTargetFPS(60);

    load_assets();
}

void chess_clean(){
    for(i32 i = 0; i <= 1; i++){
        for(i32 j = 0; j <= KING; j++){
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
    return GET_BITI(selected_turns, new_index) && selected_black == blacks_turn;
}

void do_move(i32 new_index){
    if(GET_BITI(get_pieces(!selected_black), new_index)){
        PlaySound(capture_sound);
    }else{
        PlaySound(move_sound);
    }
    move_piece(selected_black, selected_piece, selected_index, new_index);
    selected_index = -1;
    selected_piece = -1;
    blacks_turn = !blacks_turn;
}

void poll_events(){
    mouse_x = GetMouseX();
    mouse_y = GetMouseY();
    mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mouse_right = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    i32 new_index = position_to_index(mouse_x, mouse_y);
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        selected_index = -1;
        selected_piece = -1;
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(new_index != selected_index && selected_piece != -1 && can_move(new_index)){
            do_move(new_index);
        }else{
            selected_index = new_index;
            selected_piece = find_piece_all(selected_index, &selected_black);
            dragged_piece = selected_piece;
            selected_turns = get_turns_m(selected_index, selected_black, selected_piece);
        }
    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        if(new_index != selected_index && dragged_piece != -1 && can_move(new_index)){
            do_move(new_index);
        }
        dragged_piece = -1;    
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
            

        EndDrawing();
    }

    chess_clean();
}