#pragma once
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SFX_NUM 256

void sfx_init(void);
void sfx_load(int no, const void* data, size_t size);
void sfx_play(int no);
void sfx_stop();

// This function is called every frame when VBLANK is reached,
// but the user program does not need to call it
// because it is automatically called from `vdp_wait_vblank`.
void sfx_frame(void);

#ifdef __cplusplus
};
#endif
