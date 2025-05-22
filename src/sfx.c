/**
 * Play 8-bit PCM as sound effect using DMA1 and Timer0
 * This code was created with reference to the following wiki:
 * https://akkera102.sakura.ne.jp/gbadev/index.php?tutorial.22
 */
#include <gba_sound.h>
#include <gba_timers.h>
#include <gba_dma.h>
#include "sfx.h"

#define SND_CPU_CLOCK (16 * 1024 * 1024)
#define SND_PCM_RATE 16384
#define SND_FREQUENCY (SND_CPU_CLOCK / SND_PCM_RATE)

typedef struct SFXData_ {
    const int8_t* data;
    int frames;
} SFXData;

typedef struct Channel_ {
    int left;
    int playing;
    int stop_request;
    int play_request;
} Channel;

static struct SFXTable {
    Channel ch[2];
    SFXData data[MAX_SFX_NUM];
} _sfx;

void sfx_init(void)
{
    memset(&_sfx, 0, sizeof(_sfx));
    REG_TM0CNT_L = 0x10000 - SND_FREQUENCY;
    REG_TM1CNT_L = 0x10000 - SND_FREQUENCY;
    REG_SOUNDCNT_L = 0;
    REG_SOUNDCNT_H = SNDA_RESET_FIFO | SNDB_RESET_FIFO | SNDA_VOL_100 | SNDB_VOL_100 | DSOUNDCTRL_ATIMER(0) | DSOUNDCTRL_BTIMER(1) | 1;
}

void sfx_load(int no, const void* data, size_t size)
{
    no &= MAX_SFX_NUM - 1;
    _sfx.data[no].data = (const int8_t*)data;
    _sfx.data[no].frames = ((size - 1024) * 60) / SND_PCM_RATE;
}

static inline void _play(int ch, int no)
{
    if (0 != ch && 1 != ch) {
        return; // invalid channel
    }
    if (_sfx.ch[ch].left && _sfx.ch[ch].playing < no) {
        return; // Canceled because a high priority (low number) sound effect is playing
    }
    if (_sfx.data[no].data) {
        _sfx.ch[ch].play_request = 1;
        _sfx.ch[ch].playing = no;
    }
}

static inline void _stop(int ch)
{
    if (0 != ch && 1 != ch) {
        return; // invalid channel
    }
    if (0 == ch) {
        REG_SOUNDCNT_H &= ~(SNDA_R_ENABLE | SNDA_L_ENABLE | SNDA_RESET_FIFO);
        REG_TM1CNT_H = 0;
        REG_DMA1CNT = 0;
    } else {
        REG_SOUNDCNT_H &= ~(SNDB_R_ENABLE | SNDB_L_ENABLE | SNDB_RESET_FIFO);
        REG_TM2CNT_H = 0;
        REG_DMA2CNT = 0;
    }
    _sfx.ch[ch].left = 0;
}

void sfx_play(int no)
{
    _play(0, no);
}

void sfx_stop()
{
    _sfx.ch[0].stop_request = 1;
}

void sfx_play_ch2(int no)
{
    _play(1, no);
}

void sfx_stop_ch2()
{
    _sfx.ch[1].stop_request = 1;
}

void sfx_frame(void)
{
    for (int ch = 0; ch < 2; ch++) {
        if (_sfx.ch[ch].stop_request) {
            _stop(ch);
            _sfx.ch[ch].stop_request = 0;
        }
        if (_sfx.ch[ch].play_request) {
            _sfx.ch[ch].play_request = 0;
            _sfx.ch[ch].left = _sfx.data[_sfx.ch[ch].playing].frames;
            if (0 == ch) {
                REG_TM0CNT_H = 0;
                REG_DMA1CNT = 0;
                DMA1COPY(_sfx.data[_sfx.ch[ch].playing].data, &REG_FIFO_A, DMA_SPECIAL | DMA32 | DMA_REPEAT | DMA_SRC_INC | DMA_DST_FIXED);
                REG_TM0CNT_H = TIMER_START;
                REG_SOUNDCNT_H |= (SNDA_R_ENABLE | SNDA_L_ENABLE | SNDA_RESET_FIFO);
            } else {
                REG_TM1CNT_H = 0;
                REG_DMA2CNT = 0;
                DMA2COPY(_sfx.data[_sfx.ch[ch].playing].data, &REG_FIFO_B, DMA_SPECIAL | DMA32 | DMA_REPEAT | DMA_SRC_INC | DMA_DST_FIXED);
                REG_TM1CNT_H = TIMER_START;
                REG_SOUNDCNT_H |= (SNDB_R_ENABLE | SNDB_L_ENABLE | SNDB_RESET_FIFO);
            }
        }
        if (_sfx.ch[ch].left) {
            _sfx.ch[ch].left--;
            if (0 == _sfx.ch[ch].left) {
                sfx_stop(ch);
            }
        }
    }
}
