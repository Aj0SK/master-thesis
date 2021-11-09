#include <iostream>
#include <sdsl/rrr_vector.hpp>

using std::cout;
using std::pair;
using std::vector;
using namespace sdsl;

std::random_device rd;
std::seed_seq sd{45};
std::mt19937 randomizer(sd);

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
  std::shuffle(v.begin(), v.end(), randomizer);

  std::vector<size_t> queries(queries_count);
  for (auto& q : queries)
    q = rand() % v.size();
  return {v, queries};
}

int main(int argc, char** argv)
{
  using rrr_vec_type = rrr_vector<31, int_vector<>, 32>;
  using rrr_select_type = typename rrr_vec_type::select_1_type;
  using rrr_rank_type = typename rrr_vec_type::rank_1_type;

  // vector<bool> data;
  // vector<size_t> queries;

  size_t density = 50;

  auto [data, queries] = get_test(1024, 10, density);

  bit_vector bv(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    bv[i] = data[i];
  }

  rrr_vec_type rrr_vector(bv);

  std::cout << "Expected: ";
  for (size_t i = 0; i < 31; ++i)
    std::cout << data[i];
  std::cout << "\n";

  for (size_t i = 0; i < data.size(); ++i)
  {
    if (data[i] != rrr_vector[i])
    {
      std::cout << "Problem on " << i << "\n";
      exit(1);
    }
  }
  std::cout << "successfully ended!\n";

  // rrr_select_type rrr_sel(&rrr_vector);
  // rrr_rank_type rrr_rank(&rrr_vector);
  return 0;
}