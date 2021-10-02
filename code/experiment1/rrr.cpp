#include "binary_helpers.h"
#include "rrr_convert.h"

#include <sdsl/rrr_helper.hpp>
#include <sdsl/rrr_vector_15.hpp>

#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::min;
using std::next_permutation;
using std::pair;
using std::vector;

constexpr bool kTest30 = false, kTest31 = false, kTest62 = false,
               kTest63 = false;

////////////////////////////////////////////////////////////////////////////////
// Benchmarks

constexpr size_t kPerTestQueries = 10'000;

// Returns vector of random pairs. Each pair represents
// (k, index) where k is number of set bits in the block
// and index is an index into sequence of all k-bits blocks
// in some particular order.
vector<pair<uint64_t, uint64_t>> get_test(uint64_t N)
{
  srand(2104);
  vector<pair<uint64_t, uint64_t>> test;
  test.reserve(kPerTestQueries);
  for (size_t i = 0; i < kPerTestQueries; ++i)
  {
    uint64_t k = 1 + rand() % (N - 1);
    uint64_t index = rand() % BinCoeff64[N][k];
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
      auto block = RRR30_Helper::f(k, index);
      benchmark::DoNotOptimize(block);
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
      auto block = RRR30_Helper::f_simd(k, index);
      benchmark::DoNotOptimize(block);
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
      auto block = RRR30_Helper::f_binary(k, index);
      benchmark::DoNotOptimize(block);
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
      auto block = RRR31_Helper::f(k, index);
      benchmark::DoNotOptimize(block);
    }
  }
}

static void BM_SDSL62_LINEAR(benchmark::State& state)
{
  auto test = get_test(62);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto block = RRR62_Helper::f(k, index);
      benchmark::DoNotOptimize(block);
    }
  }
}

static void BM_SDSL62_BINARY(benchmark::State& state)
{
  auto test = get_test(62);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto block = RRR62_Helper::f_binary(k, index);
      benchmark::DoNotOptimize(block);
    }
  }
}

static void BM_SDSL63_LINEAR(benchmark::State& state)
{
  auto test = get_test(63);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto block = RRR63_Helper::f(k, index);
      benchmark::DoNotOptimize(block);
    }
  }
}

static void BM_SDSL63_33030(benchmark::State& state)
{
  auto test = get_test(63);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto block = RRR63_Alt_Helper::f(k, index);
      benchmark::DoNotOptimize(block);
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
      auto block = sdsl::rrr_helper<N>::decode_bit(k, index, N - 1);
      benchmark::DoNotOptimize(block);
    }
  }
}

BENCHMARK(BM_SDSL15);
BENCHMARK(BM_SDSL30_LINEAR);
BENCHMARK(BM_SDSL30_LINEAR_SIMD);
BENCHMARK(BM_SDSL30_BINARY);
BENCHMARK(BM_SDSL31_LINEAR_SIMD);
BENCHMARK(BM_SDSL62_LINEAR);
BENCHMARK(BM_SDSL62_BINARY);
BENCHMARK(BM_SDSL63_LINEAR);
BENCHMARK(BM_SDSL63_33030);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 30);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 31);
BENCHMARK_TEMPLATE(BM_FUNC, uint64_t, 62);
BENCHMARK_TEMPLATE(BM_FUNC, uint64_t, 63);

int main(int argc, char** argv)
{
  if constexpr (kTest30)
  {
    cout << "Started testing of 30 bit impl...\n";
    for (size_t i = 0; i < (1ull << 30); ++i)
    {
      auto [k, index] = RRR30_Helper::decode(i);
      uint32_t res = RRR30_Helper::f_simd(k, index);
      if (res != i)
      {
        cout << "Problem with 30-bit impl!\n";
        return 1;
      }
    }
    std::cout << "Successfully tested for block size " << 30 << "\n";
  }

  if constexpr (kTest31)
  {
    cout << "Started testing of 31 bit impl...\n";
    for (uint32_t i = 0; i < (1ull << 31); ++i)
    {
      auto [k, index] = RRR31_Helper::decode(i);
      uint32_t res = RRR31_Helper::f(k, index);
      if (res != i)
      {
        cout << "Problem with 31-bit impl!\n";
        return 1;
      }
    }
    std::cout << "Successfully tested for block size " << 31 << "\n";
  }

  if constexpr (kTest62)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    cout << "Started testing of 62 bit impl...\n";
    for (uint32_t i = 0; i < 1'000'000; ++i)
    {
      uint64_t r = dis(gen) % (1ull << 62);

      auto [k, index] = RRR62_Helper::decode(r);
      uint64_t res = RRR62_Helper::f(k, index);
      if (res != r)
      {
        cout << "Problem with 62-bit impl!\n";
        return 1;
      }
    }
    std::cout << "Successfully tested for block size " << 62 << "\n";
  }

  if constexpr (kTest63)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    cout << "Started testing of 63 bit impl...\n";
    for (uint32_t i = 0; i < 1'000'000'000; ++i)
    {
      uint64_t r = dis(gen) % (1ull << 63);
      auto [k, index] = RRR63_Alt_Helper::decode(r);
      uint64_t res = RRR63_Alt_Helper::f(k, index);
      if (res != r)
      {
        cout << "Problem with 63-bit impl!\nShould be:";
        return 1;
      }
    }
    std::cout << "Successfully tested for block size " << 63 << "\n";
  }

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  size_t counter = 0;
  for (size_t k = 1; k <= 15; ++k)
  {
    for (size_t i = 0; i < BinCoeff64[15][k]; ++i)
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
  return 0;
}