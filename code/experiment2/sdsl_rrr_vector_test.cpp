#include <sdsl/rrr_vector.hpp>
#include <iostream>
#include <benchmark/benchmark.h>

using std::vector;
using std::cout;
using std::pair;
using namespace sdsl;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

enum Operation { Access, Rank, Support};

std::pair<vector<bool>, vector<size_t> > get_test(size_t size, size_t queries_count, int density)
{
    std::vector<bool> v(size);
    int ones = static_cast<double>(size*density)/100.0;
    for(size_t i=0; i<size; ++i)
    {
        if(i<ones) v[i] = true;
        else v[i] = false;
    }
    random_shuffle(v.begin(), v.end());

    std::vector<size_t> queries(queries_count);
    for(size_t i=0; i<queries_count; ++i)
    {
        queries[i] = rand() % v.size();
    }
    return {v, queries};
}

template <Operation op, size_t kN, short unsigned int BLOCK_SIZE, int density, size_t RANK_SAMPLE_DENS = 32> static void BM_FUNC(benchmark::State& state)
{
    typedef rrr_vector<BLOCK_SIZE, int_vector<>, RANK_SAMPLE_DENS> rrr_vec_type;
    typedef typename rrr_vec_type::select_1_type rrr_select_type;
    typedef typename rrr_vec_type::rank_1_type rrr_rank_type;

    auto [data, queries] = get_test(kN, 10'000, density);
    bit_vector bv(data.size());
    for(size_t i=0; i<data.size(); ++i)
    {
        bv[i] = data[i];
    }

    rrr_vec_type rrr_vector(bv);
    rrr_select_type rrr_sel(&rrr_vector);
    rrr_rank_type   rrr_rank(&rrr_vector);

    for (auto _ : state)
    {
        for (size_t q : queries) {
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

BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, 10'000, 15, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, 10'000, 31, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, 10'000, 63, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Access, 10'000, 127, 20);

BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, 10'000, 15, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, 10'000, 31, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, 10'000, 63, 20);
BENCHMARK_TEMPLATE(BM_FUNC, Operation::Rank, 10'000, 127, 20);

int main(int argc, char** argv)
{
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}