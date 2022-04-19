#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res1-locate.txt res2-locate.txt
git clone "$REPOSRC" sdsl-old 2> /dev/null || (cd "sdsl-old" ; git pull ; cd ..)
cd sdsl-old/build

git checkout benchmark_original
./build.sh
cd ../benchmark/indexing_locate
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res1-locate.txt

cd ../../../
git clone "$REPOSRC" sdsl-new 2> /dev/null || (cd "sdsl-new" ; git pull ; cd ..)
cd sdsl-new/build

git checkout benchmark_our
./build.sh
cd ../benchmark/indexing_locate
make clean-build
make cleanresults
make timing
cp ./results/all.txt ../../../res2-locate.txt