#include <benchmark/benchmark.h>
#include <iostream>
#include <sdsl/rrr_vector.hpp>

using std::cout;
using std::pair;
using std::vector;
using namespace sdsl;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

std::random_device rd;
std::seed_seq sd{2, 1, 7, 2, 19, 20, 3, 5, 67, 111};
std::mt19937 randomizer(sd);

enum Operation
{
  Access,
  Rank,
  Support
};
enum AccessPattern
{
  Random,
  ContinuousRandom
};

template <AccessPattern ap>
std::pair<vector<bool>, vector<size_t>>
get_test(size_t size, size_t queries_count, int density)
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
  shuffle(v.begin(), v.end(), randomizer);

  std::vector<size_t> queries(queries_count);
  if constexpr (ap == AccessPattern::Random)
  {
    for (auto& q : queries)
      q = randomizer() % v.size();
  }
  if constexpr (ap == AccessPattern::ContinuousRandom)
  {
    for (size_t i = 0; i != queries_count;)
    {
      int start = randomizer() % v.size();
      for (size_t j = 0; j < 100 || (i != queries_count); ++j)
      {
        queries[i] = (start + j) % v.size();
        ++i;
      }
    }
  }
  return {v, queries};
}

template <Operation op, AccessPattern ap, size_t kN,
          short unsigned int BLOCK_SIZE, int density,
          size_t RANK_SAMPLE_DENS = 32>
static void BM_FUNC(benchmark::State& state)
{
  using rrr_vec_type = rrr_vector<BLOCK_SIZE, int_vector<>, RANK_SAMPLE_DENS>;
  using rrr_select_type = typename rrr_vec_type::select_1_type;
  using rrr_rank_type = typename rrr_vec_type::rank_1_type;

  // vector<bool> data;
  // vector<size_t> queries;

  auto [data, queries] = get_test<ap>(kN, 10'000, density);

  bit_vector bv(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    bv[i] = data[i];
  }

  rrr_vec_type rrr_vector(bv);
  rrr_select_type rrr_sel(&rrr_vector);
  rrr_rank_type rrr_rank(&rrr_vector);

  for (auto _ : state)
  {
    for (size_t q : queries)
    {
      if constexpr (op == Operation::Access)
      {
        bool x = rrr_vector[q];
        benchmark::DoNotOptimize(x);
      }
      if constexpr (op == Operation::Rank)
      {
        bool x = rrr_rank(q);
        benchmark::DoNotOptimize(x);
      }
    }
  }

  double ratio = 8.0 * size_in_bytes(rrr_vector) / static_cast<double>(kN);
  state.counters["Space"] = ratio;
}

BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::Random, 10'000,
                   15, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::Random, 10'000,
                   31, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::Random, 10'000,
                   63, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::Random, 10'000,
                   127, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::Random, 10'000, 15,
                   20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::Random, 10'000, 31,
                   20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::Random, 10'000, 63,
                   20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::Random, 10'000, 127,
                   20);

BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::ContinuousRandom,
                   10'000, 15, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::ContinuousRandom,
                   10'000, 31, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::ContinuousRandom,
                   10'000, 63, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, AccessPattern::ContinuousRandom,
                   10'000, 127, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::ContinuousRandom,
                   10'000, 15, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::ContinuousRandom,
                   10'000, 31, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::ContinuousRandom,
                   10'000, 63, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, AccessPattern::ContinuousRandom,
                   10'000, 127, 20);

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}