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
|[./src/sfx.h](./src/sfx.h)|PCM 効果音関連のAPI|
|[./src/joypad.h](./src/joypad.h)|ジョイパッド関連のAPI|

### Tools

- [./tools/bmp2pal/](./tools/bmp2pal/) ... 256色BitmapからGBA形式パレットを生成
- [./tools/bmp2chr/](./tools/bmp2chr/) ... 256色 or 16色BitmapからGBAの16/16形式
- [./tools/wav2pcm/](./tools/wav2pcm/) ... 44100Hz, 16bit, 1ch (VGS-Zero形式) の wav ファイルを GBA 形式（16384Hz, 8bit, 1ch）の RAW PCM データ（sfx形式）に変換

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
