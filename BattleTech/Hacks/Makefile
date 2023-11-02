TARGETS = rawdump mtp2png addmoney memdumpbin2png memdumpbinsim vga_palette.h btech_palette.h svg_palette.svg
TARGETS += $(shell ls -1 ../*.MTP | sed "s/\.MTP/\.png/g" | sed "s~../~~g" | grep -v MAP12 | grep -v MAP13 | grep -v MAP15)
TARGETS += memdumpbin-sim16.png memdumpbin-sim256.png memdumpbin-sim0.png
TARGETS += svg_palette.svg
TARGETS += $(shell if [ -f MEMDUMP.BIN ]; then echo "MEMDUMP.BIN.png"; fi)

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

addmoney: addmoney.cpp

%.png: ../%.MTP mtp2png
	./mtp2png $< $@

memdumpbin-%.png: memdumpbin-%.bin memdumpbin2png
	./memdumpbin2png $< $@

MEMDUMP.BIN.png: memdumpbin2png
	./memdumpbin2png MEMDUMP.BIN MEMDUMP.BIN.png

memdumpbinsim: memdumpbinsim.cpp

memdumpbin-sim%.bin: memdumpbinsim
	./memdumpbinsim $(subst memdumpbin-sim,,$(subst .bin,,$*)) > $@ $*

memdumpbin-sim%.png: memdumpbin-sim%.bin memdumpbin2png
	./memdumpbin2png $< $@

clean:
	rm -rf $(TARGETS)
