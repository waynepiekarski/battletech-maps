all: rawdump mtp2png images

rawdump: rawdump.cpp

mtp2png: mtp2png.cpp

images: mtp2png
	./mtp2png ../MAP1.MTP out-map1.png
	./mtp2png ../MAP2.MTP out-map2.png
	./mtp2png ../MAP11.MTP out-map11.png
	./mtp2png ../MAP14.MTP out-map14.png
