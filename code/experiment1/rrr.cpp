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

constexpr bool kTest30 = false, kTest31 = false, kTest62 = true, kTest63 = true;
constexpr size_t kMaxBinN = 64;

////////////////////////////////////////////////////////////////////////////////
// Benchmarks

vector<pair<uint64_t, uint64_t>> get_test(int N)
{
  srand(2104);
  vector<pair<uint64_t, uint64_t>> test;
  test.reserve(10'000);
  for (size_t i = 0; i < 10'000; ++i)
  {
    size_t k = 1 + rand() % (N - 1);
    size_t index = rand() % BinCoeff64[N][k];
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

static void BM_SDSL31_LINEAR_SIMD(benchmark::State& state)
{
  auto test = get_test(31);
  for (auto _ : state)
  {
    for (auto [k, index] : test)
    {
      auto x = RRR31_Helper::f(k, index);
      benchmark::DoNotOptimize(x);
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
      auto x = RRR62_Helper::f(k, index);
      benchmark::DoNotOptimize(x);
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
      auto x = RRR62_Helper::f_binary(k, index);
      benchmark::DoNotOptimize(x);
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
      auto x = RRR63_Helper::f(k, index);
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
BENCHMARK(BM_SDSL31_LINEAR_SIMD);
BENCHMARK(BM_SDSL62_LINEAR);
BENCHMARK(BM_SDSL62_BINARY);
BENCHMARK(BM_SDSL63_LINEAR);
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
    for (size_t i = 0; i < (1 << 30); ++i)
    {
      auto [k, index] = RRR30_Helper::decode(i);
      uint32_t res = RRR30_Helper::f(k, index);
      if (res != i)
      {
        cout << "Problem!\n";
        exit(1);
      }
    }
  }

  if constexpr (kTest31)
  {
    cout << "Started testing of 31 bit impl...\n";
    for (uint32_t i = 0; i < (1 << 31); ++i)
    {
      auto [k, index] = RRR31_Helper::decode(i);
      uint32_t res = RRR31_Helper::f(k, index);
      if (res != i)
      {
        cout << "Problem na indexe: " << i << "\n";
        print_binary(i);
        cout << "\n";
        print_binary(res);
        cout << "\n";
        cout << k << " " << index << "\n";
        exit(1);
      }
    }
  }

  if constexpr (kTest62)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    cout << "Started testing of 62 bit impl...\n";
    for (uint32_t i = 0; i < 1'000'000; ++i)
    {
      uint64_t r = dis(gen);
      r %= 1ull << 62;

      auto [k, index] = RRR62_Helper::decode(r);
      uint64_t res = RRR62_Helper::f(k, index);
      if (res != r)
      {
        cout << "Problem na indexe: " << r << "\n";
        print_binary(r);
        cout << "\n";
        print_binary(res);
        cout << "\n";
        cout << k << " " << index << "\n";
        exit(1);
      }
    }
  }

  if constexpr (kTest63)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    cout << "Started testing of 63 bit impl...\n";
    for (uint32_t i = 0; i < 1'000'000; ++i)
    {
      uint64_t r = dis(gen);
      r %= 1ull << 63;

      auto [k, index] = RRR63_Helper::decode(r);
      uint64_t res = RRR63_Helper::f(k, index);
      if (res != r)
      {
        cout << "Problem na indexe: " << r << "\n";
        print_binary(r);
        cout << "\n";
        print_binary(res);
        cout << "\n";
        cout << k << " " << index << "\n";
        exit(1);
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

  cout << "Performed " << counter << " 15-bit tests."
       << "\n";

  // auto cnk = sdsl::binomial_coefficients<128>::data.table[128][64];

  return 0;
}