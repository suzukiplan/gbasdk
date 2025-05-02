#pragma once
#include <string.h>
#include <stdint.h>

#define ON 1
#define OFF 0

typedef struct
{
    /**
     *   Bit   Expl.
     *   0-7   Y-Coordinate           (0-255)
     *   8     Rotation/Scaling Flag  (0=Off, 1=On)
     *   When Rotation/Scaling used (Attribute 0, bit 8 set):
     *     9     Double-Size Flag     (0=Normal, 1=Double)
     *   When Rotation/Scaling not used (Attribute 0, bit 8 cleared):
     *     9     OBJ Disable          (0=Normal, 1=Not displayed)
     *   10-11 OBJ Mode  (0=Normal, 1=Semi-Transparent, 2=OBJ Window, 3=Prohibited)
     *   12    OBJ Mosaic             (0=Off, 1=On)
     *   13    Colors/Palettes        (0=16/16, 1=256/1)
     *   14-15 OBJ Shape              (0=Square,1=Horizontal,2=Vertical,3=Prohibited)
     */
    uint16_t attr0;

    /**
     *   Bit   Expl.
     *   0-8   X-Coordinate           (0-511)
     *   When Rotation/Scaling used (Attribute 0, bit 8 set):
     *     9-13  Rotation/Scaling Parameter Selection (0-31)
     *           (Selects one of the 32 Rotation/Scaling Parameters that
     *           can be defined in OAM, for details read next chapter.)
     *   When Rotation/Scaling not used (Attribute 0, bit 8 cleared):
     *     9-11  Not used
     *     12    Horizontal Flip      (0=Normal, 1=Mirrored)
     *     13    Vertical Flip        (0=Normal, 1=Mirrored)
     *   14-15 OBJ Size               (0..3, depends on OBJ Shape, see Attr 0)
     *           Size  Square   Horizontal  Vertical
     *           0     8x8      16x8        8x16
     *           1     16x16    32x8        8x32
     *           2     32x32    32x16       16x32
     *           3     64x64    64x32       32x64
     */
    uint16_t attr1;

    /**
     *   Bit   Expl.
     *   0-9   Character Name          (0-1023=Tile Number)
     *   10-11 Priority relative to BG (0-3; 0=Highest)
     *   12-15 Palette Number   (0-15) (Not used in 256 color/1 palette mode)
     */
    uint16_t attr2;

    /**
     * Used for affine transformations such as rotation and scaling
     */
    uint16_t dummy;
} OAM;

extern uint16_t* _dispcnt;
extern uint16_t* _scroll;
extern uint16_t _scroll_buffer[8];
extern uint16_t* _palette;
extern uint16_t* _pattern_bg;
extern uint16_t* _pattern_sp;
extern uint16_t* _map;
extern uint16_t* _nametbl[4];
extern OAM* _oam;

