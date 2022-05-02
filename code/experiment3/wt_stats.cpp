#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sdsl/rrr_helper.hpp>
#include <sdsl/rrr_vector.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace sdsl;

using namespace std;

static inline __uint128_t sdsl_to_gcc(sdsl::uint128_t x)
{
  return (static_cast<__uint128_t>(static_cast<uint64_t>(x >> 64)) << 64) +
         static_cast<uint64_t>(x);
}

std::string toString(__uint128_t num)
{
  std::string str;
  do
  {
    int digit = num % 10;
    str = std::to_string(digit) + str;
    num = (num - digit) / 10;
  } while (num != 0);
  return str;
}

template <int block_size, int cutoff> void analyze(const bit_vector& bv)
{
  vector<int> freq(block_size + 1, 0);
  for (int i = 0; i < bv.size(); i += block_size)
  {
    int count = 0;
    for (int j = i; j < min((int)bv.size(), i + block_size); ++j)
    {
      if (bv[j])
        ++count;
    }
    ++freq[count];
  }

  size_t sum = 0;
  for (auto x : freq)
  {
    sum += x;
  }

  uint16_t cut_from = (cutoff + 1) / 2;
  uint16_t cut_to = block_size - cut_from + 1;

  double total_classic = 0, total_hybrid = 0;
  for (int i = 0; i <= block_size; ++i)
  {
    double x = 0.0;

    if constexpr (block_size != 127)
    {
      x = ceil(
          log2(binomial_coefficients<block_size>::data.table[block_size][i]));
    }

    if constexpr (block_size == 127)
    {
      __uint128_t num = sdsl_to_gcc(
          binomial_coefficients<block_size>::data.table[block_size][i]);
      x = ceil(log2(static_cast<double>(num)));
    }

    double classic = log2(block_size + 1) + x;

    double hybrid = log2(cutoff + 1);
    if (i >= cut_from && i <= cut_to)
    {
      hybrid += block_size;
    }
    else
    {
      hybrid += x;
    }

    total_classic += freq[i] * classic;
    total_hybrid += freq[i] * hybrid;
  }

  double size = static_cast<double>(bv.size());

  std::cout << "<" << block_size << ", " << cutoff << ">"
            << "\t\t" << total_classic / size << "/" << total_hybrid / size
            << "\t" << total_hybrid / total_classic << "\n";
}

int main(int argc, char* argv[])
{
  bit_vector bv;
  if (!load_from_file(bv, argv[1]))
  {
    cout << "Not loaded successfully.\n";
    return 1;
  }

  analyze<31, 7>(bv);
  analyze<31, 15>(bv);
  analyze<63, 7>(bv);
  analyze<63, 15>(bv);
  analyze<63, 31>(bv);
  analyze<127, 7>(bv);
  analyze<127, 15>(bv);
  analyze<127, 31>(bv);
  analyze<127, 63>(bv);

  return 0;
}
