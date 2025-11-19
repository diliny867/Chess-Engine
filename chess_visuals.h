#pragma once

#include "chess.h"

#include "raylib.h"
#include "raymath.h"


i32 mouse_x = 0;
i32 mouse_y = 0;
bool mouse_left = false;
bool mouse_right = false;
i32 hover_index = NUL_INDEX;

i32 selected_index = NUL_INDEX;
i32 last_turn[2] = { NUL_INDEX, NUL_INDEX };
piece_t selected_piece = NO_PIECE;
piece_t dragged_piece = NO_PIECE;
bool selected_black = false;
u64 selected_turns = 0;

bool in_promotion = false;
i32 promoted_to = NUL_INDEX;
i32 promotion_indexes[4] = { NUL_INDEX, NUL_INDEX, NUL_INDEX, NUL_INDEX };
piece_t promotion_pieces[4] = { QUEEN, KNIGHT, ROOK, BISHOP };

typedef struct {
    i32 from, to;
} arrow_t;
arrow_t arrows[64 * 64] = { 0 };
i32 arrows_count = 0;
i32 selected_arrow = NUL_INDEX;
arrow_t dragged_arrow = { 0, 0 };
bool doing_arrow = false;

bool checkmated = false;

// 128x128
Texture2D piece_textures[CCOLOR_COUNT][PIECE_COUNT];

RenderTexture2D board_texture;

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
//#define ARROW_COLOR    (Color){0x6D, 0x7F, 0x58, 0xAF}
#define ARROW_COLOR    (Color){0xC7, 0xC1, 0x10, 0xAF}

#define MOVE_HIGHLIGHT (Color){0xF7, 0xF1, 0x1D, 0x50}
#define MOVE_HIGHLIGHT2 (Color){0x8B, 0xCE, 0xF7, 0x6F}

