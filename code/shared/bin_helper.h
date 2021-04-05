#ifndef BIN_HELPER_H
#define BIN_HELPER_H

#include <algorithm>
#include <bitset>
#include <iostream>

using std::cout;
using std::random_shuffle;
using std::vector;

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