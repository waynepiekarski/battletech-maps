#!/bin/bash

# Takes the output from the LOGL <n> command and simplifies it to make a heatmap of what instructions ran how many times during that time
if [[ "$1" == "" ]]; then
  echo "Need to provide LOGCPU.TXT file from dosbox-debug LOGL command"
  exit 1
fi

set -xv
cat "$1" | awk -F'EAX' '{ print $1 }' | cut -c1-47,66- | sort | uniq -c
