#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <vector>

using std::cout;
using std::pair;
using std::vector;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

std::random_device rd;
std::seed_seq sd{2, 1, 7, 2, 19, 20, 3, 4, 5, 67, 111};
std::mt19937 randomizer(sd);

vector<bool> get_data(size_t size, int density)
{
  std::vector<bool> v(size);
  size_t ones = (size / 100) * density;
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

uint64_t nCr(uint64_t n, uint64_t k)
{
  if (k > n)
    return 0;

  uint64_t r = 1;
  for (uint64_t d = 1; d <= k; ++d)
  {
    r *= n--;
    r /= d;
  }
  return r;
}

std::pair<double, double> f(size_t kDensity, size_t kBlockSize,
                            size_t kBlockNumber, size_t kHybridCutOff)
{
  size_t kSize = kBlockNumber * kBlockSize;

  if ((kSize % 100) != 0)
  {
    exit(1);
  }

  vector<bool> bv = get_data(kSize, kDensity);
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

  double expected_length = ceil(log2(kBlockSize));
  for (const auto& [ones, count] : dist)
  {
    double block_probability =
        static_cast<double>(count) / static_cast<double>(kBlockNumber);
    double block_repr_size = ceil(log2(nCr(kBlockSize, ones)));
    expected_length += block_probability * block_repr_size;
  }
  expected_length *= static_cast<double>(kBlockNumber);
  expected_length /= static_cast<double>(kSize);
  /*std::cout << "Expected length per bit is " << expected_length << "\n";*/

  double expected_length_hybrid = ceil(log2(kHybridCutOff + 1));
  for (const auto& [ones, count] : dist)
  {
    double block_probability =
        static_cast<double>(count) / static_cast<double>(kBlockNumber);
    double block_repr_size = (ones <= kHybridCutOff)
                                 ? ceil(log2(nCr(kBlockSize, ones)))
                                 : kBlockSize;
    expected_length_hybrid += block_probability * block_repr_size;
  }
  expected_length_hybrid *= static_cast<double>(kBlockNumber);
  expected_length_hybrid /= static_cast<double>(kSize);
  /*std::cout << "Expected hybrid length per bit is " << expected_length_hybrid
            << "\n";*/

  return {expected_length, expected_length_hybrid};
}

int main()
{
  constexpr size_t kTestRounds = 1'000;

  constexpr size_t kDensity = 5;
  constexpr size_t kBlockSize = 31;
  constexpr size_t kBlockNumber = 1'000;
  // constexpr size_t kHybridCutOff = 5;

  for (size_t kHybridCutOff = 1; kHybridCutOff < 20; ++kHybridCutOff)
  {
    double original_total = 0.0, hybrid_total = 0.0;
    for (size_t i = 0; i < kTestRounds; ++i)
    {
      auto [original, hybrid] =
          f(kDensity, kBlockSize, kBlockNumber, kHybridCutOff);
      original_total += original;
      hybrid_total += hybrid;
    }
    original_total /= static_cast<double>(kTestRounds);
    hybrid_total /= static_cast<double>(kTestRounds);

    std::cout << kHybridCutOff << ": " << original_total << "  /\t"
              << hybrid_total << "\n";
  }
  // std::cout << '\n';

  return 0;
}