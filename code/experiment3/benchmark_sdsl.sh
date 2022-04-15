#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -r -f vysledky_sdsl.png
git clone "$REPOSRC" 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)

cd sdsl-lite
git checkout benchmark_our_hybrid && ./install.sh
cd ./build/
./build.sh
cd ../benchmark/rrr_vector
make cleanall
make timing

cd ../../../

python3 visual_sdsl.py
