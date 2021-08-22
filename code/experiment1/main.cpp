// https://github.com/simongog/sdsl-lite/blob/master/examples/bit-vector.cpp
#include <algorithm>
#include <benchmark/benchmark.h>
#include <cmath>
#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <vector>

using namespace sdsl;

using std::cerr;
using std::cout;
using std::vector;

bit_vector get_bit_vector(size_t n, size_t density)
{
  vector<bool> vec;
  size_t ones_count = n * density / 100ull;
  for (size_t i = 0; i < ones_count; ++i)
    vec.push_back(true);
  for (size_t i = 0; i < n - ones_count; ++i)
    vec.push_back(false);

  random_shuffle(vec.begin(), vec.end());

  bit_vector bv(n, 0);
  for (size_t i = 0; i < n; ++i)
    if (vec[i])
      bv[i] = 1;
  return bv;
}

vector<bool> generate_random_bits(size_t n)
{
  // TODO: meh, asi to chce byt static
  std::mt19937 rnd{std::random_device()()};
  std::bernoulli_distribution dist(0.5);
  std::vector<bool> bits;
  std::generate_n(std::back_inserter(bits), n, [&] { return dist(rnd); });
  return bits;
}

template <size_t kN, size_t kDensity, size_t kB, size_t kSB, bool random,
          size_t iterations = 1'000>
static void BM_Rank(benchmark::State& state)
{
  auto bv = get_bit_vector(kN, kDensity);
  rrr_vector<kB, int_vector<>, kSB> rrr(bv);
  typename rrr_vector<kB>::rank_1_type rank_rrr(&rrr);

  double ratio = 8.0 * size_in_bytes(rrr) / static_cast<double>(kN);
  state.counters["Space"] = ratio;

  srand(0);

  vector<size_t> indexes;
  indexes.reserve(iterations);
  for (size_t i = 0; i < iterations; ++i)
  {
    if constexpr (random)
    {
      indexes.push_back(rand() % kN);
    }
    else
    {
      indexes.push_back(i);
    }
  }

  for (auto _ : state)
  {
    for (auto index : indexes)
    {
      auto x = rank_rrr(index);
      benchmark::DoNotOptimize(x);
    }
  }
}

template <size_t kN, size_t kDensity, size_t kB, size_t kSB, bool random,
          size_t iterations = 1'000>
static void BM_Select(benchmark::State& state)
{
  auto bv = get_bit_vector(kN, kDensity);
  rrr_vector<kB, int_vector<>, kSB> rrr(bv);
  typename rrr_vector<kB>::select_1_type select_rrr(&rrr);

  double ratio = 8.0 * size_in_bytes(rrr) / static_cast<double>(kN);
  state.counters["Space"] = ratio;

  srand(0);

  vector<size_t> indexes;
  indexes.reserve(iterations);
  for (size_t i = 0; i < iterations; ++i)
  {
    if constexpr (random)
    {
      indexes.push_back(rand() % kN);
    }
    else
    {
      indexes.push_back(i);
    }
  }

  for (auto _ : state)
  {
    for (auto index : indexes)
    {
      auto x = select_rrr(index);
      benchmark::DoNotOptimize(x);
    }
  }
}

///// Select

BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 15, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 15, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 63, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 63, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 127, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 127, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 255, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 5, 255, 32, false);

BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 15, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 15, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 63, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 63, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 127, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 127, 32, false);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 255, 32, true);
BENCHMARK_TEMPLATE(BM_Select, 1 << 24, 10, 255, 32, false);

///// Rank

BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 15, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 15, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 63, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 63, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 127, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 127, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 255, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 5, 255, 32, false);

BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 15, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 15, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 63, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 63, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 127, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 127, 32, false);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 255, 32, true);
BENCHMARK_TEMPLATE(BM_Rank, 1 << 24, 10, 255, 32, false);

// BENCHMARK_MAIN();

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();
}
