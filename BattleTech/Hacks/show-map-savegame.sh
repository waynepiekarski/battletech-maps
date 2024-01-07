#!/bin/bash

echo "Enabling map visibility in $1"
set -xv
`dirname $0`/setbytes $1 0200 0CFC FF