#define OAM_ATTR0_SCALING_OFF 0b000000000
#define OAM_ATTR0_SCALING_ON 0b100000000
#define OAM_ATTR0_DOUBLE_OFF 0b0000000000
#define OAM_ATTR0_DOUBLE_ON 0b1000000000
#define OAM_ATTR0_MODE_NORMAL 0b000000000000
#define OAM_ATTR0_MODE_SEMI_TRANSPARENT 0b010000000000
#define OAM_ATTR0_MODE_WINDOW 0b100000000000
#define OAM_ATTR0_MOSAIC_ON 0b1000000000000
#define OAM_ATTR0_MOSAIC_OFF 0b0000000000000
#define OAM_ATTR0_PALETTE_16 0b00000000000000
#define OAM_ATTR0_PALETTE_256 0b10000000000000
#define OAM_ATTR0_SHAPE_SQUARE 0b0000000000000000
#define OAM_ATTR0_SHAPE_HORIZONTAL 0b0100000000000000
#define OAM_ATTR0_SHAPE_VERTICAL 0b1000000000000000
#define OAM_ATTR1_TRANSFORM_MASK 0b1100000111111111
#define OAM_ATTR1_TRANSFORM_NONE 0b00000000000000
#define OAM_ATTR1_TRANSFORM_FLIP_HORIZONTAL 0b01000000000000
#define OAM_ATTR1_TRANSFORM_FLIP_VERTICAL 0b10000000000000
#define OAM_ATTR1_TRANSFORM_SCALE_05 0b11100000000000
#define OAM_ATTR1_TRANSFORM_SCALE_10 0b11101000000000
#define OAM_ATTR1_TRANSFORM_SCALE_15 0b11110000000000
#define OAM_ATTR1_TRANSFORM_SCALE_20 0b11111000000000
extern const uint16_t OAM_ATTR1_TRANSFORM_SCALE[4];
extern const uint16_t OAM_ATTR1_TRANSFORM_ROTATE[16 + 4];
extern const uint16_t OAM_ATTR1_SIZE[4];
extern const uint16_t OAM_ATTR2_PRIORITY[4];
extern const uint16_t OAM_ATTR2_PALETTE[16];

// attr0 設定用の低レベルAPI (基本使わない想定)
inline void vdp_oam_set_attr0(OAM* oam, uint8_t y, uint16_t scaling, uint16_t double_size, uint16_t mode, uint16_t mosaic, uint16_t shape)
{
    oam->attr0 = y | scaling | double_size | mode | mosaic | shape;
}

// attr1 設定用の低レベルAPI (基本使わない想定)
inline void vdp_oam_set_attr1(OAM* oam, uint16_t x, uint16_t transform, uint16_t size)
{
    oam->attr1 = (x & 0x1FF) | transform | size;
}

// attr2 設定用の低レベルAPI (基本使わない想定)
inline void vdp_oam_set_attr2(OAM* oam, uint16_t ptn, uint16_t priority, uint16_t palette)
{
    oam->attr2 = (ptn & 0x3FF) | priority | palette;
}

/**
 * VDP を初期化
 */
void vdp_init(void);

/**
 * V-BLANK を待機
 *
 * Remarks
 * - この関数で内部的にBGMのフレーム処理が実行される
 * - この関数で内部的にSFXのフレーム処理が実行される
 * - この関数で内部的に仮想OAMを実OAMへコピーする処理が実行される（OAMの更新は not blanking の間でも実行可能）
 */
void vdp_wait_vblank(void);

/**
 * 画面を常にV-BLANKの状態にする（画面が真っ白になるので注意）
 */
void vdp_force_vblank(int on);

/**
 * パレットを初期化
 */
void vdp_palette_init(const void* data);

/**
 * BG用キャラクタパターンを VRAM に読み込む
 */
void vdp_set_tile(int index, const void* data, size_t size);

/**
 * 指定index間でキャラクタパターンのコピーを行う
 */
void vdp_copy_tile(int to, int from);

/**
 * 指定indexのキャラクタパターンに水平インタレースを入れる
 */
void vdp_interlace_h(int index, int num);

/**
 * 指定indexのキャラクタパターンに垂直インタレースを入れる
 */
void vdp_interlace_v(int index, int num);

/**
 * 指定BGに 0x00 で終端するバイトコード列を書き込む
 */
void vdp_print_bg(int no, int x, int y, const char* text);

/**
 * 全てのBG領域をクリアしてスクロールを初期値に設定
 */
void vdp_cls();

/**
 * BGの表示基点を設定（HWスクロール）
 */
inline void vdp_scroll(int no, uint16_t x, uint16_t y)
{
    no &= 0x03;
    no <<= 1;
    x &= 0x1FF;
    y &= 0x1FF;
    _scroll[no] = x;
    _scroll[no + 1] = y;
    _scroll_buffer[no] = x;
    _scroll_buffer[no + 1] = y;
}

