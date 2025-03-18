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
    *((uint8_t*)0x4000084) = 0x00; // Reset PSG/FIFO Master (set the all of all PSG regs = 0) and Sound1~4 OFF (full muted)
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
