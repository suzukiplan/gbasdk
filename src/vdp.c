#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include "vdp.h"
#include "sfx.h"
#include "psg.h"

typedef struct
{
    uint16_t dummy1[3];
    uint16_t pa;
    uint16_t dummy2[3];
    uint16_t pb;
    uint16_t dummy3[3];
    uint16_t pc;
    uint16_t dummy4[3];
    uint16_t pd;
} OAM_AFFINE;

const uint16_t OAM_ATTR1_TRANSFORM_SCALE[4] = {
    OAM_ATTR1_TRANSFORM_SCALE_05,
    OAM_ATTR1_TRANSFORM_SCALE_10,
    OAM_ATTR1_TRANSFORM_SCALE_15,
    OAM_ATTR1_TRANSFORM_SCALE_20};

const uint16_t OAM_ATTR1_TRANSFORM_ROTATE[16 + 4] = {
    0 << 9,  // 0x00: 0
    1 << 9,  // 0x01: 23
    2 << 9,  // 0x02: 45
    3 << 9,  // 0x03: 68
    4 << 9,  // 0x04: 90
    5 << 9,  // 0x05: 113
    6 << 9,  // 0x06: 135
    7 << 9,  // 0x07: 158
    8 << 9,  // 0x08: 180
    9 << 9,  // 0x09: 203
    10 << 9, // 0x0A: 225
    11 << 9, // 0x0B: 248
    12 << 9, // 0x0C: 270
    13 << 9, // 0x0D: 293
    14 << 9, // 0x0E: 315
    15 << 9, // 0x0F: 338
    16 << 9, // 0x10: 15
    17 << 9, // 0x11: 30
    18 << 9, // 0x12: 330
    19 << 9, // 0x13: 345
};

const uint16_t OAM_ATTR1_SIZE[4] = {
    0 << 14,
    1 << 14,
    2 << 14,
    3 << 14};

const uint16_t OAM_ATTR2_PRIORITY[4] = {
    0 << 10,
    1 << 10,
    2 << 10,
    3 << 10};

const uint16_t OAM_ATTR2_PALETTE[16] = {
    0 << 12,
    1 << 12,
    2 << 12,
    3 << 12,
    4 << 12,
    5 << 12,
    6 << 12,
    7 << 12,
    8 << 12,
    9 << 12,
    10 << 12,
    11 << 12,
    12 << 12,
    13 << 12,
    14 << 12,
    15 << 12};