/**
 * BGの表示基点を取得（HWスクロール）
 */
inline void vdp_scroll_get(int no, uint16_t* x, uint16_t* y)
{
    no &= 0x03;
    no <<= 1;
    *x = _scroll_buffer[no];
    *y = _scroll_buffer[no + 1];
}

/**
 * BGの表示基点を設定（水平HWスクロール）
 */
inline void vdp_scroll_x(int no, uint16_t x)
{
    no &= 0x03;
    no <<= 1;
    x &= 0x1FF;
    _scroll[no] = x;
    _scroll_buffer[no] = x;
}

/**
 * BGの表示基点を設定（垂直HWスクロール）
 */
inline void vdp_scroll_y(int no, uint16_t y)
{
    no &= 0x03;
    no <<= 1;
    y &= 0x1FF;
    _scroll[no | 1] = y;
    _scroll_buffer[no + 1] = y;
}

/**
 * BGの指定位置に指定パターンを指定パレットで書き込む
 */
inline void vdp_put_bg(int no, int x, int y, uint16_t pal, uint16_t ptn)
{
    _nametbl[no & 0x03][(y & 0x1F) * 0x20 + (x & 0x1F)] = ((pal & 0xF) << 12) | ptn;
}

/**
 * 特定のBGをクリア
 */
inline void vdp_cls_bg(int no)
{
    memset(_nametbl[no & 0x03], 0, 0x800);
}

/**
 * 特定のBGの特定矩形領域をクリア
 */
inline void vdp_clear_bg(int no, int x, int y, int width, int height)
{
    if (0 < width && 0 < height) {
        no &= 0x03;
        for (int i = 0; i < height; i++) {
            memset(_nametbl[no] + 0x20 * (i + y) + x, 0, width * 2);
        }
    }
}

/**
 * 特定のBGを指定パターン番号を埋める
 */
inline void vdp_fill_bg(int no, uint16_t pal, uint16_t ptn)
{
    uint16_t attr = ((pal & 0xF) << 12) | ptn;
    no &= 0x03;
    for (int i = 0; i < 0x400; i++) {
        _nametbl[no][i] = attr;
    }
}

/**
 * スプライト用キャラクタパターンを VRAM に読み込む
 */
void vdp_set_tile_sp(int index, const void* data, size_t size);

/**
 * スプライトを初期化
 */
void vdp_oam_init(
    OAM* oam,      // target OAM
    int transform, // 0: disable, not0: enable
    int shape,     // type of size ... 0: 8x8,16x16,32x32,64x64 | 1: 16x8,32x8,32x16,64x32 | 2: 8x16,8x32,16x32,32x64
    int ptn,       // pattern index number (0-511)
    int size,      // pattern size (0-3)
    int palette    // palette number (0-15)
);

/**
 * 全てのスプライトをクリア
 */
void vdp_oam_clear(void);

/**
 * スプライトの座標を更新
 */
inline void vdp_oam_pos(OAM* oam, int x, int y)
{
    oam->attr0 = (oam->attr0 & 0xFF00) | (y & 0xFF);
    oam->attr1 = (oam->attr1 & 0xFE00) | (x & 0x1FF);
}

/**
 * スプライトを非表示にする（画面外に移動する）
 */
inline void vdp_oam_hide(OAM* oam)
{
    oam->attr0 = (oam->attr0 & 0xFF00) | 192;
}

/**
 * スプライトのパターン番号を更新
 */
inline void vdp_oam_ptn(OAM* oam, int ptn)
{
    oam->attr2 = (oam->attr2 & 0xFC00) | (ptn & 0x3FF);
}

/**
 * スプライトのパレット番号を更新
 */
inline void vdp_oam_palette(OAM* oam, int palette)
{
    oam->attr2 = (oam->attr2 & 0x0FFF) | (palette << 12);
}

/**
 * スプライトのパレット番号を取得
 */
