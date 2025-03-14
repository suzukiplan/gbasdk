#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t* _joypad;

/*
 * joypad_get is called only once per frame.
 * The result is held in a variable and passed to the joypad_check_ argument to check the input.
 *
 * Bit   Expl.
 * 0     Button A        (0=Pressed, 1=Released)
 * 1     Button B        (etc.)
 * 2     Select          (etc.)
 * 3     Start           (etc.)
 * 4     Right           (etc.)
 * 5     Left            (etc.)
 * 6     Up              (etc.)
 * 7     Down            (etc.)
 * 8     Button R        (etc.)
 * 9     Button L        (etc.)
 * 10~15 unused
 */
inline uint16_t joypad_get(void) { return *_joypad; }
inline int joypad_check_a(uint16_t pad) { return 0 == (pad & 0b0000000001); }
inline int joypad_check_b(uint16_t pad) { return 0 == (pad & 0b0000000010); }
inline int joypad_check_select(uint16_t pad) { return 0 == (pad & 0b0000000100); }
inline int joypad_check_start(uint16_t pad) { return 0 == (pad & 0b0000001000); }
inline int joypad_check_right(uint16_t pad) { return 0 == (pad & 0b0000010000); }
inline int joypad_check_left(uint16_t pad) { return 0 == (pad & 0b0000100000); }
inline int joypad_check_up(uint16_t pad) { return 0 == (pad & 0b0001000000); }
inline int joypad_check_down(uint16_t pad) { return 0 == (pad & 0b0010000000); }
inline int joypad_check_r(uint16_t pad) { return 0 == (pad & 0b0100000000); }
inline int joypad_check_l(uint16_t pad) { return 0 == (pad & 0b1000000000); }
inline int joypad_check_any(uint16_t pad) { return 0b1100001111 != (pad & 0b1100001111); }

#ifdef __cplusplus
};
#endif