#define MATE_HIGHLIGHT (Color){0xE8, 0x11, 0x23, 0x6F}
#define MATE_TINT      (Color){0xFF, 0xFF, 0xFF, 0x2F}
#define WHITE_WON_TEXT (Color){0xEF, 0xEF, 0xEF, 0xFF}
#define BLACK_WON_TEXT (Color){0x10, 0x10, 0x10, 0xFF}

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
void draw_arrow(i32 from, i32 to){
    i32 xf, yf, xt, yt;
    index_to_position(from, &xf, &yf);
    if(from == to){
        DrawRing((Vector2){ xf + HALF_SQUARE_SIDE, yf + HALF_SQUARE_SIDE }, 35, 40, 0, 360, 0, ARROW_COLOR);
    }else {
        index_to_position(to, &xt, &yt);
        f32 size = hypotf(xt - xf, yt - yf) - 7;
        f32 angle = Vector2LineAngle((Vector2){ xf, yf }, (Vector2){ xt, yt }) * RAD2DEG;
        DrawRectanglePro((Rectangle){ xf + HALF_SQUARE_SIDE, yf + HALF_SQUARE_SIDE, size, 16 }, (Vector2){ 8, 8 }, -angle, ARROW_COLOR);
        DrawPoly((Vector2){ xt + HALF_SQUARE_SIDE, yt + HALF_SQUARE_SIDE }, 3, 30, -angle, ARROW_COLOR);
    }
}
void draw_arrows(){
    for(i32 i = 0; i < arrows_count; i++){
        draw_arrow(arrows[i].from, arrows[i].to);
    }
    if(doing_arrow){
        draw_arrow(dragged_arrow.from, dragged_arrow.to);
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
void draw_board_to_texture(){ //draw it to texture then render that texture, because why not
    BeginTextureMode(board_texture);
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
    EndTextureMode();
}
void draw_board() {
    DrawTextureRec(board_texture.texture, (Rectangle){ 0, 0, (f32)board_texture.texture.width, (f32)-board_texture.texture.height }, (Vector2){ 0, 0 }, WHITE);
}

void draw_text_centered(char* text, i32 x, i32 y, i32 font_size, Color color){
    i32 text_width = MeasureText(text, font_size);
    i32 textx = x - text_width / 2;
    i32 texty = y - font_size / 2;
    DrawText(text, textx, texty, font_size, color);
}

void draw_checkmated(){
    i32 king_index = find_piece(blacks_turn, KING);
    draw_square_by_index(king_index, MATE_HIGHLIGHT);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MATE_TINT);
    char text[20];
    sprintf(text, "%s Won", blacks_turn ? "White" : "Black");
    draw_text_centered(text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 120, blacks_turn ? WHITE_WON_TEXT : BLACK_WON_TEXT);
    draw_text_centered("Press ENTER to restart", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 120, 40, blacks_turn ? WHITE_WON_TEXT : BLACK_WON_TEXT);
}

void draw_init(){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");

    InitAudioDevice();

    SetTargetFPS(60);

    load_assets();

    board_texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    draw_board_to_texture();
}

void draw_clean(){
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

bool check_enter_promotion(i32 new_index){
    if(selected_piece == PAWN && ((blacks_turn && new_index <= 7) || (!blacks_turn && new_index >= 56))){
        in_promotion = true;
        promoted_to = new_index;
        for(i32 i = 0; i < 4; i++){
            promotion_indexes[i] = promoted_to + (blacks_turn ? 8 : -8) * i;
        }
        return true;
    }
    return false;
}

void start_arrow(i32 index){
    selected_arrow = index;
    doing_arrow = true;
}
void end_arrow(i32 index){
    // find if arrow exists
    for(i32 i = 0; i < arrows_count; i++){
        if(arrows[i].from == selected_arrow && arrows[i].to == index){
            arrows[i].from = arrows[arrows_count - 1].from;
            arrows[i].to   = arrows[arrows_count - 1].to;
            arrows_count--;
            return;
        }
    }
    arrows[arrows_count].from = selected_arrow;
    arrows[arrows_count].to   = index;
    arrows_count++;
    doing_arrow = false;
}
void clear_arrows(){
    arrows_count = 0;
    doing_arrow = false;
}

void end_turn(){
    blacks_turn = !blacks_turn;
    setup_turn();
    if(is_checkmate(blacks_turn)){
        checkmated = true;
        PlaySound(notify_sound);
        return;
    }
}

void deselect_piece(){
    selected_index = NUL_INDEX;
    selected_piece = NO_PIECE;
    dragged_piece = NO_PIECE;
}
void select_piece(i32 index){
    selected_index = index;
    selected_piece = piece_at_all(selected_index, &selected_black);
    if(selected_black == blacks_turn){
        dragged_piece = selected_piece;
    }
    selected_turns = get_index_legal_turns(selected_black, selected_index, selected_piece);
}

// pretty rudimentary special move checking
bool move_is_double_pawn_move(piece_t piece, i32 from, i32 to){
    if(piece != PAWN){
        return false;
    }
    i32 diff = abs(to - from);
    return diff == 16;
}
bool move_is_enpassant(piece_t piece, i32 from, i32 to){
    if(piece != PAWN){
        return false;
    }
    i32 diff = abs(to - from);
    if(diff != 7 && diff != 9){
        return false;
    }
    return !piece_at(to, PAWN);
}
bool move_is_castling(piece_t piece, i32 from, i32 to){
    if(piece != KING){
        return false;
    }
    i32 diff = abs(to - from);
    return diff == 3 || diff == 2;
}

void do_move(i32 new_index){
    if(GET_BIT(colors[!blacks_turn], new_index)){
        PlaySound(capture_sound);
    }else{
        PlaySound(move_sound);
    }
    
    last_turn[0] = selected_index;
    last_turn[1] = new_index;

    if(move_is_castling(selected_piece, selected_index, new_index)){
        if(new_index < selected_index){
            move_piece(blacks_turn, ROOK, new_index - 1, new_index + 1);
        }else{
            move_piece(blacks_turn, ROOK, new_index + 1, new_index - 1);
        }
    }

    if(selected_piece == KING){
        lost_castling_rights[blacks_turn][CASTLE_OOO] = true;
        lost_castling_rights[blacks_turn][CASTLE_OO] = true;
    }
    if(selected_piece == ROOK){
        if(selected_index == 0 || selected_index == 56){
            lost_castling_rights[blacks_turn][CASTLE_OOO] = true;
        }
        else if(selected_index == 7 || selected_index == 63){
            lost_castling_rights[blacks_turn][CASTLE_OO] = true;
        }
    }

    if(move_is_enpassant(selected_piece, selected_index, new_index)){
        clear_piece(!blacks_turn, PAWN, new_index + 8 * sign(selected_index - new_index));
    }

    enpassantable_mask = 0;
    if(move_is_double_pawn_move(selected_piece, selected_index, new_index)){
        enpassantable_mask |= (1ull << (selected_index + ((new_index - selected_index) >> 1)));
    }

    move_piece(blacks_turn, selected_piece, selected_index, new_index);

    end_turn();

    deselect_piece();
}

void restart_game(){
    checkmated = false;
    blacks_turn = false;
    arrows_count = 0;
    last_turn[0] = NUL_INDEX;
    last_turn[1] = NUL_INDEX;
    setup_board();
    setup_turn();
}

void poll_events(){
    mouse_x = GetMouseX();
    mouse_y = GetMouseY();
    mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mouse_right = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    hover_index = position_to_index(mouse_x, mouse_y);

    if(checkmated){
        if(IsKeyPressed(KEY_ENTER)){
            if(checkmated){
                PlaySound(notify_sound);
                restart_game();
            }   
        }
        return;
    }

    i32 new_index = hover_index;
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        //if(selected_index == NUL_INDEX){
            start_arrow(new_index);
            dragged_arrow.from = new_index;
        //}
        in_promotion = false;
        deselect_piece();
    }
    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
        if(doing_arrow){
            dragged_arrow.to = new_index;
        }
    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
        if(doing_arrow){
            end_arrow(new_index);
        }
        doing_arrow = false;
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        clear_arrows();
        if(in_promotion){
            for(i32 i = 0; i < 4; i++){
                if(hover_index == promotion_indexes[i]){
                    do_move(promoted_to);
                    promote(blacks_turn, promoted_to, promotion_pieces[i]);
                    break;
                }
            }
            in_promotion = false;
            deselect_piece();
        }else if(new_index != selected_index && selected_piece != NO_PIECE && can_move(new_index)){
            if(!check_enter_promotion(new_index)){
                do_move(new_index);
            }
        }else{
            select_piece(new_index);
        }
    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        if(new_index != selected_index && dragged_piece != NO_PIECE && can_move(new_index)){
            if(!check_enter_promotion(new_index)){
                do_move(new_index);
            }
        }
        dragged_piece = NO_PIECE;
    }
    
    // if(IsKeyPressed(KEY_SPACE)){
    //     printf("%d %d %d\n", selected_index, selected_piece, (i32)selected_black);
    // }
}

void chess_run(){
    draw_init();

    while (!WindowShouldClose()){
        poll_events();

        BeginDrawing();
        
            ClearBackground(RAYWHITE);

            draw_board();
            draw_pieces();
            draw_selected();
            draw_arrows();
            draw_promotion();

            if(checkmated){
                draw_checkmated();
            }

        EndDrawing();
    }

    draw_clean();
}