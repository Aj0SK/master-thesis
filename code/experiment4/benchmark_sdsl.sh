#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res1.txt res2.txt
git clone "$REPOSRC" 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)
cd sdsl-lite/build

git checkout benchmark_original
./build.sh
cd ../benchmark/indexing_count
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res1.txt

cd ../../build

git checkout refactor
./build.sh
cd ../benchmark/indexing_count
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res2.txt