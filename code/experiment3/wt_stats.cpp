#include <algorithm>
#include <fstream>
#include <iostream>
#include <sdsl/rrr_vector.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace sdsl;

const int kBlockSize = 31;

using namespace std;

int main(int argc, char* argv[])
{
  bit_vector bv;
  if (!load_from_file(bv, argv[1]))
  {
    cout << "Not loaded successfully.\n";
    return 1;
  }

  vector<int> freq(kBlockSize + 1, 0);

  for (int i = 0; i < bv.size(); i += kBlockSize)
  {
    int count = 0;
    for (int j = i; j < min((int)bv.size(), i + kBlockSize); ++j)
    {
      if (bv[j])
        ++count;
    }
    ++freq[count];
  }

  size_t sum = 0;
  for (auto x : freq)
    sum += x;

  std::cout << "Frequencies for block size " << kBlockSize << "\n";
  for (int i = 0; i < kBlockSize + 1; ++i)
  {
    std::cout << i << " : " << freq[i] << " "
              << ((double)freq[i]) / ((double)sum) << "\n";
  }

  return 0;
}
