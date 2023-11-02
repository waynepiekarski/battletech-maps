#!/bin/bash

# ORIGIN: https://commons.wikimedia.org/w/index.php?title=User:Psychonaut/ipalette.sh&oldid=8607095

# Name   : ipalette.sh
# Author : Psychonaut
# Date   : 2007-11-19
# Licence: public domain
# Purpose: This bash script generates an SVG image of an indexed palette
# Usage  : Modify the variables in the "User-modifiable variables" section
#          to taste; then run the script.  The SVG image is sent to
#          standard output.

# User-modifiable variables
cols=16                       # Number of columns in grid
gridsize=32                   # Width of each grid square
cellsize=32                   # Width of each cell within a grid square
strokewidth=0                 # Stroke width
strokecolor="black"           # Stroke colour
palette=(
000000 0000AA 00AA00 00AAAA AA0000 AA00AA AA5500 AAAAAA 555555 5555FF
55FF55 55FFFF FF5555 FF55FF FFFF55 FFFFFF 000000 101010 202020 353535
454545 555555 656565 757575 8A8A8A 9A9A9A AAAAAA BABABA CACACA DFDFDF
EFEFEF FFFFFF 0000FF 4100FF 8200FF BE00FF FF00FF FF00BE FF0082 FF0041
FF0000 FF4100 FF8200 FFBE00 FFFF00 BEFF00 82FF00 41FF00 00FF00 00FF41
00FF82 00FFBE 00FFFF 00BEFF 0082FF 0041FF 8282FF 9E82FF BE82FF DF82FF
FF82FF FF82DF FF82BE FF829E FF8282 FF9E82 FFBE82 FFDF82 FFFF82 DFFF82
BEFF82 9EFF82 82FF82 82FF9E 82FFBE 82FFDF 82FFFF 82DFFF 82BEFF 829EFF
BABAFF CABAFF DFBAFF EFBAFF FFBAFF FFBAEF FFBADF FFBACA FFBABA FFCABA
FFDFBA FFEFBA FFFFBA EFFFBA DFFFBA CAFFBA BAFFBA BAFFCA BAFFDF BAFFEF
BAFFFF BAEFFF BADFFF BACAFF 000071 1C0071 390071 550071 710071 710055
710039 71001C 710000 711C00 713900 715500 717100 557100 397100 1C7100
007100 00711C 007139 007155 007171 005571 003971 001C71 393971 453971
553971 613971 713971 713961 713955 713945 713939 714539 715539 716139
717139 617139 557139 457139 397139 397145 397155 397161 397171 396171
395571 394571 515171 595171 615171 695171 715171 715169 715161 715159
715151 715951 716151 716951 717151 697151 617151 597151 517151 517159
517161 517169 517171 516971 516171 515971 000041 100041 200041 310041
410041 410031 410020 410010 410000 411000 412000 413100 414100 314100
204100 104100 004100 004110 004120 004131 004141 003141 002041 001041
202041 282041 312041 392041 412041 412039 412031 412028 412020 412820
413120 413920 414120 394120 314120 284120 204120 204128 204131 204139
204141 203941 203141 202841 2D2D41 312D41 352D41 3D2D41 412D41 412D3D
412D35 412D31 412D2D 41312D 41352D 413D2D 41412D 3D412D 35412D 31412D
2D412D 2D4131 2D4135 2D413D 2D4141 2D3D41 2D3541 2D3141 000000 000000
000000 000000 000000 000000 000000 000000
)

# Dependent variables
palettesize=${#palette[@]}
rows=$(( palettesize / cols ))

cat <<EOF
<?xml version="1.0"?>
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.0"
     width="$(( cols * gridsize ))"
     height="$(( rows * gridsize ))">
EOF

row=0
col=0
for (( i = 0; i < palettesize; i++ ))
  do
  cat <<EOF
      <rect width="$cellsize"
            height="$cellsize" 
            y="$((row * gridsize + (gridsize - cellsize) / 2))"
            x="$((col * gridsize + (gridsize - cellsize) / 2))" 
            style="fill:#${palette[$i]};
                   stroke-width:$strokewidth;
                   stroke:$strokecolor;" />
EOF
      if ((++col == cols))
      then
        col=0
        ((row++))
      fi
done

cat <<EOF
</svg>
EOF
