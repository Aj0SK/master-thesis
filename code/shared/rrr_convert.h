#ifndef RRR_CONVERT_H
#define RRR_CONVERT_H

#include "binomial_coefficient.h"

#include <sdsl/int_vector.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

#include <immintrin.h>
#include <xmmintrin.h>

using std::pair;
using std::vector;

template <typename T>
T get_ith_in_lexicographic_sdsl(size_t n, size_t c, size_t target)
{
  // Stores C(n, k) for all pairs of n and k and should be computed at the
  // compile time
  static constexpr auto nCrArr{BinCoeff<64, uint64_t>::set_data()};

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

// SDSL's implementation of a 15-bit block in rrr_vector
// source: sdsl-lite
// link:
// https://github.com/simongog/sdsl-lite/blob/master/include/sdsl/rrr_vector_15.hpp
class binomial15
{
public:
  typedef uint32_t number_type;

private:
  static class impl
  {
  public:
    static const int n = 15;
    static const int MAX_SIZE = 32;
    uint8_t m_space_for_bt[16];
    uint8_t m_space_for_bt_pair[256];
    uint64_t m_C[MAX_SIZE];
    sdsl::int_vector<16> m_nr_to_bin;
    sdsl::int_vector<16> m_bin_to_nr;

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
        if (class_cnt == 1)
          m_space_for_bt[i] = 0;
        else
          m_space_for_bt[i] = sdsl::bits::hi(class_cnt) + 1;
      }
      if (n == 15)
      {
        for (int x = 0; x < 256; ++x)
        {
          m_space_for_bt_pair[x] =
              m_space_for_bt[x >> 4] + m_space_for_bt[x & 0x0F];
        }
      }
    }
  } iii;

public:
  static inline uint8_t space_for_bt(uint32_t i)
  {
    return iii.m_space_for_bt[i];
  }

  static inline uint32_t nr_to_bin(uint8_t k, uint32_t nr)
  {
    return iii.m_nr_to_bin[iii.m_C[k] + nr];
  }

  static inline uint32_t bin_to_nr(uint32_t bin)
  {
    return iii.m_bin_to_nr[bin];
  }

  static inline uint8_t space_for_bt_pair(uint8_t x)
  {
    return iii.m_space_for_bt_pair[x];
  }
};

// Implementation of a 30-bit block size version.
// 000000000000000|000000000000000
// ___big_block___|__small_block__
// (k - number of one's, index - position in all numbers with k-bits set)
class RRR30_Helper
{
private:
  inline static struct impl
  {
    std::array<std::array<uint32_t, 16>, 31> helper;
    impl()
    {
      for (size_t k = 0; k < 31; ++k)
      {
        std::fill(helper[k].begin(), helper[k].end(), 0);
        uint32_t total = 0;
        for (size_t ones_in_big = (k > 15) ? (k - 15) : 0;
             ones_in_big <= std::min(k, static_cast<size_t>(15)); ++ones_in_big)
        {
          helper[k][ones_in_big] = total;
          total +=
              BinCoeff64[15][ones_in_big] * BinCoeff64[15][k - ones_in_big];
        }
      }
    }
  } data;

public:
  static inline pair<int, uint32_t> encode(uint32_t bin)
  {
    const int k = __builtin_popcount(bin);
    const uint32_t left_bin = bin >> 15;     // upper 15 bits
    const uint32_t right_bin = bin & 0x7fff; // lower 15 bits
    const int left_k = __builtin_popcount(left_bin);
    const int right_k = __builtin_popcount(right_bin);

    uint32_t nr = 0;
    nr += data.helper[k][right_k];
    nr += BinCoeff64[15][left_k] * binomial15::bin_to_nr(right_bin);
    nr += binomial15::bin_to_nr(left_bin);

    return {k, nr};
  }

  static inline uint32_t decode(int k, uint32_t nr)
  {
    const int right_k_from = (k > 15) ? (k - 15) : 0;
    const int right_k_to = std::min(k, 15);

    int right_k = right_k_from;
    for (; right_k < right_k_to; ++right_k)
    {
      if (auto curr_index = data.helper[k][right_k + 1]; curr_index >= nr)
      {
        if (curr_index == nr)
          ++right_k;
        break;
      }
    }

    nr -= data.helper[k][right_k];

    const int left_k = k - right_k;

    const uint32_t left_bin =
        binomial15::nr_to_bin(left_k, nr % BinCoeff64[15][left_k]);

    const uint32_t right_bin =
        binomial15::nr_to_bin(right_k, nr / BinCoeff64[15][left_k]);

    return (left_bin << 15) | right_bin;
  }

