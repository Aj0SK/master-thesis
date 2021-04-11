#ifndef BIT_SEQUENCE_H
#define BIT_SEQUENCE_H

#include "bin_coeff.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include <immintrin.h>
#include <xmmintrin.h>

using std::cerr;
using std::next_permutation;
using std::pair;
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

class RRR30_Helper
{
private:
  inline static struct impl
  {
    std::array<std::array<size_t, 16>, 31> to_add;
    std::array<std::array<uint32_t, 16>, 31> helper;
    impl()
    {
      for (size_t k = 0; k < 31; ++k)
        for (size_t ones_in_big = 0; ones_in_big < 16; ++ones_in_big)
        {
          to_add[k][ones_in_big] =
              nCrArr[15][ones_in_big] * nCrArr[15][k - ones_in_big];
        }

      for (size_t k = 0; k < 31; ++k)
      {
        std::fill(helper[k].begin(), helper[k].end(), 0);
        uint32_t total = 0;
        for (size_t ones_in_big = (k > 15) ? (k - 15) : 0;
             ones_in_big <= std::min(k, static_cast<size_t>(15)); ++ones_in_big)
        {
          helper[k][ones_in_big] = total;
          total += to_add[k][ones_in_big];
        }
      }
    }
  } data;

public:
  // stores C(n, k) for all pairs of n and k and should be computed at the
  // compile time
  static constexpr auto nCrArr{BinCoeff<64>::set_data()};

  static inline pair<size_t, uint32_t> decode(uint32_t block)
  {
    size_t k = __builtin_popcount(block);
    uint32_t index = 0;
    uint32_t left = block >> 15;
    uint32_t right = block & 32767;
    size_t count_left = __builtin_popcount(left);
    size_t count_right =
        __builtin_popcount(right); // 32767 = 111'1111'1111'1111

    for (size_t i = 0; i < count_right; ++i)
    {
      index += nCrArr[15][i] * nCrArr[15][k - i];
    }

    index += nCrArr[15][count_left] * binomial15::bin_to_nr(right);
    index += binomial15::bin_to_nr(left);

    return {k, index};
  }

  static inline uint32_t f(size_t k, uint32_t index)
  {
    const size_t ones_in_big_lower = (k > 15) ? (k - 15) : 0;
    const size_t ones_in_big_upper = std::min(k, static_cast<size_t>(15));

    size_t ones_in_big = ones_in_big_lower;
    for (; ones_in_big < ones_in_big_upper; ++ones_in_big)
    {
      if (auto curr_index = data.helper[k][ones_in_big + 1];
          curr_index >= index)
      {
        if (curr_index == index)
          ++ones_in_big;
        break;
      }
    }

    index -= data.helper[k][ones_in_big];

    size_t ones_in_small = k - ones_in_big;

    uint32_t small_index =
        binomial15::nr_to_bin(ones_in_small, index % nCrArr[15][ones_in_small]);

    uint32_t big_index =
        binomial15::nr_to_bin(ones_in_big, index / nCrArr[15][ones_in_small]);

    return (small_index << 15) | big_index;
  }

  static inline uint32_t f_binary(size_t k, uint32_t index)
  {
    const size_t ones_in_big_lower = (k > 15) ? (k - 15) : 0;
    const size_t ones_in_big_upper = std::min(k, static_cast<size_t>(15));

    auto it =
        std::upper_bound(data.helper[k].begin() + ones_in_big_lower,
                         data.helper[k].begin() + ones_in_big_upper, index);
    size_t ones_in_big = std::distance(data.helper[k].begin(), it);
    if (data.helper[k][ones_in_big] > index)
      --ones_in_big;

    index -= data.helper[k][ones_in_big];

    size_t ones_in_small = k - ones_in_big;

    uint32_t small_index =
        binomial15::nr_to_bin(ones_in_small, index % nCrArr[15][ones_in_small]);

    uint32_t big_index =
        binomial15::nr_to_bin(ones_in_big, index / nCrArr[15][ones_in_small]);

    return (small_index << 15) | big_index;
  }

  static inline uint32_t f_simd(size_t k, uint32_t index)
  {
    const size_t ones_in_big_upper = std::min(k, static_cast<size_t>(15));

    const __m128i keys = _mm_set1_epi32(index);
    const __m128i vec1 =
        _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data.helper[k][0]));
    const __m128i vec2 =
        _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data.helper[k][4]));
    const __m128i vec3 =
        _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data.helper[k][8]));
    const __m128i vec4 =
        _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data.helper[k][12]));

    const __m128i cmp1 = _mm_cmpgt_epi32(vec1, keys);
    const __m128i cmp2 = _mm_cmpgt_epi32(vec2, keys);
    const __m128i cmp3 = _mm_cmpgt_epi32(vec3, keys);
    const __m128i cmp4 = _mm_cmpgt_epi32(vec4, keys);

    const __m128i tmp1 = _mm_packs_epi32(cmp1, cmp2);
    const __m128i tmp2 = _mm_packs_epi32(cmp3, cmp4);
    const uint32_t mask1 = _mm_movemask_epi8(tmp1);
    const uint32_t mask2 = _mm_movemask_epi8(tmp2);

    const uint32_t mask = (mask2 << 16) | mask1;

    size_t ones_in_big = ones_in_big_upper;

    if (mask != 0)
    {
      ones_in_big = (1 + __builtin_ctz(mask)) / 2;

      if (data.helper[k][ones_in_big] > index)
        --ones_in_big;
    }

    index -= data.helper[k][ones_in_big];

    size_t ones_in_small = k - ones_in_big;

    uint32_t small_index =
        binomial15::nr_to_bin(ones_in_small, index % nCrArr[15][ones_in_small]);

    uint32_t big_index =
        binomial15::nr_to_bin(ones_in_big, index / nCrArr[15][ones_in_small]);

    return (small_index << 15) | big_index;
  }
};

#endif