CXX = g++
CXXFLAGS = -std=c++17 -O2 -march=native -I ~/include -L ~/lib
SDSL = -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
BUILD = build

all: prepare table benchmark

prepare:
	mkdir -p $(BUILD)

$(BUILD)/main.out: main.cpp
	$(CXX) $(CXXFLAGS) $< $(BENCHMARK) $(SDSL) -o $@

benchmark: $(BUILD)/main.out
	$< > out.txt

$(BUILD)/table.out: table.cpp
	$(CXX) $(CXXFLAGS) $< $(BENCHMARK) -o $@

table: $(BUILD)/table.out
	$<
