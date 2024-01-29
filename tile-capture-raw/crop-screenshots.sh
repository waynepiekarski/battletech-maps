#!/bin/bash

set -e
cd `dirname $0`
rm -rf crop
mkdir crop

set -xv
# Remove game borders from the screenshots and crop out the character
for each in `ls -1 noblock_00[0,2,4,6].png`; do
    convert -crop 32x128+104+0 $each crop/$each
done
for each in `ls -1 noblock_00[1,3,5,7].png`; do
    convert -crop 96x128+112+0 $each crop/$each
done

# Combine proper tiles with the character cropped out
montage -mode concatenate crop/noblock_000.png crop/noblock_001.png crop/block-0.png
montage -mode concatenate crop/noblock_002.png crop/noblock_003.png crop/block-1.png
montage -mode concatenate crop/noblock_004.png crop/noblock_005.png crop/block-2.png
montage -mode concatenate crop/noblock_006.png crop/noblock_007.png crop/block-3.png

# Now break up into actual artwork tiles
set +xv
for BLOCK in $(seq 0 3); do
    for y in $(seq 0 7); do
	for x in $(seq 0 7); do
	    XOFS=$((16*$x))
	    YOFS=$((16*$y))
	    TILE=$((8*$y+$x+64*$BLOCK))
	    NAME=`printf crop/tile-%03d.png ${TILE}`
	    echo "${BLOCK} @ $x $y = +${XOFS}+${YOFS} = ${TILE} --> ${NAME}"
	    convert -crop 16x16+${XOFS}+${YOFS} crop/block-${BLOCK}.png ${NAME}
	done
    done
done

# Reconstitute back into a single image to check it is was all done right
set -xv
montage -mode concatenate -tile 16x crop/tile-*.png crop/combined-16x16.png
montage -mode concatenate -tile 8x crop/tile-*.png crop/combined-8x32.png
