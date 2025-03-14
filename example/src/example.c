#include "gbasdk.h"
#include "palette_dat.h"
#include "font_dat.h"

int main(void)
{
    vdp_init();                                 // VDPを初期化
    vdp_force_vblank(ON);                       // いつでもVRAM更新できる状態にする
    vdp_palette_init(palette_dat);              // 16x16パレットを初期化
    vdp_set_tile(0, font_dat, font_dat_size);   // フォント画像をVRAMへ読み込む
    vdp_cls();                                  // 画面をクリア
    vdp_print_bg(0, 9, 10, "HELLO,WORLD!");     // BG0 の (9,10) の位置に HELLO,WORLD! を表示
    vdp_force_vblank(OFF);                      // VBLANK時のみ画面更新できる状態にする

    uint16_t sx = 0;
    uint16_t sy = 0;
    while (ON) {
        // VBLANKを待機
        vdp_wait_vblank();

        // ゲームパッドの入力を取得
        uint16_t joypad = joypad_get();

        // dpadの左右が押された場合はBGを左右スクロール
        if (joypad_check_left(joypad)) {
            vdp_scroll_x(0, ++sx);
        } else if (joypad_check_right(joypad)) {
            vdp_scroll_x(0, --sx);
        }

        // dpadの上下が押された場合はBGを上下スクロール
        if (joypad_check_up(joypad)) {
            vdp_scroll_y(0, ++sy);
        } else if (joypad_check_down(joypad)) {
            vdp_scroll_y(0, --sy);
        }
    }
}
