#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include "vdp.h"

uint16_t* _dispcnt = (uint16_t*)0x4000000;
uint16_t* _bgctrl = (uint16_t*)0x4000008;
uint16_t* _scroll = (uint16_t*)0x4000010;
uint16_t* _palette = (uint16_t*)0x5000000;
uint16_t* _pattern = (uint16_t*)0x6000000;
uint16_t* _nametbl[4] = {
    (uint16_t*)0x600E000,
    (uint16_t*)0x600E800,
    (uint16_t*)0x600F000,
    (uint16_t*)0x600F800
};

void vdp_init(void)
{
	irqInit();
	irqEnable(IRQ_VBLANK);
	vdp_wait_vblank();
    // www: window
    // s: sprite enabled
    // bbbb: bg0~3 enabled
    // f: force vblank
    // o:
    // h:
    // mmm: screen mode
    //              wwwsbbbbfoh00mmm
    _dispcnt[0] = 0b0001111100000000;

    // ss: size
    // aaaaa: Screen Base Block 0-31, in units of 2 KBytes
    // P: palette (0: 16x16, 1: 256)
    // M: mosaic
    // cc: Character Base Block  (0-3, in units of 16 KBytes)
    // pp: priority
    //          ss0aaaaaPM00ccpp
    _bgctrl[0] = 0b0001110000000011;
    _bgctrl[1] = 0b0001110100000010;
    _bgctrl[2] = 0b0001111000000001;
    _bgctrl[3] = 0b0001111100000000;
}

void vdp_wait_vblank(void)
{
    VBlankIntrWait();
}

void vdp_force_vblank(int on)
{
    uint16_t dispcnt = _dispcnt[0];
    if (on) {
        dispcnt |= 0b0000000010000000;
    } else {
        dispcnt &= 0b1111111101111111;
    }
}

void vdp_print_bg(int no, int x, int y, const char* text)
{
    while (*text) {
        vdp_put_bg(no, x++, y, 0, *text);
        text++;
    }
}

void vdp_palette_init(const void* data)
{
    memcpy(_palette, data, 0x400);
}

void vdp_set_tile(int index, const void* data, size_t size)
{
    index &= 0x3FF;
    memcpy(&_pattern[index * 0x10], data, size);
}

void vdp_copy_tile(int to, int from)
{
    uint8_t buf[0x20];    
    memcpy(buf, &_pattern[from * 0x10], 0x20);
    memcpy(&_pattern[to * 0x10], buf, 0x20);
}

void vdp_interlace_h(int index, int num)
{
    uint16_t* ptn = &_pattern[index * 0x10];
    for (int i = 0; i < num; i++, ptn += 0x10) {
        for (int j = 2; j < 16; j += 4) {
            memset(&ptn[j], 0, 4);
        }
    }
}

void vdp_interlace_v(int index, int num)
{
    uint16_t* ptn = &_pattern[index * 0x10];
    for (int i = 0; i < num; i++, ptn += 0x10) {
        for (int j = 0; j < 16; j++) {
            ptn[j] &= 0x0F0F;
        }
    }
}

void vdp_cls()
{
    for (int i = 0; i < 4; i++) {
        vdp_cls_bg(i);
        vdp_scroll(i, 0, 0);
    }
}
