all: rawdump mtp2raw images

rawdump: rawdump.cpp

mtp2raw: mtp2raw.cpp

images: mtp2raw
	./mtp2gif.sh ../MAP1.MTP out-map1.gif
	./mtp2gif.sh ../MAP2.MTP out-map2.gif
	./mtp2gif.sh ../MAP11.MTP out-map11.gif
	./mtp2gif.sh ../MAP14.MTP out-map14.gif
