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
  # The map scroll has significant overlap, so need to crop or offset the images to fit
  if [[ $Y == 0 ]]; then
      CROPY=64
  else
      CROPY=128
  fi
  if [[ $X == 6 ]]; then
      CROPX=320
      OFSX=56
  else
      CROPX=128
      OFSX=0
  fi
  convert -crop ${CROPX}x${CROPY}+${OFSX}+0 ${each} crop/map-y${Y}-x${X}.png
  let "X+=1"
  if [[ $X == 7 ]]; then
    X=0
    let "Y+=1"
  fi
done

# Combine into a single image
montage -mode concatenate -tile 7x crop/map-*.png crop/map-combined.png
