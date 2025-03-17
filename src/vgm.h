#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void vgm_init(void);
void vgm_play(const void* data);
void vgm_stop(void);
void vgm_frame(void);

#ifdef __cplusplus
};
#endif