  static inline uint32_t decode_binary(int k, uint32_t nr)
  {
    const int right_k_from = (k > 15) ? (k - 15) : 0;
    const int right_k_to = std::min(k, 15);

    const auto it = std::upper_bound(data.helper[k].begin() + right_k_from,
                                     data.helper[k].begin() + right_k_to, nr);
    int right_k = std::distance(data.helper[k].begin(), it);
    if (data.helper[k][right_k] > nr)
      --right_k;

    nr -= data.helper[k][right_k];

    const int left_k = k - right_k;

    const uint32_t left_bin =
        binomial15::nr_to_bin(left_k, nr % BinCoeff64[15][left_k]);

    const uint32_t right_bin =
        binomial15::nr_to_bin(right_k, nr / BinCoeff64[15][left_k]);

    return (left_bin << 15) | right_bin;
  }

  static inline uint32_t decode_simd(int k, uint32_t nr)
  {
    const int right_k_to = std::min(k, 15);

    const __m128i keys = _mm_set1_epi32(nr);
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

    int right_k = right_k_to;
    if (mask != 0)
    {
      right_k = (1 + __builtin_ctz(mask)) / 2;

      if (data.helper[k][right_k] > nr)
        --right_k;
    }

    nr -= data.helper[k][right_k];

    const int left_k = k - right_k;

    const uint32_t left_bin =
        binomial15::nr_to_bin(left_k, nr % BinCoeff64[15][left_k]);

    const uint32_t right_bin =
        binomial15::nr_to_bin(right_k, nr / BinCoeff64[15][left_k]);

    return (left_bin << 15) | right_bin;
  }
};

class RRR31_Helper
{
public:
  static uint32_t f(uint32_t k, uint32_t index)
  {
    const uint32_t threshold = BinCoeff64[30][k];
    uint32_t to_or = 0;
    if (index >= threshold)
    {
      --k;
      index -= threshold;
      to_or = 1 << 30;
    }
    return to_or | RRR30_Helper::decode_simd(k, index);
  }

  static pair<uint32_t, uint32_t> encode(uint32_t block)
  {
    const uint32_t k = __builtin_popcount(block);

    if (block & (1 << 30))
    {
      const uint32_t new_block = block & (~(1 << 30));
      return {k, BinCoeff64[30][k] + RRR30_Helper::encode(new_block).second};
    }
    else
    {
      return {k, RRR30_Helper::encode(block).second};
    }
  }
};

////////////////////////////////////////////////

class RRR62_Helper
{
private:
  inline static struct impl
  {
    std::array<std::array<uint64_t, 32>, 63> helper;
    impl()
    {
      for (size_t k = 0; k < 63; ++k)
      {
        std::fill(helper[k].begin(), helper[k].end(), 0);
        uint64_t total = 0;
        for (size_t ones_in_big = (k > 31) ? (k - 31) : 0;
             ones_in_big <= std::min(k, static_cast<size_t>(31)); ++ones_in_big)
        {
          helper[k][ones_in_big] = total;
          total +=
              BinCoeff64[31][ones_in_big] * BinCoeff64[31][k - ones_in_big];
        }
      }
    }
  } data;

public:
  static inline uint64_t f(const size_t k, uint64_t index)
  {
    const size_t set_in_big_lower = (k > 31) ? (k - 31) : 0;
    const size_t set_in_big_upper = std::min(k, static_cast<size_t>(31));

    size_t set_in_big = set_in_big_lower;
    for (; set_in_big < set_in_big_upper; ++set_in_big)
    {
      if (auto curr_index = data.helper[k][set_in_big + 1]; curr_index >= index)
      {
        if (curr_index == index)
          ++set_in_big;
        break;
      }
    }

    index -= data.helper[k][set_in_big];

    const size_t set_in_small = k - set_in_big;

    const uint64_t small_index =
        RRR31_Helper::f(set_in_small, index % BinCoeff64[31][set_in_small]);
    const uint64_t big_index =
        RRR31_Helper::f(set_in_big, index / BinCoeff64[31][set_in_small]);

    return (small_index << 31) | big_index;
  }

