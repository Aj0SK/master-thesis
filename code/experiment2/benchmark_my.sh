#!/bin/bash
REPOSRC=https://github.com/Aj0SK/sdsl-lite.git

rm -f old_output.txt new_output.txt vysledky_nase.png sdsl_rrr_vector_test.out
git clone "$REPOSRC" 2> /dev/null || (cd "sdsl-lite" ; git pull ; cd ..)

cd sdsl-lite
git checkout benchmark_original && ./install.sh
cd ..
touch sdsl_rrr_vector_test.cpp
make sdsl_rrr_vector_test.out
./sdsl_rrr_vector_test.out --benchmark_format=json > old_output.txt

cd sdsl-lite
git checkout refactor && ./install.sh
cd ..
touch sdsl_rrr_vector_test.cpp
make sdsl_rrr_vector_test.out
./sdsl_rrr_vector_test.out --benchmark_format=json > new_output.txt

python3 visual.py