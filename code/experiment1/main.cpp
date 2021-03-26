// https://github.com/simongog/sdsl-lite/blob/master/examples/bit-vector.cpp
#include <algorithm>
#include <benchmark/benchmark.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <vector>

using namespace sdsl;

using std::cout;
using std::vector;

double calculate_entropy(const vector<bool>& input)
{
  double c0 = 0, c1 = 0;
  for (bool x : input)
  {
    if (x)
      ++c0;
    else
      ++c1;
  }
  double n = input.size();
  return (c0 / n) * log2(n / c0) + (c1 / n) * log2(n / c1);
}

bit_vector get_bit_vector(size_t n, size_t density)
{
  vector<bool> vec;
  for (size_t i = 0; i < n * density / 100ull; ++i)
    vec.push_back(true);
  while (vec.size() != n)
    vec.push_back(false);

  random_shuffle(vec.begin(), vec.end());

  bit_vector bv(n, 0);
  for (size_t i = 0; i < n; ++i)
    if (vec[i])
      bv[i] = 1;
  return bv;
}

vector<bool> generate_random_bits(size_t n)
{
  // TODO: meh, asi to chce byt static
  std::mt19937 rnd{std::random_device()()};
  std::bernoulli_distribution dist(0.5);
  std::vector<bool> bits;
  std::generate_n(std::back_inserter(bits), n, [&] { return dist(rnd); });
  return bits;
}

template <size_t kN, size_t kDensity, size_t kB, size_t kSB>
void BM_Select(benchmark::State& state)
{
  auto bv = get_bit_vector(kN, kDensity);
  rrr_vector<kB, int_vector<>, kSB> rrr(bv);
  typename rrr_vector<kB>::select_1_type select_rrr(&rrr);
  srand(0);
  for (auto _ : state)
  {
    auto x = select_rrr(rand() % kN);
    benchmark::DoNotOptimize(x);
  }
}
BENCHMARK_TEMPLATE(BM_Select, 1 << 28, 5, 63, 32);

BENCHMARK_MAIN();

/*int main()
{
  // auto vec = generate_random_bits(1'000);


  rrr_vector<kB, int_vector<>, kSB> rrr(bv);

  cout << "Bit-vector of size " << bv.size() << "\n";
  cout << calculate_entropy(vec) << "\n";

  cout << "Size is " << 8 * size_in_bytes(rrr) << "\n";
  double ratio = 8.0 * size_in_bytes(rrr) / static_cast<double>(kN);
  cout << "Ratio is " << ratio << "\n";

  return 0;
}*/
