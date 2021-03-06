#ifndef BIN_COEFF_H
#define BIN_COEFF_H

#include <array>
#include <cstddef>

template <size_t kMaxBinN> class BinCoeff
{
public:
  using Arr = std::array<std::array<uint64_t, kMaxBinN + 1>, kMaxBinN + 1>;

  // from: https://cp-algorithms.com/combinatorics/binomial-coefficients.html
  static constexpr Arr set_data()
  {
    Arr C = {};

    C[0][0] = 1;
    for (uint64_t n = 1; n <= kMaxBinN; ++n)
    {
      C[n][0] = C[n][n] = 1;
      for (uint64_t k = 1; k < n; ++k)
        C[n][k] = C[n - 1][k - 1] + C[n - 1][k];
    }

    return C;
  }
};

// computes and returns C(n, k)
constexpr uint64_t nCr(uint64_t n, uint64_t k)
{
  // from: http://blog.plover.com/math/choose.html
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

static constexpr auto BinCoeff64{BinCoeff<64>::set_data()};

#endif