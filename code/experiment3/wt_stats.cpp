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
    double classic =
        log2(block_size + 1) +
        ceil(
            log2(binomial_coefficients<block_size>::data.table[block_size][i]));

    double hybrid = log2(cutoff + 1);
    if (i >= cut_from && i <= cut_to)
    {
      hybrid += block_size;
    }
    else
    {
      hybrid += ceil(
          log2(binomial_coefficients<block_size>::data.table[block_size][i]));
    }

    total_classic += freq[i] * classic;
    total_hybrid += freq[i] * hybrid;
  }

  double size = static_cast<double>(bv.size());

  std::cout << "<" << block_size << ", " << cutoff << ">"
            << "\t\t" << total_classic / size << "/" << total_hybrid / size
            << "\n";
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

  return 0;
}