static const int16_t _deg2sin[360] = {
    0,
    4,
    8,
    13,
    17,
    22,
    26,
    31,
    35,
    40,
    44,
    48,
    53,
    57,
    61,
    66,
    70,
    74,
    79,
    83,
    87,
    91,
    95,
    100,
    104,
    108,
    112,
    116,
    120,
    124,
    128,
    131,
    135,
    139,
    143,
    146,
    150,
    154,
    157,
    161,
    164,
    167,
    171,
    174,
    177,
    181,
    184,
    187,
    190,
    193,
    196,
    198,
    201,
    204,
    207,
    209,
    212,
    214,
    217,
    219,
    221,
    223,
    226,
    228,
    230,
    232,
    233,
    235,
    237,
    238,
    240,
    242,
    243,
    244,
    246,
    247,
    248,
    249,
    250,
    251,
    252,
    252,
    253,
    254,
    254,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    254,
    254,
    253,
    252,
    252,
    251,
    250,
    249,
    248,
    247,
    246,
    244,
    243,
    242,
    240,
    238,
    237,
    235,
    233,
    232,
    230,
    228,
    226,
    223,
    221,
    219,
    217,
    214,
    212,
    209,
    207,
    204,
    201,
    198,
    196,
    193,
    190,
    187,
    184,
    181,
    177,
    174,
    171,
    167,
    164,
    161,
    157,
    154,
    150,
    146,
    143,
    139,
    135,
    131,
    127,
    124,
    120,
    116,
    112,
    108,
    104,
    100,
    95,
    91,
    87,
    83,
    79,
    74,
    70,
    66,
    61,
    57,
    53,
    48,
    44,
    40,
    35,
    31,
    26,
    22,
    17,
    13,
    8,
    4,
    0,
    -4,
    -8,
    -13,
    -17,
    -22,
    -26,
    -31,
    -35,
    -40,
    -44,
    -48,
    -53,
    -57,
    -61,
    -66,
    -70,
    -74,
    -79,
    -83,
    -87,
    -91,
    -95,
    -100,
    -104,
    -108,
    -112,
    -116,
    -120,
    -124,
    -128,
    -131,
    -135,
    -139,
    -143,
    -146,
    -150,
    -154,
    -157,
    -161,
    -164,
    -167,
    -171,
    -174,
    -177,
    -181,
    -184,
    -187,
    -190,
    -193,
    -196,
    -198,
    -201,
    -204,
    -207,
    -209,
    -212,
    -214,
    -217,
    -219,
    -221,
    -223,
    -226,
    -228,
    -230,
    -232,
    -233,
    -235,
    -237,
    -238,
    -240,
    -242,
    -243,
    -244,
    -246,
    -247,
    -248,
    -249,
    -250,
    -251,
    -252,
    -252,
    -253,
    -254,
    -254,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -254,
    -254,
    -253,
    -252,
    -252,
    -251,
    -250,
    -249,
    -248,
    -247,
    -246,
    -244,
    -243,
    -242,
    -240,
    -238,
    -237,
    -235,
    -233,
    -232,
    -230,
    -228,
    -226,
    -223,
    -221,
    -219,
    -217,
    -214,
    -212,
    -209,
    -207,
    -204,
    -201,
    -198,
    -196,
    -193,
    -190,
    -187,
    -184,
    -181,
    -177,
    -174,
    -171,
    -167,
    -164,
    -161,
    -157,
    -154,
    -150,
    -146,
    -143,
    -139,
    -135,
    -131,
    -127,
    -124,
    -120,
    -116,
    -112,
    -108,
    -104,
    -100,
    -95,
    -91,
    -87,
    -83,
    -79,
    -74,
    -70,
    -66,
    -61,
    -57,
    -53,
    -48,
    -44,
    -40,
    -35,
    -31,
    -26,
    -22,
    -17,
    -13,
    -8,
    -4,
};

