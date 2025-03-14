# GBA SDK for port from VGS-Zero (WIP)

VGS-Zero から GBA へのゲームの移植に役立つライブラリとツールセットです。

## How to use

### Libraries

1. ソースコードディレクトリに [./src](./src) 以下のファイルをコピー
2. `#include "gbasdk.h"`

API 仕様はヘッダファイルの実装を確認してください。

[gbasdk.h](./src/gbasdk.h) は以下の機能別 API のヘッダファイルを `#include` しています。

|Header File|Function|
|:-----|:-------|
|[./src/vdp.h](./src/vdp.h)|映像処理装置 (VDP) 関連のAPI|
|[./src/joypad.h](./src/joypad.h)|ジョイパッド関連のAPI|

### Tools

- [./tools/bmp2pal/](./tools/bmp2pal/) ... 256色BitmapからGBA形式パレットを生成
- [./tools/bmp2chr/](./tools/bmp2chr/) ... 256色 or 16色BitmapからGBAの16/16形式パターンデータを生成

## Example

`Hello, World!` を表示する簡単な実装例を提供しています。

```c
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
    while (ON) { vdp_wait_vblank(); }           // VBLANK待ちを繰り返す無限ループ
}
```

- ビルド環境の OS は macOS or Linux をサポートしています
- devkitPro で構築した開発環境での利用を想定しています
- example の実行には mgba が必要です
  - macOS: `brew install mgba`
  - Linux: `apt install mgba`

上記が整った環境で以下のコマンドを実行すれば ビルド & 実行 ができます。

```bash
git clone https://github.com/suzukiplan/gbasdk
cd gbasdk/example
make
```

![preview](preview.png)

## License

[MIT](LICENSE.txt)
