#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f res1.txt res2.txt benchmark_sdsl_new_method.png
git clone "$REPOSRC" 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)
cd sdsl-lite/build

git checkout benchmark_original
./build.sh
cd ../benchmark/rrr_vector
make clean-build
make clean_results
make timing
cp ./results/all.txt ../../../res1.txt

cd ../../build

git checkout benchmark_our
./build.sh
cd ../benchmark/rrr_vector
make clean-build
make clean_results
make timing
cp ./results/all.txt ../../../res2.txt

cd ../../../

python3 sdsl_helper.py