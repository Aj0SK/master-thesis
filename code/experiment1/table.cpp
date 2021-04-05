#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <bitset>
#include <climits>
#include <iostream>
#include <vector>

#include "bin_coeff.h"
#include "bit_sequence.h"

using std::array;
using std::cerr;
using std::cout;
using std::pair;
using std::vector;

constexpr size_t kMaxBinN = 64;

constexpr size_t kTestN = 13;
constexpr size_t kTestC = 3;

// stores C(n, k) for all pairs of n and k and should be computed at the
// compile time
static constexpr auto nCrArr{BinCoeff<kMaxBinN>::set_data()};

static void BM_BinCoeffPrecomputed(benchmark::State& state)
{
  srand(0);
  for (auto _ : state)
  {
    size_t a = rand() % kMaxBinN;
    size_t b = rand() % kMaxBinN;
    size_t x = nCrArr[a][b];
    benchmark::DoNotOptimize(x);
  }
}

static void BM_BinCoeffCompute(benchmark::State& state)
{
  srand(0);
  for (auto _ : state)
  {
    size_t a = rand() % kMaxBinN;
    size_t b = rand() % kMaxBinN;
    size_t x = nCr(a, b);
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_BinCoeffPrecomputed);
BENCHMARK(BM_BinCoeffCompute);

static void BM_GetIthFast(benchmark::State& state)
{
  srand(0);
  size_t a = kMaxBinN;
  for (auto _ : state)
  {
    state.PauseTiming();
    size_t b = 1 + rand() % a;
    size_t c = 1 + rand() % nCrArr[a][b];
    state.ResumeTiming();
    auto out = get_ith_in_lexicographic(a, b, c);
    benchmark::DoNotOptimize(out);
  }
}

BENCHMARK(BM_GetIthFast);

// source: sdsl-lite
// link:
// https://github.com/simongog/sdsl-lite/blob/master/include/sdsl/rrr_vector_15.hpp
pair<vector<uint32_t>, vector<uint32_t>> SDSL_lite_15()
{
  constexpr size_t n = 15;

  vector<uint32_t> m_nr_to_bin, m_bin_to_nr;
  m_nr_to_bin.resize(1 << n);
  m_bin_to_nr.resize(1 << n);

  for (int i = 0, cnt = 0, class_cnt = 0; i <= n; ++i)
  {
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

  return {m_nr_to_bin, m_bin_to_nr};
}

template <typename T> void print_binary(T x)
{
  cout << std::bitset<8 * sizeof(x)>(x);
}

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  {
    // some tests
    static_assert(nCr(12, 4) == 495);
    static_assert(nCr(45, 10) == 3'190'187'286);
    static_assert(nCr(30, 3) == 4'060);
    static_assert(nCr(25, 7) == 480'700);
    static_assert(nCr(19, 3) == 969);

    static_assert(nCrArr[12][4] == 495);
    static_assert(nCrArr[45][10] == 3'190'187'286);
    static_assert(nCrArr[25][7] == 480'700);

    auto helper = sorted_bit_sequences(kTestN, kTestC);
    for (size_t i = 0; i < helper.size(); ++i)
    {
      auto index = get_ith_in_lexicographic(kTestN, kTestC, i + 1);
      vector<bool> fast_out;
      for (size_t i = 0; i < kTestN; ++i)
      {
        size_t set_bit = 1 << (kTestN - i - 1);
        fast_out.push_back(index & set_bit);
      }

      if (auto real_result = helper[i]; real_result != fast_out)
      {
        cout << "Error in get_ith"
             << "\n";
        return 0;
      }
    }

    for (size_t i = 0; i + 1 < kMaxBinN; ++i)
      for (size_t j = 0; j < i; ++j)
      {
        if (nCrArr[i][j] != nCr(i, j))
        {
          cout << "Error in binomial coeff... for " << i << " " << j << "\n";
          return 0;
        }
      }

    cout << "Successfully run some basic tests!"
         << "\n";
  }

  auto helper = sorted_bit_sequences(15, kTestC);
  auto [to_bin_sdsl16, from_bin_sdsl] = SDSL_lite_15();

  return 0;
}