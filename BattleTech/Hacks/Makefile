TARGETS = rawdump mtp2png addmoney memdumpbin2png memdumpbinsim
TARGETS += $(shell ls -1 ../*.MTP | sed "s/\.MTP/\.png/g" | sed "s~../~~g" | grep -v MAP12 | grep -v MAP13 | grep -v MAP15)
TARGETS += $(shell ls -1 memdumpbin-*.bin | sed "s/\.bin/\.png/g")
TARGETS += memdumpbin-sim.png

all: $(TARGETS)

rawdump: rawdump.cpp

mtp2png: mtp2png.cpp

memdumpbin2png: memdumpbin2png.cpp

addmoney: addmoney.cpp

%.png: ../%.MTP
	./mtp2png $< $@

memdumpbin-%.png: memdumpbin-%.bin memdumpbin2png
	./memdumpbin2png $< $@

memdumpbinsim: memdumpbinsim.cpp

memdumpbin-sim.bin: memdumpbinsim
	./memdumpbinsim > memdumpbin-sim.bin

clean:
	rm -rf $(TARGETS)
