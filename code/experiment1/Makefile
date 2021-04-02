CXX = g++
CXXFLAGS = -std=c++17 -O2 -march=native -I ~/include -L ~/lib
SDSL = -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
BUILD = build

all: prepare benchmark

prepare:
	mkdir -p $(BUILD)

$(BUILD)/main.out: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp $(BENCHMARK) $(SDSL) -o $(BUILD)/main.out

benchmark: $(BUILD)/main.out
	$(BUILD)/main.out > out.txt