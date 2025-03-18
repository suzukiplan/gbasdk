/**
 * VGM ファイルを GBA PSG のレジスタセット形式に変更
 * PSG\0              : アイキャッチ (4bytes)
 * 0x60~0x9F, value   : 書き込みレジスタの下位 8 bit と 書き込み値 (1byte)
 * 0x00               : wait vblank
 * 0xFF, offset       : EOF と offset (4bytes)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static const uint8_t _vgm_regs[256] = {
            // VGM  Name DMG-A  GBA-A     Description
    0x60,   // 0x00 NR10 0xFF10 0x4000060 矩形波１ 制御レジスタ（スイープ）
    0x62,   // 0x01 NR11 0xFF11 0x4000062 矩形波１ 制御レジスタ（音色・長さ）
    0x63,   // 0x02 NR12 0xFF12 0x4000063 矩形波１ 制御レジスタ（音量）
    0x64,   // 0x03 NR13 0xFF13 0x4000064 矩形波１ 制御レジスタ（周波数下位）
    0x65,   // 0x04 NR14 0xFF14 0x4000065 矩形波１ 制御レジスタ（周波数上位・長さ・キーオン）
    0x00,   // 0x05 NR20 0xFF15 unused
    0x68,   // 0x06 NR21 0xFF16 0x4000068 矩形波２ 制御レジスタ（音色・長さ）
    0x69,   // 0x07 NR22 0xFF17 0x4000069 矩形波２ 制御レジスタ（音量）
    0x6C,   // 0x08 NR23 0xFF18 0x400006C 矩形波２ 制御レジスタ（周波数下位）
    0x6D,   // 0x09 NR24 0xFF19 0x400006D 矩形波２ 制御レジスタ（周波数上位・長さ・キーオン）
    0x70,   // 0x0A NR30 0xFF1A 0x4000070 波形メモリ 制御レジスタ（マスター）
    0x72,   // 0x0B NR31 0xFF1B 0x4000072 波形メモリ 制御レジスタ（長さ）
    0x73,   // 0x0C NR32 0xFF1C 0x4000073 波形メモリ 制御レジスタ（音量）
    0x74,   // 0x0D NR33 0xFF1D 0x4000074 波形メモリ 制御レジスタ（周波数下位）
    0x75,   // 0x0E NR34 0xFF1E 0x4000075 波形メモリ 制御レジスタ（周波数上位・長さ・キーオン）
    0x00,   // 0x0F NR40 0xFF1F unused
    0x78,   // 0x10 NR41 0xFF20 0x4000078 ノイズ 制御レジスタ（長さ）
    0x79,   // 0x11 NR42 0xFF21 0x4000079 ノイズ 制御レジスタ（音量）
    0x7C,   // 0x12 NR43 0xFF22 0x400007C ノイズ 制御レジスタ（周波数）
    0x7D,   // 0x13 NR44 0xFF23 0x400007D ノイズ 制御レジスタ（長さ・キーオン）
    0x80,   // 0x14 NR50 0xFF24 0x4000080 全体音量制御レジスタ
    0x81,   // 0x15 NR51 0xFF25 0x4000081 各チャンネル出力制御レジスタ
    0x84,   // 0x16 NR52 0xFF26 0x4000084 各チャンネルステータス/サウンド有効フラグレジスタ
    0x00,   // 0x17      0xFF27 unused
    0x00,   // 0x18      0xFF28 unused
    0x00,   // 0x19      0xFF29 unused
    0x00,   // 0x1A      0xFF2A unused
    0x00,   // 0x1B      0xFF2B unused
    0x00,   // 0x1C      0xFF2C unused
    0x00,   // 0x1D      0xFF2D unused
    0x00,   // 0x1E      0xFF2E unused
    0x00,   // 0x1F      0xFF2F unused
    0x90,   // 0x20      0xFF30 0x4000090 波形メモリデータ
    0x91,   // 0x21      0xFF31 0x4000091 波形メモリデータ
    0x92,   // 0x22      0xFF32 0x4000092 波形メモリデータ
    0x93,   // 0x23      0xFF33 0x4000093 波形メモリデータ
    0x94,   // 0x24      0xFF34 0x4000094 波形メモリデータ
    0x95,   // 0x25      0xFF35 0x4000095 波形メモリデータ
    0x96,   // 0x26      0xFF36 0x4000096 波形メモリデータ
    0x97,   // 0x27      0xFF37 0x4000097 波形メモリデータ
    0x98,   // 0x28      0xFF38 0x4000098 波形メモリデータ
    0x99,   // 0x29      0xFF39 0x4000099 波形メモリデータ
    0x9A,   // 0x2A      0xFF3A 0x400009A 波形メモリデータ
    0x9B,   // 0x2B      0xFF3B 0x400009B 波形メモリデータ
    0x9C,   // 0x2C      0xFF3C 0x400009C 波形メモリデータ
    0x9D,   // 0x2D      0xFF3D 0x400009D 波形メモリデータ
    0x9E,   // 0x2E      0xFF3E 0x400009E 波形メモリデータ
    0x9F,   // 0x2F      0xFF3F 0x400009F 波形メモリデータ
};

static void write_wreg_cmd(FILE* fp, uint8_t addr, uint8_t value)
{
    if (!_vgm_regs[addr]) {
        printf("Detected an invalid register address (0x%02X)\n", addr);
        exit(-1);
    }
    uint8_t out[2];
    out[0] = _vgm_regs[addr];
    /*
    if (0x73 == out[0]) {
        puts("set msb of NR32");
        value |= 0x80;
    }
    */
    out[1] = value;
    fwrite(out, 1, 2, fp);
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        puts("usage: vgm2psg input.vgm output.psg");
        return 1;
    }
    FILE* fp = fopen(argv[1],"rb");
    if (!fp) {
        puts("File not found.");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int size = (int)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (size < 0x100) {
        fclose(fp);
        puts("Invalid size.");
        return -1;
    }
    uint8_t* data = (uint8_t*)malloc(size);
    if (size != fread(data, 1, size, fp)) {
        fclose(fp);
        puts("File read error.");
        return -1;
    }
    fclose(fp);

    uint32_t version;
    memcpy(&version, &data[0x08], 4);
    printf("- version = %X\n", version);

    uint32_t loop;
    memcpy(&loop, &data[0x1C], 4);
    printf("- loop offset = %u (0x%X)\n", loop, loop);

    uint32_t start;
    memcpy(&start, &data[0x34], 4);
    start += 0x40;
    printf("- data offset = %u (0x%X)\n", start, start);

    uint32_t clock;
    memcpy(&clock, &data[0x80], 4);
    printf("- GB clock = %u\n", clock);
    if (clock != 4194304) {
        puts("Invalid clock.");
        return -1;
    }

    printf("- Interval type: %d\n", data[0x24]);
    if (60 != data[0x24]) {
        puts("Unsupported type (60=NTSC only)");
        return -1;
    }

    fp = fopen(argv[2], "wb");
    if (!fp) {
        puts("Output file open failed");
        return -1;
    }

    const char* eyecatch = "PSG";
    fwrite(eyecatch, 1, 4, fp);

    uint8_t cmd;
    uint32_t offset = start;
    uint32_t psg_offset = 4;
    uint32_t psg_loop = 0;
    uint64_t samples = 0;

    int usage[256];
    uint8_t zero[1] = { 0 };
    uint8_t eof[1] = { 0xFF };
    memset(usage, 0, sizeof(usage));
    while (1) {
        psg_loop = offset == loop ? psg_offset : psg_loop;
        cmd = data[offset++];
        while (cmd == 0xB3) {
            uint8_t addr = data[offset++];
            uint8_t value = data[offset++];
            usage[addr]++;
            write_wreg_cmd(fp, addr, value);
            psg_offset += 2;
            psg_loop = offset == loop ? psg_offset : psg_loop;
            cmd = data[offset++];
        }
        switch (cmd) {
            case 0x62: {
                samples += 735;
                fwrite(zero, 1, 1, fp);
                psg_offset += 1;
                break;
            }
            case 0x66: {
                fwrite(eof, 1, 1, fp);
                fwrite(&psg_loop, 1, 4, fp);
                printf("- psg_loop = %u (0x%X)\n", psg_loop, psg_loop);
                if (loop && !psg_loop) {
                    puts("psg_loop has not set.");
                    exit(-1);
                }
                printf("- total samples = %lluHz (%llusec)\n", samples, samples / 44100);
                /*
                for (int i = 0; i < 256; i++) {
                    if (usage[i]) {
                        printf("- REG[%02X] ... %d times\n", i, usage[i]);
                    }
                }
                */
                fclose(fp);
                exit(0);
            }
            default:
                printf("Detected an unsupported command: 0x%02X\n", cmd);
                exit(-1);
        }
    }

    return 0;
}