  static inline uint64_t f_binary(const size_t k, uint64_t index)
  {
    const size_t set_in_big_lower = (k > 31) ? (k - 31) : 0;
    const size_t set_in_big_upper = std::min(k, static_cast<size_t>(31));

    const auto it =
        std::upper_bound(data.helper[k].begin() + set_in_big_lower,
                         data.helper[k].begin() + set_in_big_upper, index);
    size_t set_in_big = std::distance(data.helper[k].begin(), it);
    if (data.helper[k][set_in_big] > index)
      --set_in_big;

    index -= data.helper[k][set_in_big];

    const size_t set_in_small = k - set_in_big;

    const uint64_t small_index =
        RRR31_Helper::f(set_in_small, index % BinCoeff64[31][set_in_small]);
    const uint64_t big_index =
        RRR31_Helper::f(set_in_big, index / BinCoeff64[31][set_in_small]);

    return (small_index << 31) | big_index;
  }

  static inline pair<uint64_t, uint64_t> encode(const uint64_t block)
  {
    const uint64_t k = __builtin_popcountll(block);
    const uint32_t left = block >> 31;
    const uint32_t right =
        block & 2147483647; // 2147483647_10 = 1111111111111111111111111111111_2
    const uint32_t count_left = __builtin_popcount(left);
    const uint32_t count_right = __builtin_popcount(right);

    uint64_t index = 0;
    index += data.helper[k][count_right];
    index += BinCoeff64[31][count_left] * RRR31_Helper::encode(right).second;
    index += RRR31_Helper::encode(left).second;

    return {k, index};
  }
};

class RRR63_Helper
{
public:
  static uint64_t f(uint64_t k, uint64_t index)
  {
    const uint64_t threshold = BinCoeff64[62][k];
    uint64_t to_or = 0;
    if (index >= threshold)
    {
      --k;
      index -= threshold;
      to_or = 1ull << 62;
    }
    return to_or | RRR62_Helper::f(k, index);
  }

  static pair<uint64_t, uint64_t> encode(const uint64_t block)
  {
    const uint64_t k = __builtin_popcountll(block);

    if (block & (1ull << 62))
    {
      const uint64_t new_block = block & (~(1ull << 62));
      return {k, BinCoeff64[62][k] + RRR62_Helper::encode(new_block).second};
    }
    else
    {
      return {k, RRR62_Helper::encode(block).second};
    }
  }
};

class RRR63_Alt_Helper
{
private:
  inline static struct impl
  {
    std::array<std::array<uint64_t, 31>, 64> helper;
    impl()
    {
      for (size_t k = 0; k < 64; ++k)
      {
        std::fill(helper[k].begin(), helper[k].end(), 0);
        uint64_t total = 0;
        for (size_t ones_in_big = (k > 30) ? (k - 30) : 0;
             ones_in_big <= std::min(k, static_cast<size_t>(30)); ++ones_in_big)
        {
          helper[k][ones_in_big] = total;
          total +=
              BinCoeff64[30][ones_in_big] * BinCoeff64[30][k - ones_in_big];
        }
      }
    }
  } data;

public:
  static inline uint64_t f(size_t k, uint64_t index)
  {
    // x000
    // pozicia: 4 -> 63
    // shiftov: 3 -> 62
    // pocet kombinacii pred: 8 (1<<3)
    // pocet shiftov
    // x001
    // x010
    // x011
    // x100
    uint64_t to_or = 0;
    const bool first_bit = (index >= (BinCoeff64[62][k]));
    if (first_bit)
    {
      index -= BinCoeff64[62][k];
      --k;
      to_or |= 1ull << 62;
    }
    const bool second_bit = (index >= (BinCoeff64[61][k]));
    if (second_bit)
    {
      index -= BinCoeff64[61][k];
      --k;
      to_or |= 1ull << 61;
    }
    const bool third_bit = (index >= (BinCoeff64[60][k]));
    if (third_bit)
    {
      index -= BinCoeff64[60][k];
      --k;
      to_or |= 1ull << 60;
    }

    const size_t set_in_big_lower = (k > 30) ? (k - 30) : 0;
    const size_t set_in_big_upper = std::min(k, static_cast<size_t>(30));

    size_t set_in_big = set_in_big_lower;
    for (; set_in_big < set_in_big_upper; ++set_in_big)
    {
      if (auto curr_index = data.helper[k][set_in_big + 1]; curr_index >= index)
      {
        if (curr_index == index)
          ++set_in_big;
        break;
      }
    }

    index -= data.helper[k][set_in_big];

    const size_t set_in_small = k - set_in_big;

    const uint64_t small_index = RRR30_Helper::decode_simd(
        set_in_small, index % BinCoeff64[30][set_in_small]);
    const uint64_t big_index = RRR30_Helper::decode_simd(
        set_in_big, index / BinCoeff64[30][set_in_small]);

    return to_or | (small_index << 30) | big_index;
  }

