
TARGETS += rawdump mtp2png mtp2tiles mtp256 addmoney movexy memdumpbin2png memdumpbinsim framebuffer2png hexdiff setbytes save2png
TARGETS += vga_palette.h svg_palette.svg
TARGETS += $(shell ls -1 ../*.MTP | sed "s/\.MTP/\.png/g" | sed "s~../~~g" | grep -v MAP12 | grep -v MAP13 | grep -v MAP15)
TARGETS += $(shell ls -1 ../*.MTP | sed "s/\.MTP/-tiles\.png/g" | sed "s~../~~g" | grep -v MAP12 | grep -v MAP13 | grep -v MAP15 | grep -v MAP14)
TARGETS += memdumpbin-sim16.png memdumpbin-sim256.png memdumpbin-sim0.png
TARGETS += svg_palette.svg
TARGETS += $(shell if [ -f MEMDUMP.BIN ]; then echo "MEMDUMP.BIN.fb.png MEMDUMP.BIN.full.png"; fi)

all: $(TARGETS)

CXXFLAGS = -std=c++17

save2png: save2png.cpp

rawdump: rawdump.cpp

setbytes: setbytes.cpp

mtp2png.cpp: vga_palette.h
mtp2png: mtp2png.cpp

mtp2tiles: mtp2tiles.cpp

mtp256: mtp256.cpp

vga_palette.h: vgapalette.sh
	./vgapalette.sh > vga_palette.h

svg_palette.svg: svgpalette.sh
	./svgpalette.sh > svg_palette.svg

memdumpbin2png.cpp: vga_palette.h
memdumpbin2png: memdumpbin2png.cpp

framebuffer2png.cpp: vga_palette.h
framebuffer2png: framebuffer2png.cpp

addmoney: addmoney.cpp

%.png: ../%.MTP mtp2png
	./mtp2png $< $@

%-tiles.png: ../%.MTP mtp2tiles ../tile-capture-raw/crop/tile-255.png
	./mtp2tiles $< $@

../tile-capture-raw/crop/tile-255.png: ../tile-capture-raw/*.png ../tile-capture-raw/crop-screenshots.sh
	../tile-capture-raw/crop-screenshots.sh

memdumpbin-%.png: memdumpbin-%.bin memdumpbin2png
	./memdumpbin2png $< 320 $@

MEMDUMP.BIN.full.png: memdumpbin2png MEMDUMP.BIN
	./memdumpbin2png MEMDUMP.BIN 320 MEMDUMP.BIN.full.png

MEMDUMP.BIN.fb.png: framebuffer2png MEMDUMP.BIN
	./framebuffer2png MEMDUMP.BIN MEMDUMP.BIN.fb.png

memdumpbinsim: memdumpbinsim.cpp

memdumpbin-sim%.bin: memdumpbinsim
	./memdumpbinsim $(subst memdumpbin-sim,,$(subst .bin,,$*)) > $@

memdumpbin-sim%.png: memdumpbin-sim%.bin memdumpbin2png
	./memdumpbin2png $< 320 $@

clean:
	rm -rf $(TARGETS)
