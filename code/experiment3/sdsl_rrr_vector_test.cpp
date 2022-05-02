#include <benchmark/benchmark.h>
#include <iostream>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/sd_vector.hpp>

using std::cout;
using std::pair;
using std::vector;
using namespace sdsl;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

constexpr int kDensity = 5;
constexpr int kSeqLen = 1 << 25;

enum Operation
{
  Access,
  Rank,
  Select
};
enum AccessPattern
{
  Random
};

template <Operation op, AccessPattern ap>
std::pair<vector<bool>, vector<size_t>>
get_test(size_t size, size_t queries_count, int density)
{
  std::random_device rd;
  std::seed_seq sd{2, 1, 7, 2, 19, 20, 3, 5, 67, 111};
  std::mt19937 randomizer(sd);

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
  if constexpr (ap == AccessPattern::Random && op != Operation::Select)
  {
    for (auto& q : queries)
      q = randomizer() % v.size();
  }
  if constexpr (ap == AccessPattern::Random && op == Operation::Select)
  {
    for (auto& q : queries)
      q = 1 + randomizer() % ones;
  }

  return {v, queries};
}

template <Operation op, AccessPattern ap, size_t kN, int density,
          short unsigned int BLOCK_SIZE>
static void BM_SD(benchmark::State& state)
{
  using vec_type = sd_vector<>;
  using vector_select_type = typename vec_type::select_1_type;
  using vector_rank_type = typename vec_type::rank_1_type;

  auto [data, queries] = get_test<op, ap>(kN, 10'000, density);

  bit_vector bv(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    bv[i] = data[i];
  }

  sd_vector sd_vector(bv);
  vector_rank_type sd_rank(&sd_vector);
  vector_select_type sd_select(&sd_vector);

  for (auto _ : state)
  {
    for (size_t q : queries)
    {
      if constexpr (op == Operation::Access)
      {
        bool x = sd_vector[q];
        benchmark::DoNotOptimize(x);
      }
      if constexpr (op == Operation::Rank)
      {
        bool x = sd_rank(q);
        benchmark::DoNotOptimize(x);
      }
      if constexpr (op == Operation::Select)
      {
        bool x = sd_select(q);
        benchmark::DoNotOptimize(x);
      }
    }
  }

  double ratio = 8.0 * size_in_bytes(sd_vector) / static_cast<double>(kN);
  state.counters["Space"] = ratio;
}

template <Operation op, AccessPattern ap, size_t kN, int density,
          short unsigned int BLOCK_SIZE, size_t RANK_SAMPLE_DENS = 32,
          int hybrid = BLOCK_SIZE>
static void BM_RRR(benchmark::State& state)
{
  using rrr_vec_type =
      rrr_vector<BLOCK_SIZE, int_vector<>, RANK_SAMPLE_DENS, hybrid>;
  using rrr_select_type = typename rrr_vec_type::select_1_type;
  using rrr_rank_type = typename rrr_vec_type::rank_1_type;

  auto [data, queries] = get_test<op, ap>(kN, 10'000, density);

  bit_vector bv(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    bv[i] = data[i];
  }

  rrr_vec_type rrr_vector(bv);
  rrr_rank_type rrr_rank(&rrr_vector);
  rrr_select_type rrr_select(&rrr_vector);
  // int checksum = 0;

  for (auto _ : state)
  {
    // checksum = 0;
    for (size_t q : queries)
    {
      bool x;
      if constexpr (op == Operation::Access)
      {
        x = rrr_vector[q];
      }
      if constexpr (op == Operation::Rank)
      {
        x = rrr_rank(q);
      }
      if constexpr (op == Operation::Select)
      {
        x = rrr_select(q);
      }
      benchmark::DoNotOptimize(x);
      // checksum += x;
    }
  }

  // std::cout << "Control checksum is " << checksum << "\n";

  double ratio = 8.0 * size_in_bytes(rrr_vector) / static_cast<double>(kN);
  state.counters["Space"] = ratio;
}

BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 31)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 31, 32, 7)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 63)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 63, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 127)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 127, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_SD, Operation::Access, AccessPattern::Random, kSeqLen,
                   kDensity, 0)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 31)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 31, 32, 7)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 63)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 63, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 127)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 127, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_SD, Operation::Rank, AccessPattern::Random, kSeqLen,
                   kDensity, 0)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 31)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 31, 32, 7)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 63)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 63, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 127)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_RRR, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 127, 32, 15)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_SD, Operation::Select, AccessPattern::Random, kSeqLen,
                   kDensity, 0)
    ->Unit(benchmark::kNanosecond);

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}