  static pair<uint64_t, uint64_t> encode(const uint64_t block)
  {
    const uint64_t k = __builtin_popcountll(block);
    bool first_bit = block & (1ull << 62);
    bool second_bit = block & (1ull << 61);
    bool third_bit = block & (1ull << 60);

    uint64_t index = first_bit * BinCoeff64[62][k] +
                     second_bit * BinCoeff64[61][k - first_bit] +
                     third_bit * BinCoeff64[60][k - first_bit - second_bit];

    const uint64_t rem_k = k - first_bit - second_bit - third_bit;

    // 1073741823_10 = 111111111111111111111111111111_2
    const uint32_t left = (block >> 30) & 1073741823ull;
    const uint32_t right = block & 1073741823ull;
    const uint32_t count_left = __builtin_popcount(left);
    const uint32_t count_right = __builtin_popcount(right);

    index += data.helper[rem_k][count_right];
    index += BinCoeff64[30][count_left] * RRR30_Helper::encode(right).second;
    index += RRR30_Helper::encode(left).second;
    return {k, index};
  }
};

class RRR127_Helper
{
private:
  inline static struct impl
  {
    std::array<std::array<__uint128_t, 64>, 128> helper;
    impl()
    {
      for (size_t k = 0; k < 128; ++k)
      {
        std::fill(helper[k].begin(), helper[k].end(), 0);
        __uint128_t total = 0;
        for (size_t ones_in_big = (k > 63) ? (k - 63) : 0;
             ones_in_big <= std::min(k, static_cast<size_t>(63)); ++ones_in_big)
        {
          helper[k][ones_in_big] = total;
          total +=
              BinCoeff128[63][ones_in_big] * BinCoeff128[63][k - ones_in_big];
        }
      }
    }
  } data;

public:
  static inline __uint128_t f(size_t k, __uint128_t index)
  {
    // x000
    // pozicia: 4 -> 63
    // shiftov: 3 -> 62
    // pocet kombinacii pred: 8 (1<<3)
    // pocet shiftov
    // x001
    // x010
    // x011
    // x100
    __uint128_t to_or = 0;
    constexpr __uint128_t one = 1;
    const bool first_bit = (index >= (BinCoeff128[126][k]));
    if (first_bit)
    {
      index -= BinCoeff128[126][k];
      --k;
      to_or |= one << 126;
    }

    const size_t set_in_big_lower = (k > 63) ? (k - 63) : 0;
    const size_t set_in_big_upper = std::min(k, static_cast<size_t>(63));

    size_t set_in_big = set_in_big_lower;
    for (; set_in_big < set_in_big_upper; ++set_in_big)
    {
      if (auto curr_index = data.helper[k][set_in_big + 1]; curr_index >= index)
      {
        if (curr_index == index)
          ++set_in_big;
        break;
      }
    }

    index -= data.helper[k][set_in_big];

    const size_t set_in_small = k - set_in_big;

    const __uint128_t small_index =
        RRR63_Helper::f(set_in_small, index % BinCoeff128[63][set_in_small]);
    const __uint128_t big_index =
        RRR63_Helper::f(set_in_big, index / BinCoeff128[63][set_in_small]);

    return to_or | (small_index << 63) | big_index;
  }

  static size_t popcountllll(__uint128_t n)
  {
    const size_t cnt_hi = __builtin_popcountll(n >> 64);
    const size_t cnt_lo = __builtin_popcountll(n);
    return cnt_hi + cnt_lo;
  }

  static pair<uint64_t, __uint128_t> encode(const __uint128_t block)
  {
    const size_t k = popcountllll(block);
    const __uint128_t one = 1;
    bool first_bit = block & (one << 126);

    __uint128_t index = first_bit * BinCoeff128[126][k];

    const uint64_t rem_k = k - first_bit;

    // 1073741823_10 = 111111111111111111111111111111_2
    const __uint128_t low_end = 9223372036854775807ull;
    const uint64_t left = (block >> 63) & low_end;
    const uint64_t right = block & low_end;
    const uint64_t count_left = __builtin_popcountll(left);
    const uint64_t count_right = __builtin_popcountll(right);

    index += data.helper[rem_k][count_right];
    index += BinCoeff128[63][count_left] *
             static_cast<__uint128_t>(RRR63_Helper::encode(right).second);
    index += RRR63_Helper::encode(left).second;
    return {k, index};
  }
};

#endif