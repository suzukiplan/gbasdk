# GBA SDK for port from VGS-Zero (WIP)

[VGS-Zero](https://github.com/suzukiplan/vgszero) から GBA へのゲームの移植に役立つライブラリとツールセットです。

## Feature

本SDKは、VGS-Zero用に開発されたゲームをGBAへ移植する用途に特化した仕様にしています。

- グラフィックスは mode0 の **16色/16パレット** のみ使用
- map (nametable) は BG0~3 の全面で 32x32 サイズに固定
- 音楽はGB/GBC互換音源のみを使用
- 効果音はPCM 1chを使用（性能を優先して PCM 2ch は未使用）
- VGS-Zero のグラフィックスと効果音をほぼそのまま移行できるツールチェインを提供
- BGM は Furnace Tracker での作成を推奨（GB/GBC互換音源の VGM を本SDKで利用できる .psg 形式に変換するツールチェインを提供）

VGS-Zero との仕様差

|比較項目|VGS-Zero|GBA (本SDK)|
|-:|:-:|:-:|
|画面解像度| 240x192 | 240x160 |
|BG面数| 2面(BG/FG) | 4面(BG0,1,2,3) |
|nametable| 32x32 | 32x32 |
|表示色| 16色/16パレット | 16色/16パレット |
|最大パターン数|65792 (VRAM+DPM)|1024|
|スプライト数|256|128|
|スプライトサイズ|8x8 ~ 128x128|8x8 ~ 64x64|
|BGM|VGS or NSF|DMG (GB/GBC)|
|SFX|PCM (44.1k, 16bits, 1ch)|PCM (16k, 8bits, 1ch)|
|SFX同時発音|256|1|

## How to use

### Libraries

1. ソースコードディレクトリに [./src](./src) 以下のファイルをコピー
2. `#include "gbasdk.h"`

API 仕様はヘッダファイルの実装を確認してください。

[gbasdk.h](./src/gbasdk.h) は以下の機能別 API のヘッダファイルを `#include` しています。

|Header File|Function|
|:-----|:-------|
|[./src/vdp.h](./src/vdp.h)|映像処理装置 (VDP) 関連の API|
|[./src/psg.h](./src/psg.h)|BGM (GB互換音源) 関連の API|
|[./src/sfx.h](./src/sfx.h)|効果音 (PCM) 関連の API|
|[./src/joypad.h](./src/joypad.h)|ジョイパッド関連の API|

### Tools

- [./tools/bmp2pal/](./tools/bmp2pal/) ... 256色BitmapからGBA形式パレットを生成
- [./tools/bmp2chr/](./tools/bmp2chr/) ... 256色 or 16色BitmapからGBAの16/16形式
- [./tools/wav2pcm/](./tools/wav2pcm/) ... 44100Hz, 16bit, 1ch (VGS-Zero形式) の wav ファイルを GBA 形式（16384Hz, 8bit, 1ch）の RAW PCM データに変換
- [./tools/vgm2psg/](./tools/vgm2psg/) ... [Furnace Tracker](https://github.com/tildearrow/furnace) 等で出力した GB 音源用の VGM ファイルを BGM API が読めるデータ形式に変換

## Example

`Hello, World!` を表示する簡単な実装例を提供しています。

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

- ジョイパッドの上下左右を押すことで HW スクロール機能を用いて `Hello, World!` を 8 方向に動かすことができます
- スタートボタンを押すことで効果音が鳴ります

詳細は [./example/src/example.c](./example/src/example.c) の実装をご確認ください。

## License

[MIT](LICENSE.txt)
