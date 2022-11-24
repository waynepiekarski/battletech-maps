#!/bin/bash

set -xv
dosbox `dirname $0`/BattleTech/BTECH.EXE
# strace dosbox `dirname $0`/BattleTech/BTECH.EXE 2>&1 | grep "^stat("
