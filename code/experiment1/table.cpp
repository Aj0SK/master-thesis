#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>

#include "bin_coeff.h"
#include "bit_sequence.h"

using std::array;
using std::cerr;
using std::cout;
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

  return 0;
}