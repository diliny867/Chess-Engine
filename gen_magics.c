
// gcc gen_magics.c -o gen_magics -O1 -std=c99 -I ./include/

#include "common.h"
#include "xorshift.h"


u64 bishop_magics[64];
u64 rook_magics[64];

u32 bishop_shifts[64];
u32 rook_shifts[64];

u64 bishop_masks[64];
u64 rook_masks[64];


#define bit_count(val) __builtin_popcountll(val)

void print_mask(u64 val){
    u8* vals = (u8*)&val;
    for(i32 i = 0; i < 8; i++){
        for(i32 j = 0; j < 8; j++){
            printf("%d", (vals[7 - i] >> j) & 1);
        }
        printf("\n");
    }
}

#define print_tableu64(table) \
    do{ \
        printf("u64 %s[64] = {\n", #table); \
        for(i32 i = 0; i < 64; i++){ \
            printf("    0x%016llXULL%c\n", table[i], i < 63 ? ',' : '\0'); \
        } \
        printf("};\n"); \
    }while(0);

#define print_tableu32(table) \
    do{ \
        printf("u32 %s[64] = {\n", #table); \
        for(i32 i = 0; i < 64; i++){ \
            printf("    %u%c\n", table[i], i < 63 ? ',' : '\0'); \
        } \
        printf("};\n"); \
    }while(0);


u64 next_subset(u64 subset, u64 set){
    return (subset - set) & set;
}

#define min(a, b) (a) < (b) ? (a) : (b);
#define max(a, b) (a) > (b) ? (a) : (b);

i32 get_dist_to_edge(i32 index, i32 direction){
    i32 r = index / 8;
    i32 f = index % 8;
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
}

u64 gen_attack(i32 index, i32 directions[4], u64 mask){
    u64 attack = 0;
    for(i32 i = 0; i < 4; i++){
        u64 direction = directions[i];
        u64 iters = get_dist_to_edge(index, direction);
        u64 shift = index;
        for(i32 j = 0; j < iters; j++){
            u64 val = 1ull << shift;
            if((val & mask) != 0){
                break;
            }
            shift += direction;
            attack |= val;
        }
    }
    return attack;
}

u64 gen_slider_mask(i32 index, i32 directions[4]){
    return gen_attack(index, directions, 0) & ~(1ull << index);
}

bool try_magic(u64 mask, u64 magic, u32 shift, i32 index, u64* table, i32 directions[4]){
    u64 sub = 0;
    u64 hash;
    u64 attack;

    do{
        attack = gen_attack(index, directions, sub); //todo: precalculate this

        hash = (sub * magic) >> shift;

        if(table[hash] == 0){
            table[hash] = attack;
        }else if(table[hash] != attack){
            return false;
        }

    } while(sub = next_subset(sub, mask));
    
    return true;
}

xorshift64_state xs;
void gen_magic(i32 index, i32 directions[4], u64* magics, u32* shifts, u64* masks, bool bishop){
    
    u64 mask = gen_slider_mask(index, directions);
    // print_mask(mask);
    // printf("\n");
    
    u64 shift = 64 - bit_count(mask);
    u64 size = 1 << bit_count(mask);
    
    assert(bishop ? shift >= 55 : shift >= 52);
    
    u64 magic;
    u64* table;
    bool found;
    while(true){
        magic = xorshift64(&xs) & xorshift64(&xs) & xorshift64(&xs);
        
        table = calloc(size, sizeof(u64)); // probably faster to alloc then free than memzero static array
        found = try_magic(mask, magic, shift, index, table, directions);
        free(table);

        if(found){
            break;
        }
    }

    shifts[index] = shift;
    magics[index] = magic;
    masks[index]  = mask;
}

void gen_magics(){
    xs = (xorshift64_state){ time(0) };

    i32 bishop_directions[4] = { 7, 9, -9, -7 };
    i32 rook_directions[4] = { 1, 8, -8, -1 };
    
    for(i32 i = 0; i < 64; i++){
        gen_magic(i, bishop_directions, bishop_magics, bishop_shifts, bishop_masks, true);
    }

    for(i32 i = 0; i < 64; i++){
        gen_magic(i, rook_directions, rook_magics, rook_shifts, rook_masks, false);
    }
}

int main(){

    gen_magics();

    print_tableu64(bishop_magics);
    print_tableu64(rook_magics);

    print_tableu32(bishop_shifts);
    print_tableu32(rook_shifts);

    print_tableu64(bishop_masks);
    print_tableu64(rook_masks);

    return 0;
}