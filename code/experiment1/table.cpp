#include <algorithm>
#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::vector;

constexpr size_t kN = 1 << 3;
constexpr size_t kC = 4;

vector<vector<int>> create_helper(size_t n, size_t c)
{
  if (c >= n)
  {
    cerr << "Bad c in create_helper."
         << "\n";
    exit(0);
  }
  vector<int> arr;
  arr.reserve(n);

  for (size_t i = 0; i < n - c; ++i)
    arr.push_back(0);
  for (size_t i = 0; i < c; ++i)
    arr.push_back(1);

  vector<vector<int>> out;
  do
  {
    out.push_back(arr);
  } while (next_permutation(arr.begin(), arr.end()));

  return out;
}

int main()
{
  auto helper = create_helper(kN, kC);
  for (auto& arr : helper)
  {
    for (auto x : arr)
    {
      cout << x;
    }
    cout << "\n";
  }
  return 0;
}