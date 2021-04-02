#include <algorithm>
#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::vector;

constexpr size_t kN = 1 << 3;
constexpr size_t kC = 4;

constexpr size_t nCr(size_t N, size_t K)
{
  // This function gets the total number of unique combinations based upon N and
  // K. N is the total number of items. K is the size of the group. Total number
  // of unique combinations = N! / ( K! (N - K)! ). This function is less
  // efficient, but is more likely to not overflow when N and K are large. Taken
  // from:  http://blog.plover.com/math/choose.html
  if (K > N)
    return 0;
  size_t r = 1;
  for (size_t d = 1; d <= K; ++d)
  {
    r *= N--;
    r /= d;
  }
  return r;
}

vector<vector<bool>> create_helper(size_t n, size_t c)
{
  if (c >= n)
  {
    cerr << "Bad c in create_helper."
         << "\n";
    exit(0);
  }
  vector<bool> arr;
  arr.reserve(n);

  for (size_t i = 0; i < n - c; ++i)
    arr.push_back(0);
  for (size_t i = 0; i < c; ++i)
    arr.push_back(1);

  vector<vector<bool>> out;
  do
  {
    out.push_back(arr);
  } while (next_permutation(arr.begin(), arr.end()));

  return out;
}

vector<bool> get_ith(size_t n, size_t c, size_t target)
{
  vector<bool> out;
  for (size_t i = 0; i < n; ++i)
  {
    size_t bits_to_right = n - i - 1;
    size_t x = nCr(bits_to_right, c);
    if (target > x)
    {
      --c;
      out.push_back(1);
      target -= x;
    }
    else
    {
      out.push_back(0);
    }
  }

  return out;
}

int main()
{
  {
    // some tests
    static_assert(nCr(12, 4) == 495);
    static_assert(nCr(45, 10) == 3'190'187'286);
    static_assert(nCr(30, 3) == 4'060);
    static_assert(nCr(25, 7) == 480'700);
    size_t kTestN = 13;
    size_t kTestC = 3;

    auto helper = create_helper(kTestN, kTestC);
    for (size_t i = 0; i < helper.size(); ++i)
    {
      if (helper[i] != get_ith(kTestN, kTestC, i + 1))
      {
        cout << "Error in get_ith"
             << "\n";
        return 0;
      }
    }
  }

  return 0;
}