#!/bin/bash

echo "Enabling map visibility in $1"
set -xv
echo "TODO: Seems to remove the Chameleon mech that I had"
`dirname $0`/setbytes $1 0010 0CFC FF
`dirname $0`/setbytes $1 0E40 0F44 FF
