#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>

using std::array;
using std::cerr;
using std::cout;
using std::vector;

constexpr size_t kN = 1 << 3;
constexpr size_t kC = 4;
constexpr size_t kMaxBinN = 64;

// from https://cp-algorithms.com/combinatorics/binomial-coefficients.html
class BinCoeff
{
public:
  using Arr = std::array<std::array<std::size_t, kMaxBinN + 1>, kMaxBinN + 1>;

  static constexpr Arr set_data()
  {
    Arr C = {};

    C[0][0] = 1;
    for (size_t n = 1; n <= kMaxBinN; ++n)
    {
      C[n][0] = C[n][n] = 1;
      for (size_t k = 1; k < n; ++k)
        C[n][k] = C[n - 1][k - 1] + C[n - 1][k];
    }

    return C;
  }
};

static constexpr BinCoeff::Arr nCrArr{BinCoeff::set_data()};

constexpr size_t nCr(size_t N, size_t K)
{
  // This function gets the total number of unique combinations based upon N and
  // K. N is the total number of items. K is the size of the group. Total number
  // of unique combinations = N! / ( K! (N - K)! ). This function is less
  // efficient, but is more likely to not overflow when N and K are large. Taken
  // from:  http://blog.plover.com/math/choose.html
  if (K > N)
    return 0;
  size_t r = 1;
  for (size_t d = 1; d <= K; ++d)
  {
    r *= N--;
    r /= d;
  }
  return r;
}

vector<vector<bool>> create_helper(size_t n, size_t c)
{
  if (c >= n)
  {
    cerr << "Bad c in create_helper."
         << "\n";
    exit(0);
  }
  vector<bool> arr;
  arr.reserve(n);

  for (size_t i = 0; i < n - c; ++i)
    arr.push_back(0);
  for (size_t i = 0; i < c; ++i)
    arr.push_back(1);

  vector<vector<bool>> out;
  do
  {
    out.push_back(arr);
  } while (next_permutation(arr.begin(), arr.end()));

  return out;
}

vector<bool> get_ith(size_t n, size_t c, size_t target)
{
  vector<bool> out;
  for (size_t i = 0; i < n; ++i)
  {
    size_t bits_to_right = n - i - 1;
    // size_t count_with_zero = nCr(bits_to_right, c);
    size_t count_with_zero = nCrArr[bits_to_right][c];
    if (target > count_with_zero)
    {
      --c;
      out.push_back(1);
      target -= count_with_zero;
    }
    else
    {
      out.push_back(0);
    }
  }

  return out;
}

static void BM_SomeFunction1(benchmark::State& state)
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

static void BM_SomeFunction2(benchmark::State& state)
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

BENCHMARK(BM_SomeFunction1);
BENCHMARK(BM_SomeFunction2);

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

    size_t kTestN = 13;
    size_t kTestC = 3;

    auto helper = create_helper(kTestN, kTestC);
    for (size_t i = 0; i < helper.size(); ++i)
    {
      if (helper[i] != get_ith(kTestN, kTestC, i + 1))
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
  }

  return 0;
}