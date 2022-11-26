all: rawdump mtp2png images addmoney

rawdump: rawdump.cpp

mtp2png: mtp2png.cpp

addmoney: addmoney.cpp

images: mtp2png
	./mtp2png ../MAP1.MTP out-map1.png
	./mtp2png ../MAP2.MTP out-map2.png
	./mtp2png ../MAP11.MTP out-map11.png
	./mtp2png ../MAP14.MTP out-map14.png
	./mtp2png ../MAP3.MTP out-map3.png
	./mtp2png ../MAP4.MTP out-map4.png
	./mtp2png ../MAP5.MTP out-map5.png
	./mtp2png ../MAP6.MTP out-map6.png
	./mtp2png ../MAP7.MTP out-map7.png
	./mtp2png ../MAP8.MTP out-map8.png
	./mtp2png ../MAP9.MTP out-map9.png
	./mtp2png ../MAP10.MTP out-map10.png
