CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native -I ~/include -L ~/lib
MY = -I../shared
SDSL = -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
BUILD = build

all: prepare rrr table benchmark

prepare:
	mkdir -p $(BUILD)

$(BUILD)/main.out: main.cpp ../shared/*.h
	$(CXX) $(CXXFLAGS) $< $(BENCHMARK) $(SDSL) -o $@

benchmark: $(BUILD)/main.out
	$< > out.txt

$(BUILD)/table.out: table.cpp ../shared/*.h
	$(CXX) $(CXXFLAGS) $(MY) $< $(BENCHMARK) -o $@

table: $(BUILD)/table.out
	$<

$(BUILD)/rrr.out: rrr.cpp ../shared/*.h
	$(CXX) $(CXXFLAGS) $(MY) $< $(BENCHMARK) $(SDSL) -o $@

rrr: $(BUILD)/rrr.out
	$<
