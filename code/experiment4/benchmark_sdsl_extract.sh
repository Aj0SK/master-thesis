#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res1-extract.txt res2-extract.txt
git clone "$REPOSRC" sdsl-old 2> /dev/null || (cd "sdsl-old" ; git pull ; cd ..)
cd sdsl-old/build

git checkout benchmark_original
./build.sh
cd ../benchmark/indexing_extract
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res1-extract.txt

cd ../../../
git clone "$REPOSRC" sdsl-new 2> /dev/null || (cd "sdsl-new" ; git pull ; cd ..)
cd sdsl-new/build

git checkout benchmark_our
./build.sh
cd ../benchmark/indexing_extract
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res2-extract.txt

cd ../../../

python3 benchmark_sdsl_extract_visual.py