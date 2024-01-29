#!/bin/bash

# The hex values do not sort in image viewers, so create symlinks with decimal values
cd `dirname $0`
rm -rf DECIMAL
mkdir DECIMAL
for each in `ls -1 save-y*-x*.png`; do
  YHEX="0x`echo $each | cut -c7-10`"
  XHEX="0x`echo $each | cut -c13-16`"
  YDEC=$((YHEX))
  XDEC=$((XHEX))
  printf "%s X=%s Y=%s %.5d %.5d\n" $each $XHEX $YHEX $XDEC $YDEC
  OUT=`printf DECIMAL/save-y%.5d-x%.5d-%s-%s.png $YDEC $XDEC $YHEX $XHEX`
  ln -s ../$each $OUT
done
