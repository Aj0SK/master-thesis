#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sdsl/suffix_arrays.hpp>
#include <string>

using namespace sdsl;
using namespace std;

constexpr int BLOCK_SIZE = 31;
constexpr int CUTOFF = 7;
vector<int> block_stat(BLOCK_SIZE + 1, 0);

template <typename t_wt>
void visualize_wt_rec(const t_wt& wt, typename t_wt::node_type v)
{
  if (!wt.is_leaf(v))
  {
    auto vs = wt.expand(v);
    if (!wt.empty(vs[0]))
    {
      visualize_wt_rec(wt, vs[0]);
    }
    if (!wt.empty(vs[1]))
    {
      visualize_wt_rec(wt, vs[1]);
    }

    int count0 = 0, count1 = 0;
    int index = 0, clas = 0;
    for (auto it = wt.bit_vec(v).begin(); it != wt.bit_vec(v).end(); ++it)
    {
      if (*it)
        ++clas, ++count1;
      else
        ++count0;
      ++index;
      if (index == BLOCK_SIZE)
      {
        ++block_stat[clas];
        clas = 0;
        index = 0;
      }
    }
    /*std::cout << count0 << " " << count1 << " "
              << static_cast<double>(count0) / static_cast<double>(count1)
              << "\n";*/
  }
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    cout << "Usage " << argv[0]
         << " text_file [max_locations] [post_context] [pre_context]" << endl;
    cout << "    This program constructs a very compact FM-index" << endl;
    cout << "    which supports count, locate, and extract queries." << endl;
    cout << "    text_file      Original text file." << endl;
    cout << "    max_locations  Maximal number of location to report." << endl;
    cout << "    post_context   Maximal length of the reported post-context."
         << endl;
    cout << "    pre_context    Maximal length of the pre-context." << endl;
    return 1;
  }
  size_t max_locations = 5;
  size_t post_context = 10;
  size_t pre_context = 10;
  if (argc >= 3)
  {
    max_locations = atoi(argv[2]);
  }
  if (argc >= 4)
  {
    post_context = atoi(argv[3]);
  }
  if (argc >= 5)
  {
    pre_context = atoi(argv[4]);
  }
  string index_suffix = ".fm9";
  string index_file = string(argv[1]) + index_suffix;
  csa_wt<wt_huff<rrr_vector<BLOCK_SIZE, int_vector<>, 32, CUTOFF>>, 1 << 20,
         1 << 20>
      fm_index;

  if (!load_from_file(fm_index, index_file))
  {
    ifstream in(argv[1]);
    if (!in)
    {
      cout << "ERROR: File " << argv[1] << " does not exist. Exit." << endl;
      return 1;
    }
    cout << "No index " << index_file << " located. Building index now."
         << endl;
    construct(fm_index, argv[1], 1);     // generate index
    store_to_file(fm_index, index_file); // save it
  }

  std::cout << "Size in bytes is " << size_in_mega_bytes(fm_index);

  // auto wt = fm_index.wavelet_tree;
  // visualize_wt_rec(wt, wt.root());

  // for (int i = 0; i <= BLOCK_SIZE; ++i)
  //   std::cout << i << " " << block_stat[i] << "\n";
}
