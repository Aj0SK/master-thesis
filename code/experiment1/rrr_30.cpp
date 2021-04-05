#include <algorithm>
#include <iostream>
#include <vector>

#include "bin_helper.h"
#include "bit_sequence.h"

using std::cout;
using std::next_permutation;
using std::pair;
using std::vector;

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

int main()
{
  // sdsl

  for (size_t i = 1; i < 15; ++i)
  {
    cout << i;
    cout << ":";
    auto x = binomial15::nr_to_bin(i, 1);
    print_binary(x);
    cout << "\n";

    auto y = get_ith_in_lexicographic_sdsl<uint32_t>(15, i, 2);
    cout << "alt:";
    print_binary(y);
    cout << "\n";

    if (x != y)
    {
      cerr << "Problem in impl."
           << "\n";
      return 1;
    }
  }

  return 0;
}