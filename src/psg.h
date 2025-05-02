#pragma once

void psg_init(void);
void psg_play(const void* data);
void psg_stop(void);
void psg_fadeout(void);

// This function is called every frame when VBLANK is reached,
// but the user program does not need to call it
// because it is automatically called from `vdp_wait_vblank`.
void psg_frame(void);
