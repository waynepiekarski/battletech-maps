#!/bin/bash

# Takes the output from two LOGL <n> commands and simplifies it so they can be diffed between runs
if [[ "$2" == "" ]]; then
  echo "Need to provide LOGCPU.TXT file from dosbox-debug LOGL command"
  exit 1
fi

set -xve
cat "$1" | awk -F'EAX' '{ print $1 }' | cut -c1-47,66- > .tmp.LOGL.1
cat "$2" | awk -F'EAX' '{ print $1 }' | cut -c1-47,66- > .tmp.LOGL.2
printf "%-70s  %-70s\n" "$1" "$2"
diff --width=140 --side-by-side .tmp.LOGL.1 .tmp.LOGL.2
rm -f .tmp.LOGL.1 .tmp.LOGL.2
