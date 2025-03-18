#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void psg_init(void);
void psg_play(const void* data);
void psg_stop(void);
void psg_frame(void);

#ifdef __cplusplus
};
#endif
