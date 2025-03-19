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
            *(uint8_t*)(0x4000000 | addr) = *_psg.cursor++;
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
}
