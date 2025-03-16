#include <gba_sound.h>
#include <gba_timers.h>
#include <gba_dma.h>
#include "sfx.h"

#define SND_CPU_CLOCK   (16 * 1024 * 1024)
#define SND_PCM_RATE    16384
#define SND_FREQUENCY   (SND_CPU_CLOCK / SND_PCM_RATE)

typedef struct SFXData_ {
    const int8_t* data;
    int frames;
} SFXData;

static struct SFXTable {
    int left;
    int playing;
    SFXData data[MAX_SFX_NUM];
} _sfx;

void sfx_init(void)
{
    memset(&_sfx, 0, sizeof(_sfx));
	REG_TM0CNT_L = 0x10000 - SND_FREQUENCY;
	REG_SOUNDCNT_X = SNDSTAT_ENABLE;
	REG_SOUNDCNT_L = 0;
	REG_SOUNDCNT_H = SNDA_RESET_FIFO | SNDB_RESET_FIFO | SNDA_VOL_100 | SNDB_VOL_100 | DSOUNDCTRL_ATIMER(0) | DSOUNDCTRL_BTIMER(1);
}

void sfx_load(int no, const void* data, size_t size)
{
    no &= MAX_SFX_NUM - 1;
    _sfx.data[no].data = (const int8_t*)data;
    _sfx.data[no].frames = ((size - 1024) * 60) / SND_PCM_RATE;
}

void sfx_play(int no)
{
    if (_sfx.left && _sfx.playing < no) {
        return; // Canceled because a high priority (low number) sound effect is playing
    }
    if (_sfx.data[no].data) {
        _sfx.playing = no;
        _sfx.left = _sfx.data[no].frames;
        REG_TM0CNT_H = 0;
        REG_DMA1CNT = 0;
        DMA1COPY(_sfx.data[no].data, &REG_FIFO_A, DMA_SPECIAL | DMA32 | DMA_REPEAT | DMA_SRC_INC | DMA_DST_FIXED);
        REG_TM0CNT_H    = TIMER_START;
        REG_SOUNDCNT_H |= (SNDA_R_ENABLE | SNDA_L_ENABLE | SNDA_RESET_FIFO);
    } else {
        sfx_stop(); // no data
    }
}

void sfx_stop()
{
    REG_SOUNDCNT_H &= ~(SNDA_R_ENABLE | SNDA_L_ENABLE | SNDA_RESET_FIFO);
    REG_TM1CNT_H = 0;
    REG_DMA1CNT = 0;
    _sfx.left = 0;
}

void sfx_frame(void)
{
    if (_sfx.left) {
        _sfx.left--;
        if (0 == _sfx.left) {
            sfx_stop();
        }
    }
}
