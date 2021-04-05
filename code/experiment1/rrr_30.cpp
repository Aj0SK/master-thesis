#include <algorithm>
#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>

#include "bin_helper.h"
#include "bit_sequence.h"

using std::cout;
using std::next_permutation;
using std::pair;
using std::vector;

constexpr size_t kMaxBinN = 64;

// stores C(n, k) for all pairs of n and k and should be computed at the
// compile time
static constexpr auto nCrArr{BinCoeff<kMaxBinN>::set_data()};

// source: sdsl-lite
// link:
// https://github.com/simongog/sdsl-lite/blob/master/include/sdsl/rrr_vector_15.hpp
class binomial15
{
public:
  typedef uint32_t number_type;

private:
  static inline class impl
  {
  public:
    static const int n = 15;
    static const int MAX_SIZE = 32;
    uint8_t m_space_for_bt[16];
    uint8_t m_space_for_bt_pair[256];
    uint64_t m_C[MAX_SIZE];
    vector<uint16_t> m_nr_to_bin;
    vector<uint16_t> m_bin_to_nr;

    impl()
    {
      m_nr_to_bin.resize(1 << n);
      m_bin_to_nr.resize(1 << n);
      for (int i = 0, cnt = 0, class_cnt = 0; i <= n; ++i)
      {
        m_C[i] = cnt;
        class_cnt = 0;
        std::vector<bool> b(n, 0);
        for (int j = 0; j < i; ++j)
          b[n - j - 1] = 1;
        do
        {
          uint32_t x = 0;
          for (int k = 0; k < n; ++k)
            x |= ((uint32_t)b[n - k - 1]) << (n - 1 - k);
          m_nr_to_bin[cnt] = x;
          m_bin_to_nr[x] = class_cnt;
          ++cnt;
          ++class_cnt;
        } while (next_permutation(b.begin(), b.end()));
      }
    }
  } iii;

public:
  static inline uint32_t nr_to_bin(uint8_t k, uint32_t nr)
  {
    return iii.m_nr_to_bin[iii.m_C[k] + nr];
  }

  static inline uint32_t bin_to_nr(uint32_t bin)
  {
    return iii.m_bin_to_nr[bin];
  }
};

vector<pair<int, int>> get_test(int N)
{
  srand(2104);
  vector<pair<int, int>> test;
  test.reserve(10'000);
  for (size_t i = 0; i < 10'000; ++i)
  {
    size_t k = 1 + rand() % (N - 1);
    size_t index = rand() % nCrArr[N][k];
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

uint32_t f(size_t k, size_t index)
{
  size_t total = 0, ones_in_big = (k > 15) ? (k - 15) : 0;
  for (; ones_in_big < std::min(k, static_cast<size_t>(15)); ++ones_in_big)
  {
    size_t will_add = nCrArr[15][ones_in_big] * nCrArr[15][k - ones_in_big];
    if (total + will_add == index)
    {
      total += will_add;
      ++ones_in_big;
      break;
    }
    else if (total + will_add > index)
    {
      break;
    }
    total += will_add;
  }

  size_t ones_in_small = k - ones_in_big;

  index -= total;

  uint32_t small_index =
      binomial15::nr_to_bin(ones_in_small, index % nCrArr[15][ones_in_small]);

  uint32_t big_index =
      binomial15::nr_to_bin(ones_in_big, index / nCrArr[15][ones_in_small]);

  return (small_index << 16) | big_index;
}

static void BM_SDSL30(benchmark::State& state)
{
  auto test = get_test(30);
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
      auto x = get_ith_in_lexicographic_sdsl<T>(N, k, index + 1);
      benchmark::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_SDSL15);
BENCHMARK(BM_SDSL30);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 30);
BENCHMARK_TEMPLATE(BM_FUNC, uint64_t, 63);

pair<uint16_t, uint16_t> divide(uint32_t x) { return {x >> 16, x}; }

int main(int argc, char** argv)
{
  /*for (size_t k = 0; k <= 30; ++k)
  {
    size_t counter = 0;
    for (size_t ones_in_big = 0; ones_in_big <= k; ++ones_in_big)
    {
      size_t ones_in_small = k - ones_in_big;
      for (size_t i = 0; i < nCrArr[15][ones_in_big]; ++i)
        for (size_t j = 0; j < nCrArr[15][ones_in_small]; ++j)
        {
          uint16_t small = binomial15::nr_to_bin(ones_in_small, j);
          uint16_t big = binomial15::nr_to_bin(ones_in_big, i);

          uint32_t x = (small << 16) | big;

          auto [a, b] = divide(x);

          if (auto res = f(k, counter); x != res)
          {
            cerr << "!!!!!!!!!!Failed on " << k << " " << counter << "\n";
            auto [res_a, res_b] = divide(res);
            cerr << "But is ";
            print_binary(res_a);
            cout << " and ";
            print_binary(res_b);
            cout << "\n";
            cerr << "Should be ";
            print_binary(a);
            cout << " ";
            print_binary(b);
            cout << "\n";
            exit(0);
          }
          ++counter;
        }
    }
  }*/

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  size_t counter = 0;
  for (size_t k = 1; k <= 15; ++k)
  {
    for (size_t i = 0; i < nCrArr[15][k]; ++i)
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

  return 0;
}