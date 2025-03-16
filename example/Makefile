all:
	cd .. && find src | grep -e "\.h" -e "\.c" >./example/.gitignore
	cd ../tools && make
	make build_data
	cp -pr ../src/* src
	make -f Makefile.GBA
	mgba example.gba

clean:
	rm -rf build
	rm -rf data
	rm -f *.elf
	rm -f *.sav
	rm -f ./src/vdp.c
	rm -f ./src/vdp.h

build_data: data data/palette.dat data/font.dat data/sfx.pcm

data:
	mkdir data

data/palette.dat: image/font.bmp
	../tools/bmp2pal/bmp2pal $< $@

data/font.dat: image/font.bmp
	../tools/bmp2chr/bmp2chr $< $@

data/sfx.pcm: image/sfx.wav
	../tools/wav2pcm/wav2pcm $< $@
