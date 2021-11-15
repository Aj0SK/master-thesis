#!/bin/bash
git clone https://github.com/Aj0SK/sdsl-lite.git
cd sdsl-lite/build

git checkout fix_rrr_benchmark
./build.sh
cd ../benchmark/rrr_vector
make timing
cp ./results/all.txt ../../../res1.txt

make clean-build
make clean-results
cd ../../build

git checkout fix_rrr_benchmark
./build.sh
cd ../benchmark/rrr_vector
make timing
cp ./results/all.txt ../../../res2.txt