
// gcc gen_magics.c -o gen_magics -O1 -std=c99 -I ./include/

#include "common.h"
#include "xorshift.h"


#define EDGES  0xff818181818181ffULL


u64 bishop_magics[64];
u64 rook_magics[64];
u32 bishop_shifts[64];
u32 rook_shifts[64];

u64 bishop_table[64][512] = { 0 };
u64 rook_table[64][4096] = { 0 };


#define bit_count(val) __builtin_popcountll(val)


u64 next_subset(u64 subset, u64 set){
    return (subset - set) & set;
}

#define min(a, b) (a) > (b) ? (a) : (b);
#define max(a, b) (a) < (b) ? (a) : (b);

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
        u64 iters = get_dist_to_edge(index, directions[i]);
        for(i32 j = 0; i < iters; i++){
            u64 val = 1ull << index;
            if(val & mask != 0){
                break;
            }
            index += directions[i];
            attack |= val;
        }
    }
}

u64 gen_slider_mask(i32 index, i32 directions[4]){
    return gen_attack(index, directions, EDGES) ^ (1ull << index);
}

bool try_magic(u64 mask, u64 magic, u32 shift, i32 index, u64* table, i32 directions[4]){
    u64 sub = 0;
    u64 hash;

    u64 attacks;

    do{
        attacks = gen_attack(index, directions, sub);

        hash = (sub * magic) >> shift;

        if(table[hash] == 0){
            table[hash] = attacks;
        }else if(table[hash] != attacks){
            return false;
        }

    } while(sub = next_subset(sub, mask));
    
    return true;
}

xorshift64_state xs;
void gen_magic(i32 index, i32 directions[4], u64* table, u64* magic_out, u32* shift_out, bool bishop){
    u64 magic;

    u64 mask = gen_slider_mask(index, directions);
    u64 shift = 64 - bit_count(mask);

    printf("%d\n", shift);
    assert(bishop ? shift >= 55 : shift >= 52);

    do{
        magic = xorshift64(&xs) & xorshift64(&xs) & xorshift64(&xs);
    } while(!try_magic(mask, magic, shift, index, table, directions));

    *shift_out = shift;
    *magic_out = magic;
}

void gen_magics(){
    xs = (xorshift64_state){ time(0) };

    i32 bishop_directions[4] = { 7, 9, -9, -7 };
    i32 rook_directions[4] = { 1, 8, -8, -1 };
    
    for(i32 i = 0; i < 64; i++){
        gen_magic(i, bishop_directions, bishop_table[i], bishop_magics, bishop_shifts, true);
        gen_magic(i, rook_directions,   rook_table[i],   rook_magics,   rook_shifts,   false);
    }
    
}

#define print_table(table) \
    do{ \
        printf("%s\n{\n", #table); \
        for(i32 i = 0; i < 64; i++){ \
            printf("    0x%016llXULL,\n", table[i]); \
        } \
        printf("};\n"); \
    }while(0);

int main(){

    gen_magics();

    print_table(bishop_magics);
    print_table(rook_magics);


    print_table(bishop_shifts);
    print_table(rook_shifts);

    return 0;
}