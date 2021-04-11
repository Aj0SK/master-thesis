#ifndef BIT_SEQUENCE_H
#define BIT_SEQUENCE_H

#include "bin_coeff.h"

#include <algorithm>
#include <iostream>
#include <vector>

using std::cerr;
using std::next_permutation;
using std::vector;

// returns vector of all n-bit sequences with c-bits set
// in the lexigraphical order
vector<vector<bool>> sorted_bit_sequences(size_t n, size_t c)
{
  if (c > n)
  {
    cerr << "Bad c in sorted_bit_sequences."
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

// find the binary block that is on a position "target" in a lexicographical
// ordering

size_t get_ith_in_lexicographic(size_t n, size_t c, size_t target)
{
  // stores C(n, k) for all pairs of n and k and should be computed at the
  // compile time

  static constexpr auto nCrArr{BinCoeff<64>::set_data()};

  size_t out = 0;
  for (size_t i = 0; i < n; ++i)
  {
    size_t bits_to_right = n - i - 1;
    size_t count_with_zero = nCrArr[bits_to_right][c];
    out <<= 1;
    if (target > count_with_zero)
    {
      --c;
      out |= 1;
      target -= count_with_zero;
    }
  }

  return out;
}

template <typename T>
T get_ith_in_lexicographic_sdsl(size_t n, size_t c, size_t target)
{
  // stores C(n, k) for all pairs of n and k and should be computed at the
  // compile time

  static constexpr auto nCrArr{BinCoeff<64>::set_data()};

  T out = 0;
  for (size_t i = 0; i < n; ++i)
  {
    size_t bits_to_right = n - i - 1;
    size_t count_with_zero = nCrArr[bits_to_right][c];
    // out <<= 1;
    if (target > count_with_zero)
    {
      --c;
      out |= 1 << i;
      target -= count_with_zero;
    }
  }

  return out;
}

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

#endif