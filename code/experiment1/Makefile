CXX = g++
CXXFLAGS = -std=c++17 -O2 -DNDEBUG -I ~/include -L ~/lib
SDSL = -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread

main.out: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp $(BENCHMARK) $(SDSL) -o main.out