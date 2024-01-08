#!/bin/bash

set -e
cd `dirname $0`
rm -rf crop
mkdir crop
X=0
Y=0
# Sort in y-x order so the filenames are correctly arranged in the file explorer
for each in `ls -1 btech_*.png`; do
  echo "Cropping ${each} to crop/map-y${Y}-x${X}.png"
  convert -crop 320x192+0+0 ${each} crop/map-y${Y}-x${X}.png
  let "X+=1"
  if [[ $X == 7 ]]; then
    X=0
    let "Y+=1"
  fi
done
