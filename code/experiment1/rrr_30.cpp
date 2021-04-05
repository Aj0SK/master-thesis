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

constexpr size_t kMaxBinN = 50;

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

static void BM_SDSL15(benchmark::State& state)
{
  srand(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    size_t k = 1 + rand() % 14;
    size_t index = rand() % nCrArr[15][k];
    state.ResumeTiming();
    size_t x = binomial15::nr_to_bin(k, index);
    benchmark::DoNotOptimize(x);
  }
}

template <typename T, size_t N> static void BM_FUNC(benchmark::State& state)
{
  srand(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    size_t k = 1 + rand() % (N - 1);
    size_t index = rand() % nCrArr[N][k];
    state.ResumeTiming();
    size_t x = get_ith_in_lexicographic_sdsl<T>(N, k, index + 1);
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_SDSL15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 15);
BENCHMARK_TEMPLATE(BM_FUNC, uint32_t, 30);

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  size_t counter = 0;
  for (size_t k = 1; k < 15; ++k)
  {
    for (size_t i = 0; i < nCrArr[15][k]; ++i)
    {
      auto x = binomial15::nr_to_bin(k, i);
      auto y = get_ith_in_lexicographic_sdsl<uint32_t>(15, k, i + 1);

      if (x != y)
      {
        cerr << "Problem in impl."
             << "\n";
        return 1;
      }
      ++counter;
    }
  }

  cout << "Performed " << counter << " tests."
       << "\n";

  return 0;
}