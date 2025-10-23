#pragma once

#include <stdint.h>

#define XORSHIFT32_MAX 0xFFFFFFFFu
#define XORSHIFT32_MAX_INV (1.f / (float)XORSHIFT32_MAX)
#define XORSHIFT32_MIN 0

#define XORSHIFT64_MAX 0xFFFFFFFFFFFFFFFFull
#define XORSHIFT64_MAX_INV (1.0 / (double)XORSHIFT64_MAX)
#define XORSHIFT64_MIN 0


typedef struct {
	uint32_t a;
} xorshift32_state;

uint32_t xorshift32(xorshift32_state* state){
	uint32_t x = state->a;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return state->a = x;
}


typedef struct {
	uint64_t a;
} xorshift64_state;

uint64_t xorshift64(xorshift64_state* state){
	uint64_t x = state->a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return state->a = x;
}


typedef struct {
	uint32_t a[4];
} xorshift32_128_state;

uint32_t xorshift32_128(xorshift32_128_state* state){
	uint32_t t  = state->a[3];

	uint32_t s  = state->a[0];
	state->a[3] = state->a[2];
	state->a[2] = state->a[1];
	state->a[1] = s;

	t ^= t << 11;
	t ^= t >> 8;
	return state->a[0] = t ^ s ^ (s >> 19);
}
