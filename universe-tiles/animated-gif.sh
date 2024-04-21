#!/bin/bash

# Row that goes through two cities
Y="y060A"
set -xv
convert save-${Y}-x*.png ../docs/animated-${Y}.gif
