#include "bin_helper.h"
#include "bit_sequence.h"

#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <iostream>
#include <sdsl/rrr_helper.hpp>
#include <sdsl/rrr_vector_15.hpp>
#include <vector>

using std::cout;
using std::min;
using std::next_permutation;
using std::pair;
using std::vector;

constexpr bool kTest = true;
constexpr size_t kMaxBinN = 64;

uint32_t f(uint32_t k, uint32_t index)
{
  const uint32_t threshold = RRR30_Helper::nCrArr[30][k];
  uint32_t to_or = 0;
  if (index >= threshold)
  {
    --k;
    index -= threshold;
    to_or = 1 << 31;
  }
  return to_or | RRR30_Helper::f_simd(k, index);
}

pair<uint32_t, uint32_t> decode(uint32_t x)
{
  uint32_t k = __builtin_popcount(x);

  if (x & (1 << 31))
  {
    uint32_t new_x = x & (~(1 << 31));
    return {k,
            RRR30_Helper::nCrArr[30][k] + RRR30_Helper::decode(new_x).second};
  }
  else
  {
    return {k, RRR30_Helper::decode(x).second};
  }
}

////////////////////////////////////////////////////////////////////////////////
// Benchmarks

vector<pair<uint32_t, uint32_t>> get_test(int N)
{
  srand(2104);
  vector<pair<uint32_t, uint32_t>> test;
  test.reserve(10'000);
  for (size_t i = 0; i < 10'000; ++i)
  {
    uint32_t k = 1 + rand() % (N - 1);
    uint32_t index = rand() % RRR30_Helper::nCrArr[N][k];
    test.push_back({k, index});
  }
  return test;
}

static void BM_SDSL15(benchmark::State& state)
{
  auto test = get_test(15);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = binomial15::nr_to_bin(k, index);
      benchmark::DoNotOptimize(x);
    }
  }
}

static void BM_SDSL30_LINEAR_SIMD(benchmark::State& state)
{
  auto test = get_test(30);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = RRR30_Helper::f_simd(k, index);
      benchmark::DoNotOptimize(x);
    }
  }
}

static void BM_SDSL31_LINEAR_SIMD(benchmark::State& state)
{
  auto test = get_test(31);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = f(k, index);
      benchmark::DoNotOptimize(x);
    }
  }
}

template <typename T, size_t N> static void BM_FUNC(benchmark::State& state)
{
  auto test = get_test(N);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = sdsl::rrr_helper<N>::decode_bit(k, index, N - 1);
      // auto x = get_ith_in_lexicographic_sdsl<T>(N, k, index + 1);
      benchmark::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_SDSL15);
BENCHMARK(BM_SDSL30_LINEAR_SIMD);
BENCHMARK(BM_SDSL31_LINEAR_SIMD);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 31);
BENCHMARK_TEMPLATE(BM_FUNC, uint64_t, 63);

pair<uint16_t, uint16_t> divide(uint32_t x) { return {x >> 16, x}; }

int main(int argc, char** argv)
{
  if constexpr (kTest)
  {
    srand(2104);

    for (size_t test_num = 0; test_num < 100'000; ++test_num)
    {
      uint32_t k = rand() % 32;
      uint32_t index = rand() % RRR30_Helper::nCrArr[31][k];
      uint32_t res = f(k, index);
      auto [ret_k, ret_index] = decode(res);
      if (ret_k != k || ret_index != index)
      {
        cerr << "Problem!"
             << "\n";
        exit(1);
      }
    }
  }

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}