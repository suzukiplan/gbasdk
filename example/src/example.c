
#include "vdp.h"
#include "palette_dat.h"
#include "font_dat.h"

int main(void)
{
	vdp_init();									// VDPを初期化
	vdp_force_vblank(ON);						// いつでもVRAM更新できる状態にする
	vdp_palette_init(palette_dat);				// 16x16パレットを初期化
	vdp_set_tile(0, font_dat, font_dat_size);	// フォント画像をVRAMへ読み込む
	vdp_cls();									// 画面をクリア
	vdp_print_bg(0, 9, 10, "HELLO,WORLD!");		// BG0 の (9,10) の位置に HELLO,WORLD! を表示
	vdp_force_vblank(OFF);						// VBLANK時のみ画面更新できる状態にする
	while (ON) { vdp_wait_vblank(); }			// VBLANK待ちを繰り返す無限ループ
}
