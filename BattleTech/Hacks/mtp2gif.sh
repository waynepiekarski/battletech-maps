#!/bin/bash

if [[ "$2" == "" ]]; then
  echo "$0 <mtp_input> <output_image>"
  exit 1
fi
set -xe
$(dirname $0)/mtp2raw $1 > temp.raw
rawtopgm 64 64 temp.raw > temp.pgm
convert temp.pgm $2
rm -f temp.pgm temp.raw
