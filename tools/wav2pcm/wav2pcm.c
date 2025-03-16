#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        puts("usage: wav2pcm input.wav output.pcm");
        return 1;
    }

    printf("Loading: %s\n", argv[1]);
    FILE* fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("File open failed: %s\n", argv[1]);
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* wav = (char*)malloc(size);
    if (size != fread(wav, 1, size, fp)) {
        fclose(fp);
        free(wav);
        printf("File read failed: %s\n", argv[1]);
        exit(-1);
    }
    fclose(fp);

    if (0 != memcmp(wav, "RIFF", 4)) {
        puts("Invalid format: RIFF not exist");
        exit(-1);
    }
    wav += 4;
    size -= 4;

    int n;
    unsigned short s;

    memcpy(&n, wav, 4);
    if (n != size - 4) {
        puts("Invalid format: SIZE");
        exit(-1);
    }
    wav += 4;
    size -= 4;

    if (0 != memcmp(wav, "WAVE", 4)) {
        puts("Invalid format: not WAVE format");
        exit(-1);
    }
    wav += 4;
    size -= 4;

    // check chunks
    while (1) {
        if (0 == memcmp(wav, "fmt ", 4)) {
            wav += 4;
            size -= 4;
            memcpy(&n, wav, 4);
            if (n != 16) {
                puts("Invalid format: not linear format");
                exit(-1);
            }
            wav += 4;
            size -= 4;

            memcpy(&s, wav, 2);
            if (s != 0x0001) {
                puts("Invalid format: unsupported compress type");
                exit(-1);
            }
            wav += 2;
            size -= 2;

            short ch;
            memcpy(&ch, wav, 2);
            wav += 2;
            size -= 2;

            int rate;
            memcpy(&rate, wav, 4);
            wav += 4;
            size -= 4;

            int bps;
            memcpy(&bps, wav, 4);
            wav += 4;
            size -= 4;

            short bs;
            memcpy(&bs, wav, 2);
            wav += 2;
            size -= 2;

            short bits;
            memcpy(&bits, wav, 2);
            wav += 2;
            size -= 2;

            printf("- PCM Format: %dHz %dbits %dch (%d bytes/sec, %d bytes/sample)\n", rate, bits, ch, bps, bs);
            if (rate != 44100 || bits != 16 || ch != 1) {
                puts("Detected unsupported format (44100Hz/16bits/1ch only)");
                exit(-1);
            }
        } else if (0 == memcmp(wav, "LIST", 4)) {
            wav += 4;
            size -= 4;
            memcpy(&n, wav, 4);
            wav += 4 + n;
            size -= 4 + n;
        } else if (0 == memcmp(wav, "data", 4)) {
            wav += 4;
            size -= 4;
            break;
        } else {
            printf("Detected an invalid chunk: %c%c%c%c\n", wav[0], wav[1], wav[2], wav[3]);
            exit(-1);
        }
    }

    // validate data chunk size
    memcpy(&n, wav, 4);
    if (n != size - 4) {
        puts("Invalid format: invalid sub chunk size");
        exit(-1);
    }
    printf("- VGS(44kHz/16bit) PCM size: %d bytes\n", n);
    wav += 4;
    size -= 4;

    // 44100Hz -> 16384Hz & 16bit -> 8bit
    int16_t* wav16 = (int16_t*)wav;
    double samples = size / 2;
    int samples8 = 0;
    int8_t* wav8 = (int8_t*)malloc(size / 2);
    for (double s16 = 0.0; s16 < size / 2; s16 += 44100.0 / 16384.0) {
        wav8[samples8++] = (int8_t)(wav16[(int)s16] / 256);
    } 
    printf("- GBA(16kHz/ 8bit) PCM size: %d bytes\n", samples8);

    // write
    printf("Writing: %s (%dbytes)\n", argv[2], samples8 + 1024);
    fp = fopen(argv[2], "wb");
    if (!fp) {
        printf("File open failed: %s\n", argv[1]);
        exit(-1);
    }
    if (samples8 != fwrite(wav8, 1, samples8, fp)) {
        printf("File write failed: %s\n", argv[1]);
        exit(-1);
    }
    // append empty buffer
    char empty[1024];
    memset(empty, 0, sizeof(empty));
    fwrite(empty, 1, sizeof(empty), fp);

    fclose(fp);
    return 0;
}