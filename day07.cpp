#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using chrono::duration_cast;
using chrono::high_resolution_clock;
using chrono::microseconds;

vector<string> read_file(const string& file_name)
{
  ifstream       file(file_name);
  string         str;
  vector<string> lines;
  while (getline(file, str))
  {
    lines.emplace_back(move(str));
  }
  return lines;
}

void update_tachyons(map<int, int64_t>& tachyons, int p, int64_t m)
{
  const auto iter = tachyons.find(p);
  if (iter == tachyons.end())
  {
    tachyons[p] = m;
  }
  else
  {
    tachyons[p] += m;
  }
}

pair<int, int64_t> solve2(const string& file_name)
{
  const auto        lines = read_file(file_name);
  map<int, int64_t> tachyons[2];
  int               current                  = 0;
  tachyons[current][lines.front().find('S')] = 1;
  int splits                                 = 0;
  for (const auto& line : lines)
  {
    for (const auto [p, m] : tachyons[current])
    {
      int next = 1 - current;
      if (line[p] == '^')
      {
        ++splits;
        update_tachyons(tachyons[next], p - 1, m);
        update_tachyons(tachyons[next], p + 1, m);
      }
      else
      {
        update_tachyons(tachyons[next], p, m);
      }
    }
    tachyons[current].clear();
    current = 1 - current;
  }
  return {
      splits,
      ranges::fold_left_first(tachyons[current] | ranges::views::values, plus<int64_t>{}).value()};
}

int main()
{
  const string INPUT_FILE{"day07.inp"};
  const string EXAMPLE_FILE{"day07.ex"};

  const auto [example1, example2] = solve2(EXAMPLE_FILE);

  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 21;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 40;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto start1               = chrono::high_resolution_clock::now();
  const auto [part1, part2] = solve2(INPUT_FILE);
  auto duration1            = high_resolution_clock::now() - start1;
  println("Answer  part 1: {}\nAnswer  part 2: {}\nBoth in: {}",
          part1,
          part2,
          duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 1609;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t CORRECT_ANSWER_PART2 = 12472142047197;
  assert(part2 == CORRECT_ANSWER_PART2);
};