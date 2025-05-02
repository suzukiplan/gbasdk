#pragma once
#include <stdint.h>

extern uint8_t vgs0_rand8_seed;
extern uint16_t vgs0_rand16_seed;
extern const signed char vgs0_sin_table[256];
extern const signed char vgs0_cos_table[256];
extern const unsigned char vgs0_atan2_table[256][256];
extern const unsigned char vgs0_rand8_table[256];
extern const unsigned short vgs0_rand16_table[65536];

inline void vgs0_srand8(uint8_t seed) { vgs0_rand8_seed = seed; }
inline uint8_t vgs0_rand8(void) { return vgs0_rand8_table[vgs0_rand8_seed++]; }
inline void vgs0_srand16(uint16_t seed) { vgs0_rand16_seed = seed; }
inline uint16_t vgs0_rand16_get(void) { return vgs0_rand16_table[vgs0_rand16_seed++]; }
inline int8_t vgs0_sin(uint8_t angle) { return vgs0_sin_table[angle]; }
inline int8_t vgs0_cos(uint8_t angle) { return vgs0_cos_table[angle]; }

// (x1,y1) と (x2,y2) 間の角度 (angle) を求める
inline uint8_t vgs0_angle(int x1, int y1, int x2, int y2)
{
    // 座標系を 7bits にする
    x1 >>= 1;
    x2 >>= 1;
    y1 >>= 1;
    y2 >>= 1;
    return 255 - ((vgs0_atan2_table[(y1 - y2) & 0xFF][(x1 - x2) & 0xFF] + 128) & 0xFF);
}
