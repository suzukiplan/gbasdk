/**
 * GBA VGM API
 * 
 * This implementation was created with reference to the following：
 * 
 * Base Implementation
 * https://akkera102.sakura.ne.jp/gbadev/?tutorial.28
 * 
 * VGM Specification
 * https://vgmrips.net/wiki/VGM_Specification#Header
 * 
 * DMG Register Specification
 * https://dic.nicovideo.jp/a/gb%E9%9F%B3%E6%BA%90
 * 
 * GBA Register Specification
 * https://problemkaputt.de/gbatek.htm#gbasoundcontroller
 */
#include <gba_sound.h>
#include <string.h>
#include <stdint.h>
#include "vgm.h"

// support commands
#define VGM_CMD_WAIT        0x61	// wait variable samples (0-65535)
#define VGM_CMD_WAIT_NTSC   0x62    // wait 735 samples (1frame in NTSC)
#define VGM_CMD_WAIT_PAL    0x63    // wait 882 samples (1frame in PAL)
#define VGM_CMD_EOS         0x66    // end of sound data
#define VGM_CMD_WREG        0xB3    // write register

static struct VGMContext {
    const u8* data;
    const u8* cursor;
    u32 loop;
} _vgm;

static u8* _vgm_regs[0x30] = {
                        // Name DMG-A  Description
    (u8*)0x4000060,     // NR10 0xFF10 矩形波１ 制御レジスタ（スイープ）
    (u8*)0x4000062,     // NR11 0xFF11 矩形波１ 制御レジスタ（音色・長さ）
    (u8*)0x4000063,     // NR12 0xFF12 矩形波１ 制御レジスタ（音量）
    (u8*)0x4000064,     // NR13 0xFF13 矩形波１ 制御レジスタ（周波数下位）
    (u8*)0x4000065,     // NR14 0xFF14 矩形波１ 制御レジスタ（周波数上位・長さ・キーオン）
    (u8*)NULL,          // NR20 0xFF15 unused
    (u8*)0x4000068,     // NR21 0xFF16 矩形波２ 制御レジスタ（音色・長さ）
    (u8*)0x4000069,     // NR22 0xFF17 矩形波２ 制御レジスタ（音量）
    (u8*)0x400006C,     // NR23 0xFF18 矩形波２ 制御レジスタ（周波数下位）
    (u8*)0x400006D,     // NR24 0xFF19 矩形波２ 制御レジスタ（周波数上位・長さ・キーオン）
    (u8*)0x4000070,     // NR30 0xFF1A 波形メモリ 制御レジスタ（マスター）
    (u8*)0x4000072,     // NR31 0xFF1B 波形メモリ 制御レジスタ（長さ）
    (u8*)0x4000073,     // NR32 0xFF1C 波形メモリ 制御レジスタ（音量）
    (u8*)0x4000074,     // NR33 0xFF1D 波形メモリ 制御レジスタ（周波数下位）
    (u8*)0x4000075,     // NR34 0xFF1E 波形メモリ 制御レジスタ（周波数上位・長さ・キーオン）
    (u8*)NULL,          // NR40 0xFF1F unused
    (u8*)0x4000078,     // NR41 0xFF20 ノイズ 制御レジスタ（長さ）
    (u8*)0x4000079,     // NR42 0xFF21 ノイズ 制御レジスタ（音量）
    (u8*)0x400007C,     // NR43 0xFF22 ノイズ 制御レジスタ（周波数）
    (u8*)0x400007D,     // NR44 0xFF23 ノイズ 制御レジスタ（長さ・キーオン）
    (u8*)0x4000080,     // NR50 0xFF24 全体音量制御レジスタ
    (u8*)0x4000081,     // NR51 0xFF25 各チャンネル出力制御レジスタ
    (u8*)0x4000084,     // NR52 0xFF26 各チャンネルステータス/サウンド有効フラグレジスタ
    (u8*)NULL,          //      0xFF27 unused
    (u8*)NULL,          //      0xFF28 unused
    (u8*)NULL,          //      0xFF29 unused
    (u8*)NULL,          //      0xFF2A unused
    (u8*)NULL,          //      0xFF2B unused
    (u8*)NULL,          //      0xFF2C unused
    (u8*)NULL,          //      0xFF2D unused
    (u8*)NULL,          //      0xFF2E unused
    (u8*)NULL,          //      0xFF2F unused
    (u8*)0x4000090,     //      0xFF30 波形メモリデータ
    (u8*)0x4000091,     //      0xFF31 波形メモリデータ
    (u8*)0x4000092,     //      0xFF32 波形メモリデータ
    (u8*)0x4000093,     //      0xFF33 波形メモリデータ
    (u8*)0x4000094,     //      0xFF34 波形メモリデータ
    (u8*)0x4000095,     //      0xFF35 波形メモリデータ
    (u8*)0x4000096,     //      0xFF36 波形メモリデータ
    (u8*)0x4000097,     //      0xFF37 波形メモリデータ
    (u8*)0x4000098,     //      0xFF38 波形メモリデータ
    (u8*)0x4000099,     //      0xFF39 波形メモリデータ
    (u8*)0x400009A,     //      0xFF3A 波形メモリデータ
    (u8*)0x400009B,     //      0xFF3B 波形メモリデータ
    (u8*)0x400009C,     //      0xFF3C 波形メモリデータ
    (u8*)0x400009D,     //      0xFF3D 波形メモリデータ
    (u8*)0x400009E,     //      0xFF3E 波形メモリデータ
    (u8*)0x400009F      //      0xFF3F 波形メモリデータ
};

// Initialize
void vgm_init(void)
{
    memset(&_vgm, 0, sizeof(_vgm));
    vgm_stop();
}

// Play
void vgm_play(const void* data)
{
    // stop current music
    vgm_init();

    // set data header
    _vgm.data = (const u8*)data;

    // set loop flag
    uint32_t loop;
    memcpy(&loop, &_vgm.data[0x1C], 4);
    _vgm.loop = loop;

    // get data offset and set cursor
    uint32_t start;
    memcpy(&start, &_vgm.data[0x34], 4);
    start += 0x40;
    _vgm.cursor = _vgm.data + start;
}

void vgm_stop(void)
{
    *(u8*)(REG_BASE + 0x84) = 0x00;
    for (int i = 0; i < 0x30; i++) {
        if (_vgm_regs[i]) {
            *_vgm_regs[i] = 0x00;
        }
    }
    *(u8*)(REG_BASE + 0x74) = 0x80;
    *(u8*)(REG_BASE + 0x80) = 0x77;
    *(u8*)(REG_BASE + 0x81) = 0xff;
    *(u8*)(REG_BASE + 0x84) = 0x80;
}

// frame procedure
void vgm_frame(void)
{
    if (!_vgm.data) {
        return;
    }
    while (1) {
        // wreg command for gameboy (most frequency)
        u8 cmd = *_vgm.cursor++;
        while (cmd == VGM_CMD_WREG) {
            u8* addr = _vgm_regs[*_vgm.cursor++];
            *addr = *_vgm.cursor++;
            cmd = *_vgm.cursor++;
        }

        if (cmd == VGM_CMD_EOS) {
            // end of sound data
            if (_vgm.loop) {
                _vgm.cursor = _vgm.data + _vgm.loop;
            } else {
                vgm_stop();
                return;
            }
        } else {
            // wait command (expect VGM_CMD_WAIT_NTSC or VGM_CMD_WAIT_PAL)
            return;
        }
    }
}
