
TARGETS += rawdump mtp2png addmoney memdumpbin2png memdumpbinsim framebuffer2png
TARGETS += vga_palette.h btech_palette.h svg_palette.svg
TARGETS += $(shell ls -1 ../*.MTP | sed "s/\.MTP/\.png/g" | sed "s~../~~g" | grep -v MAP12 | grep -v MAP13 | grep -v MAP15)
TARGETS += memdumpbin-sim16.png memdumpbin-sim256.png memdumpbin-sim0.png
TARGETS += svg_palette.svg
TARGETS += $(shell if [ -f MEMDUMP.BIN ]; then echo "MEMDUMP.BIN.fb.png MEMDUMP.BIN.full.png"; fi)

all: $(TARGETS)

rawdump: rawdump.cpp

mtp2png: mtp2png.cpp vga_palette.h

vga_palette.h: vgapalette.sh
	./vgapalette.sh > vga_palette.h

btech_palette.h: btechpalette.sh
	./btechpalette.sh > btech_palette.h

svg_palette.svg: svgpalette.sh
	./svgpalette.sh > svg_palette.svg

memdumpbin2png: memdumpbin2png.cpp vga_palette.h btech_palette.h

framebuffer2png: framebuffer2png.cpp vga_palette.h btech_palette.h

addmoney: addmoney.cpp

%.png: ../%.MTP mtp2png
	./mtp2png $< $@

memdumpbin-%.png: memdumpbin-%.bin memdumpbin2png
	./memdumpbin2png $< $@

MEMDUMP.BIN.full.png: memdumpbin2png
	./memdumpbin2png MEMDUMP.BIN MEMDUMP.BIN.full.png

MEMDUMP.BIN.fb.png: framebuffer2png
	./framebuffer2png MEMDUMP.BIN MEMDUMP.BIN.fb.png

memdumpbinsim: memdumpbinsim.cpp

memdumpbin-sim%.bin: memdumpbinsim
	./memdumpbinsim $(subst memdumpbin-sim,,$(subst .bin,,$*)) > $@ $*

memdumpbin-sim%.png: memdumpbin-sim%.bin memdumpbin2png
	./memdumpbin2png $< $@

clean:
	rm -rf $(TARGETS)
