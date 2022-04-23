CXX := g++
CXXFLAGS := -std=c++17 -O3 -march=native -DNDEBUG -Wall -Wextra -Wfatal-errors -pedantic -Wshadow -Wcast-align -Wnon-virtual-dtor -I ~/include -L ~/lib
CXXFLAGS_DEBUG := -fsanitize=address -std=c++17 -g -Wall -Wextra -Wfatal-errors -pedantic -Wshadow -Wcast-align -Wnon-virtual-dtor -I ~/include -L ~/lib
SDSL := -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread

all: sdsl_rrr_vector_test

clear:
	rm -f *.out

sdsl_block_type_stat: sdsl_block_type_stat.out
	./$<

sdsl_block_type_stat.out: sdsl_block_type_stat.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

sdsl_rrr_vector_test: sdsl_rrr_vector_test.out before_benchmark 
	./$<

sdsl_rrr_vector_test.out: sdsl_rrr_vector_test.cpp
	$(CXX) $(CXXFLAGS) $< $(SDSL) $(BENCHMARK) -o $@

wt_stats.out: wt_stats.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(SDSL)

before_benchmark:
	sudo cpupower frequency-set --governor performance

after_benchmark:
	sudo cpupower frequency-set --governor powersave
