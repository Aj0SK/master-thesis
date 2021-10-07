#ifndef BINOMIAL_COEFFICIENT_H
#define BINOMIAL_COEFFICIENT_H

#include <array>
#include <cstddef>

// Class storing C(n, k) and computing it in a compile time.
template <size_t kMaxBinN, typename T> class BinCoeff
{
public:
  using Arr = std::array<std::array<T, kMaxBinN + 1>, kMaxBinN + 1>;

  // source: https://cp-algorithms.com/combinatorics/binomial-coefficients.html
  static constexpr Arr set_data()
  {
    // static_assert(kMaxBinN <= 64, "Max n in bin-coeff should be <=64.");
    Arr C = {};

    C[0][0] = 1;
    for (T n = 1; n <= kMaxBinN; ++n)
    {
      C[n][0] = C[n][n] = 1;
      for (T k = 1; k < n; ++k)
        C[n][k] = C[n - 1][k - 1] + C[n - 1][k];
    }

    return C;
  }
};

// Computes and returns C(n, k)
// source: http://blog.plover.com/math/choose.html
constexpr uint64_t nCr(uint64_t n, uint64_t k)
{
  if (k > n)
    return 0;
  uint64_t r = 1;
  for (uint64_t d = 1; d <= k; ++d)
  {
    r *= n--;
    r /= d;
  }
  return r;
}

constexpr auto BinCoeff64{BinCoeff<64, uint64_t>::set_data()};
constexpr auto BinCoeff128{BinCoeff<128, __uint128_t>::set_data()};

#endif