inline int vdp_oam_palette_get(OAM* oam)
{
    return (oam->attr2 & 0xF000) >> 12;
}

/**
 * スプライトのshapeとsizeを設定
 */
inline void vdp_oam_resize(OAM* oam, int shape, int size)
{
    shape <<= 14;
    size <<= 14;
    oam->attr0 = (oam->attr0 & 0x3FFF) | shape;
    oam->attr1 = (oam->attr1 & 0x3FFF) | size;
}

/**
 * スプライトを左右反転する
 *
 * Remarks:
 * - 本関数を呼び出すスプライトは vdp_oam_init で transform に OFF を設定する必要がある
 */
inline void vdp_oam_flipH(OAM* oam, int on)
{
    if (on) {
        oam->attr1 |= 0x1000;
    } else {
        oam->attr1 &= 0xEFFF;
    }
}

/**
 * スプライトを上下反転する
 *
 * Remarks:
 * - 本関数を呼び出すスプライトは vdp_oam_init で transform に OFF を設定する必要がある
 */
inline void vdp_oam_flipV(OAM* oam, int on)
{
    if (on) {
        oam->attr1 |= 0x2000;
    } else {
        oam->attr1 &= 0xDFFF;
    }
}

/**
 * スプライトのtransform on/off
 */
inline void vdp_oam_transform(OAM* oam, int on)
{
    if (on) {
        oam->attr0 |= 0x0100;
    } else {
        oam->attr0 &= 0xFEFF;
    }
}

/**
 * スプライトサイズを倍にする
 * 拡大はせず領域が倍になりリソース消費が4倍になるので極力設定しないことが望ましい
 *
 * Remarks:
 * - 本関数を呼び出すスプライトは vdp_oam_init で transform に ON を設定する必要がある
 * - transform が OFF のスプライトに対して ON をするとスプライトが非表示になる
 */
inline void vdp_oam_double(OAM* oam, int on)
{
    if (on) {
        oam->attr0 |= 0b0000001000000000;
    } else {
        oam->attr0 &= 0b1111110111111111;
    }
}

/**
 * スプライトを回転
 * rotate:
 * - 0~15 = 0度~338度 (※約22.5度刻み)
 * - 16 = 15度, 17 = 30度, 18 = 330度, 19 = 345度
 *
 * Remarks:
 * - rotate に 20 以上 または 0 未満の値を設定するとバッファオーバライトするので注意
 * - 本関数を呼び出すスプライトは vdp_oam_init で transform に ON を設定する必要がある
 * - 画像によっては見切れる場合がある
 * - 確実に見切れないようにするには vdp_oam_double を ON に設定する必要がある
 */
inline void vdp_oam_rotate(OAM* oam, int rotate)
{
    oam->attr1 = (oam->attr1 & OAM_ATTR1_TRANSFORM_MASK) | OAM_ATTR1_TRANSFORM_ROTATE[rotate];
}

/**
 * スプライトを拡大 or 縮小
 * scale: 0 = 0.5倍, 1 = 1.0倍, 2 = 1.5倍, 3 = 2.0倍
 *
 * Remarks:
 * - scale に 4 以上 または 0 未満の値を設定するとバッファオーバライトするので注意
 * - 本関数を呼び出すスプライトは vdp_oam_init で transform に ON を設定する必要がある
 * - 2 (1.5倍) 以上で拡大表示と見切れが発生する
 * - 見切れを回避するには vdp_oam_double を ON に設定する必要がある
 */
inline void vdp_oam_scale(OAM* oam, int scale)
{
    oam->attr1 = (oam->attr1 & OAM_ATTR1_TRANSFORM_MASK) | OAM_ATTR1_TRANSFORM_SCALE[scale];
}

/**
 * スプライトをコピー
 */
inline void vdp_oam_copy(OAM* to, OAM* from)
{
    to->attr0 = from->attr0;
    to->attr1 = from->attr1;
}
