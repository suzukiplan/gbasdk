#pragma once
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t* _dispcnt;
extern uint16_t* _scroll;
extern uint16_t* _palette;
extern uint16_t* _pattern;
extern uint16_t* _map;
extern uint16_t* _nametbl[4];

void vdp_init(void);
void vdp_wait_vblank(void);
void vdp_force_vblank(int on);
void vdp_palette_init(const void* data);
void vdp_set_tile(int index, const void* data, size_t size);
void vdp_copy_tile(int to, int from);
void vdp_interlace_h(int index, int num);
void vdp_interlace_v(int index, int num);
void vdp_print_bg(int no, int x, int y, const char* text);
void vdp_cls();

inline void vdp_scroll(int no, uint16_t x, uint16_t y)
{
    no &= 0x03;
    no <<= 1;
    _scroll[no] = x & 0x1FF;
    _scroll[no + 1] = y & 0x1FF;
}

inline void vdp_scroll_x(int no, uint16_t x)
{
    no &= 0x03;
    no <<= 1;
    _scroll[no] = x & 0x1FF;
}

inline void vdp_scroll_y(int no, uint16_t y)
{
    no &= 0x03;
    no <<= 1;
    _scroll[no | 1] = y & 0x1FF;
}

inline void vdp_put_bg(int no, int x, int y, uint16_t pal, uint16_t ptn)
{
    _nametbl[no & 0x03][(y & 0x1F) * 0x20 + (x & 0x1F)] = ((pal & 0xF) << 12) | ptn;
}

inline void vdp_cls_bg(int no)
{
    memset(_nametbl[no & 0x03], 0, 0x800);
}

inline void vdp_fill_bg(int no, uint16_t pal, uint16_t ptn)
{
    uint16_t attr = ((pal & 0xF) << 12) | ptn;
    no &= 0x03;
    for (int i = 0; i < 0x400; i++) {
        _nametbl[no][i] = attr;
    }
}

#ifdef __cplusplus
};
#endif
