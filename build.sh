#!/bin/bash
set -xe
mkdir -p build/
FLAGS="-ggdb -Wall -Wextra"
INCLUDE="-Icommon -Igm"
gcc $INCLUDE $FLAGS ./gm/gme.c ./gm/gm.c -o ./build/gme
gcc $INCLUDE $FLAGS ./gasm/gasm.c ./gasm/compiler.c -o ./build/gasm
