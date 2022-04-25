#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res-extract.txt
git clone "$REPOSRC" sdsl-lite 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)
cd sdsl-lite/build

#git checkout benchmark_our_hybrid
./build.sh
cd ../benchmark/indexing_extract
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res-extract.txt

cd ../../../

#python3 visual_sdsl_extract.py