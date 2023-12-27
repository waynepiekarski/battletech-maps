#!/bin/bash

set -xv
cut -c9- $1 | sort