static const short _deg2cos[360] = {
    256,
    255,
    255,
    255,
    255,
    255,
    254,
    254,
    253,
    252,
    252,
    251,
    250,
    249,
    248,
    247,
    246,
    244,
    243,
    242,
    240,
    238,
    237,
    235,
    233,
    232,
    230,
    228,
    226,
    223,
    221,
    219,
    217,
    214,
    212,
    209,
    207,
    204,
    201,
    198,
    196,
    193,
    190,
    187,
    184,
    181,
    177,
    174,
    171,
    167,
    164,
    161,
    157,
    154,
    150,
    146,
    143,
    139,
    135,
    131,
    127,
    124,
    120,
    116,
    112,
    108,
    104,
    100,
    95,
    91,
    87,
    83,
    79,
    74,
    70,
    66,
    61,
    57,
    53,
    48,
    44,
    40,
    35,
    31,
    26,
    22,
    17,
    13,
    8,
    4,
    0,
    -4,
    -8,
    -13,
    -17,
    -22,
    -26,
    -31,
    -35,
    -40,
    -44,
    -48,
    -53,
    -57,
    -61,
    -66,
    -70,
    -74,
    -79,
    -83,
    -87,
    -91,
    -95,
    -100,
    -104,
    -108,
    -112,
    -116,
    -120,
    -124,
    -128,
    -131,
    -135,
    -139,
    -143,
    -146,
    -150,
    -154,
    -157,
    -161,
    -164,
    -167,
    -171,
    -174,
    -177,
    -181,
    -184,
    -187,
    -190,
    -193,
    -196,
    -198,
    -201,
    -204,
    -207,
    -209,
    -212,
    -214,
    -217,
    -219,
    -221,
    -223,
    -226,
    -228,
    -230,
    -232,
    -233,
    -235,
    -237,
    -238,
    -240,
    -242,
    -243,
    -244,
    -246,
    -247,
    -248,
    -249,
    -250,
    -251,
    -252,
    -252,
    -253,
    -254,
    -254,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -255,
    -254,
    -254,
    -253,
    -252,
    -252,
    -251,
    -250,
    -249,
    -248,
    -247,
    -246,
    -244,
    -243,
    -242,
    -240,
    -238,
    -237,
    -235,
    -233,
    -232,
    -230,
    -228,
    -226,
    -223,
    -221,
    -219,
    -217,
    -214,
    -212,
    -209,
    -207,
    -204,
    -201,
    -198,
    -196,
    -193,
    -190,
    -187,
    -184,
    -181,
    -177,
    -174,
    -171,
    -167,
    -164,
    -161,
    -157,
    -154,
    -150,
    -146,
    -143,
    -139,
    -135,
    -131,
    -127,
    -124,
    -120,
    -116,
    -112,
    -108,
    -104,
    -100,
    -95,
    -91,
    -87,
    -83,
    -79,
    -74,
    -70,
    -66,
    -61,
    -57,
    -53,
    -48,
    -44,
    -40,
    -35,
    -31,
    -26,
    -22,
    -17,
    -13,
    -8,
    -4,
    0,
    4,
    8,
    13,
    17,
    22,
    26,
    31,
    35,
    40,
    44,
    48,
    53,
    57,
    61,
    66,
    70,
    74,
    79,
    83,
    87,
    91,
    95,
    100,
    104,
    108,
    112,
    116,
    120,
    124,
    128,
    131,
    135,
    139,
    143,
    146,
    150,
    154,
    157,
    161,
    164,
    167,
    171,
    174,
    177,
    181,
    184,
    187,
    190,
    193,
    196,
    198,
    201,
    204,
    207,
    209,
    212,
    214,
    217,
    219,
    221,
    223,
    226,
    228,
    230,
    232,
    233,
    235,
    237,
    238,
    240,
    242,
    243,
    244,
    246,
    247,
    248,
    249,
    250,
    251,
    252,
    252,
    253,
    254,
    254,
    255,
    255,
    255,
    255,
    255,
};

static OAM _voam[128];

uint16_t* _dispcnt = (uint16_t*)0x4000000;
uint16_t* _bgctrl = (uint16_t*)0x4000008;
uint16_t* _scroll = (uint16_t*)0x4000010;
uint16_t _scroll_buffer[8];
uint16_t* _palette = (uint16_t*)0x5000000;
uint16_t* _pattern_bg = (uint16_t*)0x6000000;
uint16_t* _pattern_sp = (uint16_t*)0x6010000;
uint16_t* _nametbl[4] = {
    (uint16_t*)0x600E000,
    (uint16_t*)0x600E800,
    (uint16_t*)0x600F000,
    (uint16_t*)0x600F800};
OAM* _oam = _voam;
OAM_AFFINE* _oam_affine = (OAM_AFFINE*)_voam;

static void vdp_oam_affine(int index, uint16_t pa, uint16_t pb, uint16_t pc, uint16_t pd)
{
    _oam_affine[index].pa = pa;
    _oam_affine[index].pb = pb;
    _oam_affine[index].pc = pc;
    _oam_affine[index].pd = pd;
}

