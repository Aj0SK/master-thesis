#include <iostream>
#include <sdsl/rrr_vector.hpp>
#include <string>

using namespace std;
using namespace sdsl;

constexpr size_t kSize = 1'000'000;

int main()
{
  typedef rrr_vector<31, int_vector<>, 32> rrr_vec_type1;
  typedef rrr_vec_type1::select_1_type rrr_select_type1;
  // typedef rrr_vec_type1::rank_1_type rrr_rank_type1;

  typedef rrr_vector<127, int_vector<>, 32> rrr_vec_type2;
  typedef rrr_vec_type2::select_1_type rrr_select_type2;
  // typedef rrr_vec_type2::rank_1_type rrr_rank_type2;

  bit_vector bv;
  bv.resize(kSize);
  for (size_t i = 0; i < kSize; ++i)
  {
    bv[i] = (i % 2) == 0;
  }

  rrr_vec_type1 rrr1(bv);
  rrr_select_type1 rrr_sel1(&rrr1);
  // rrr_rank_type1 rrr_rank1(&rrr1);

  rrr_vec_type2 rrr2(bv);
  rrr_select_type2 rrr_sel2(&rrr2);
  // rrr_rank_type2 rrr_rank2(&rrr2);

  for (int i = 1; i < 100; ++i)
  {
    if (rrr_sel1.select(i) != rrr_sel2.select(i))
    {
      std::cout << "Problem on " << i << "\n";
      std::cout << rrr_sel1.select(i) << " vs " << rrr_sel2.select(i) << "\n";
      return 1;
    }
  }

  return 0;
}
