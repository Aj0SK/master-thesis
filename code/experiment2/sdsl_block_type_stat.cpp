#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <vector>

using std::cout;
using std::pair;
using std::vector;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

constexpr size_t kDensity = 30;
constexpr size_t kBlockSize = 31;
constexpr size_t kBlockNumber = 100'000;

std::random_device rd;
std::seed_seq sd{2, 1, 7, 2, 19, 20, 3, 5, 67, 111};
std::mt19937 randomizer(sd);

vector<bool> get_data(size_t size, int density)
{
  std::vector<bool> v(size);
  size_t ones = static_cast<double>(size * density) / 100.0;
  for (size_t i = 0; i < size; ++i)
  {
    if (i < ones)
      v[i] = true;
    else
      v[i] = false;
  }
  std::shuffle(v.begin(), v.end(), randomizer);
  return v;
}
int main()
{
  vector<bool> bv = get_data(kBlockNumber * kBlockSize, kDensity);

  std::map<size_t, size_t> dist;

  for (size_t i = 0; i < bv.size(); i += kBlockSize)
  {
    size_t ones_in_block = 0;
    for (size_t j = i; j < i + kBlockSize; ++j)
    {
      if (bv[j])
        ++ones_in_block;
    }
    dist[ones_in_block] += 1;
  }

  std::cout << "Total number of blocks is " << bv.size() / kBlockSize << "\n";
  for (const auto& [ones, count] : dist)
  {
    std::cout << ones << ": " << count << "\n";
  }

  return 0;
}