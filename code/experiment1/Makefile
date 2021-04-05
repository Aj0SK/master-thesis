CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -march=native -I ~/include -L ~/lib 
MY = -I../shared
SDSL = -lsdsl -ldivsufsort -ldivsufsort64
BENCHMARK = -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
BUILD = build

all: prepare rrr_30 table benchmark

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

$(BUILD)/rrr_30.out: rrr_30.cpp ../shared/*.h
	$(CXX) $(CXXFLAGS) $(MY) $< $(BENCHMARK) -o $@

rrr_30: $(BUILD)/rrr_30.out
	$<
