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
#include <string.h>
#include <stdint.h>
#include "psg.h"

static struct Context {
    const uint8_t* data;
    const uint8_t* cursor;
    int fadeout;
} _psg;

// Initialize
void psg_init(void)
{
    psg_stop();
}

// Play
void psg_play(const void* data)
{
    // stop current music
    psg_stop();
    // play if valid eyecatch
    if (0 == memcmp(data, "PSG", 4)) {
        _psg.data = (const uint8_t*)data;
        _psg.cursor = _psg.data + 4;
    }
}

void psg_stop(void)
{
    *((uint16_t*)0x4000062) = 0x0000; // ch1 volume zero
    *((uint16_t*)0x4000068) = 0x0000; // ch2 volume zero
    *((uint16_t*)0x4000072) = 0x0000; // ch3 volume zero
    *((uint16_t*)0x4000078) = 0x0000; // ch4 volume zero
    _psg.data = NULL;
    _psg.fadeout = 0;
}

void psg_fadeout(void)
{
    if (0 == _psg.fadeout) {
        _psg.fadeout = 1;
    }
}

// frame procedure
void psg_frame(void)
{
    if (!_psg.data) {
        return; // no data
    }
    uint8_t addr = *_psg.cursor++;
    while (0x00 != addr) {
        if (0xFF != addr) {
            if (!_psg.fadeout || 0x80 != addr) {
                *(uint8_t*)(0x4000000 | addr) = *_psg.cursor++;
            }
        } else {
            // end of sound data
            uint32_t loop;
            memcpy(&loop, _psg.cursor, 4);
            if (loop) {
                _psg.cursor = _psg.data + loop;
            } else {
                psg_stop();
                return;
            }
        }
        addr = *_psg.cursor++;
    }
    if (_psg.fadeout) {
        _psg.fadeout++;
        if (0 == (_psg.fadeout & 0x0F)) {
            uint8_t vol = *((uint8_t*)0x4000080);
            uint8_t r = vol & 0b00000111;
            uint8_t l = (vol & 0b01110000) >> 4;
            if (r) {
                r--;
            }
            if (l) {
                l--;
                l <<= 4;
            }
            *((uint8_t*)0x4000080) = r | l;
            if (0 == r && 0 == l) {
                psg_stop();
            }
        }
    }
}
