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
using std::next_permutation;
using std::pair;
using std::vector;

constexpr bool kTest = true;
constexpr size_t kMaxBinN = 64;

////////////////////////////////////////////////////////////////////////////////
// Benchmarks

vector<pair<int, int>> get_test(int N)
{
  srand(2104);
  vector<pair<int, int>> test;
  test.reserve(10'000);
  for (size_t i = 0; i < 10'000; ++i)
  {
    size_t k = 1 + rand() % (N - 1);
    size_t index = rand() % RRR30_Helper::nCrArr[N][k];
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

static void BM_SDSL30_LINEAR(benchmark::State& state)
{
  auto test = get_test(30);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = RRR30_Helper::f(k, index);
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

static void BM_SDSL30_BINARY(benchmark::State& state)
{
  auto test = get_test(30);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = RRR30_Helper::f_binary(k, index);
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
BENCHMARK(BM_SDSL30_LINEAR);
BENCHMARK(BM_SDSL30_LINEAR_SIMD);
BENCHMARK(BM_SDSL30_BINARY);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 30);
BENCHMARK_TEMPLATE(BM_FUNC, uint64_t, 63);

pair<uint16_t, uint16_t> divide(uint32_t x) { return {x >> 16, x}; }

int main(int argc, char** argv)
{
  if constexpr (kTest)
  {
    cout << "Testing 30 bit indexing."
         << "\n";
    for (size_t k = 0; k <= 30; ++k)
    {
      size_t counter = 0;
      for (size_t ones_in_big = 0; ones_in_big <= k; ++ones_in_big)
      {
        size_t ones_in_small = k - ones_in_big;
        for (size_t i = 0; i < RRR30_Helper::nCrArr[15][ones_in_big]; ++i)
          for (size_t j = 0; j < RRR30_Helper::nCrArr[15][ones_in_small]; ++j)
          {
            uint16_t small = binomial15::nr_to_bin(ones_in_small, j);
            uint16_t big = binomial15::nr_to_bin(ones_in_big, i);

            uint32_t x = (small << 16) | big;

            auto [a, b] = divide(x);

            if (auto res = RRR30_Helper::f(k, counter);
                x != res || res != RRR30_Helper::f_simd(k, counter) ||
                res != RRR30_Helper::f_binary(k, counter))
            {
              cerr << "!!!!!!!!!!Failed on " << k << " " << counter << "\n";
              // auto [res_a, res_b] = divide(res);
              exit(0);
            }
            ++counter;
          }
      }
    }
  }

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  size_t counter = 0;
  for (size_t k = 1; k <= 15; ++k)
  {
    for (size_t i = 0; i < RRR30_Helper::nCrArr[15][k]; ++i)
    {
      auto x = binomial15::nr_to_bin(k, i);
      auto y = get_ith_in_lexicographic_sdsl<uint32_t>(15, k, i + 1);

      if (x != y)
      {
        cerr << "Problem in impl 15."
             << "\n";
        return 1;
      }
      ++counter;
    }
  }

  cout << "Performed " << counter << " 15-bit tests."
       << "\n";

  // auto cnk = sdsl::binomial_coefficients<128>::data.table[128][64];

  return 0;
}