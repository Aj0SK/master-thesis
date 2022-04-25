#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res-count.txt
git clone "$REPOSRC" sdsl-lite 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)
cd sdsl-lite/build

#git checkout benchmark_our_hybrid
./build.sh
cd ../benchmark/indexing_count
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res-count.txt

cd ../../../

python3 visual_sdsl_count.py