void vdp_init(void)
{
    memset(_voam, 0, sizeof(_voam));
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
    _dispcnt[0] = 0b0001111101000000;

    // ss: size
    // aaaaa: Screen Base Block 0-31, in units of 2 KBytes
    // P: palette (0: 16x16, 1: 256)
    // M: mosaic
    // cc: Character Base Block  (0-3, in units of 16 KBytes)
    // pp: priority
    //             ss0aaaaaPM00ccpp
    _bgctrl[0] = 0b0001110000000011;
    _bgctrl[1] = 0b0001110100000010;
    _bgctrl[2] = 0b0001111000000001;
    _bgctrl[3] = 0b0001111100000000;

    // アフィン係数は以下の固定値で初期化
    // 0 ~ 15 .... 回転1: 0=0度 ~ 15=338度 (22.5度刻み)
    // 16 ~ 19 ... 回転2: 16=15度, 17=30度, 18=330度, 19=345度
    // 20 ~ 27 ... 未使用 (指定すると白い矩形になるっぽい)
    // 28 ~ 31 ... 拡大縮小: 16=0.5x, 17=1.0x, 18=1.5x, 19=2.0x
    for (int i = 0, deg = 0; i < 16; i++, deg += 22 + (i & 1)) {
        vdp_oam_affine(i, _deg2cos[deg], _deg2sin[deg], -_deg2sin[deg], _deg2cos[deg]);
    }
    vdp_oam_affine(16, _deg2cos[15], _deg2sin[15], -_deg2sin[15], _deg2cos[15]);
    vdp_oam_affine(17, _deg2cos[30], _deg2sin[30], -_deg2sin[30], _deg2cos[30]);
    vdp_oam_affine(18, _deg2cos[330], _deg2sin[330], -_deg2sin[330], _deg2cos[330]);
    vdp_oam_affine(19, _deg2cos[345], _deg2sin[345], -_deg2sin[345], _deg2cos[345]);
    vdp_oam_affine(28, 512, 0, 0, 512); // 0.5x
    vdp_oam_affine(29, 256, 0, 0, 256); // 1.0x
    vdp_oam_affine(30, 192, 0, 0, 192); // 1.5x
    vdp_oam_affine(31, 128, 0, 0, 128); // 2.0x

    // クリア
    vdp_cls();
    vdp_oam_clear();
}

void vdp_wait_vblank(void)
{
    VBlankIntrWait();                               // V-BLANK を待機
    sfx_frame();                                    // 効果音のフレーム処理
    psg_frame();                                    // BGMのフレーム処理
    memcpy((void*)0x7000000, _voam, sizeof(_voam)); // 仮想OAM から OAM へコピー
}

void vdp_force_vblank(int on)
{
    uint16_t dispcnt = _dispcnt[0];
    if (on) {
        dispcnt |= 0b0000000010000000;
    } else {
        dispcnt &= 0b1111111101111111;
    }
    _dispcnt[0] = dispcnt;
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
    memcpy(_palette + 0x200, data, 0x400);
}

void vdp_set_tile(int index, const void* data, size_t size)
{
    index &= 0x3FF;
    memcpy(&_pattern_bg[index * 0x10], data, size);
}

void vdp_set_tile_sp(int index, const void* data, size_t size)
{
    index &= 0x3FF;
    memcpy(&_pattern_sp[index * 0x10], data, size);
}

void vdp_copy_tile(int to, int from)
{
    uint8_t buf[0x20];
    memcpy(buf, &_pattern_bg[from * 0x10], 0x20);
    memcpy(&_pattern_bg[to * 0x10], buf, 0x20);
}

void vdp_interlace_h(int index, int num)
{
    uint16_t* ptn = &_pattern_bg[index * 0x10];
    for (int i = 0; i < num; i++, ptn += 0x10) {
        for (int j = 2; j < 16; j += 4) {
            memset(&ptn[j], 0, 4);
        }
    }
}

void vdp_interlace_v(int index, int num)
{
    uint16_t* ptn = &_pattern_bg[index * 0x10];
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

void vdp_oam_clear(void)
{
    for (int i = 0; i < 128; i++) {
        vdp_oam_hide(&_oam[i]);
    }
}

void vdp_oam_init(
    OAM* oam,      // target OAM
    int transform, // 0: disable, not0: enable
    int shape,     // type of size ... 0: 8x8,16x16,32x32,64x64 | 1: 16x8,32x8,32x16,64x32 | 2: 8x16,8x32,16x32,32x64
    int ptn,       // pattern index number (0-511)
    int size,      // pattern size (0-3)
    int palette    // palette number (0-15)
)
{
    shape &= 0x03;
    ptn &= 0x3FF;
    size &= 0x03;
    palette &= 0x0F;
    oam->attr0 = 192 | (transform ? 256 : 0) | (shape << 14);
    oam->attr1 = 240 | ((size & 0b11) << 14);
    oam->attr2 = ptn | (2 << 10) | (palette << 12);
}
