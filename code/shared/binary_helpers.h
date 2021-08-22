#ifndef BINARY_HELPER_H
#define BINARY_HELPER_H

#include "binomial_coefficient.h"

#include <algorithm>
#include <bitset>
#include <iostream>

using std::cerr;
using std::cout;
using std::random_shuffle;
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

// Find the binary block that is on a position "target" in a lexicographical
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

template <typename T> T generate_bit_sequence(size_t n, size_t k)
{
  vector<size_t> indexes;
  indexes.reserve(n);

  for (size_t i = 0; i < n; ++i)
    indexes.push_back(i);

  std::random_shuffle(indexes.begin(), indexes.end());

  T out = static_cast<T>(0);
  for (size_t i = 0; i < k; ++i)
  {
    out |= 1 << (indexes[i]);
  }
  return out;
}

template <typename T> void print_binary(T x)
{
  cout << std::bitset<8 * sizeof(x)>(x);
}

#endif