#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using chrono::duration_cast;
using chrono::high_resolution_clock;
using chrono::microseconds;

auto read_file(const string& file_name) -> vector<vector<int>>
{
  ifstream            file(file_name);
  vector<vector<int>> lines;
  string              str;
  while (getline(file, str))
  {
    const basic_regex regex(R"((\d))");
    const auto        match_begin    = sregex_iterator(str.begin(), str.end(), regex);
    const auto        match_end      = sregex_iterator();
    const auto        match_to_digit = [](const auto& match) { return stoi(match.str()); };
    lines.emplace_back(ranges::subrange(match_begin, match_end) |
                       ranges::views::transform(match_to_digit) | ranges::to<vector>());
  }
  return lines;
};

int64_t solve(const string& file_name, int n)
{
  vector<int> batteries_on;
  batteries_on.resize(n);
  const auto joltage = [n, &batteries_on](auto& batteries_all)
  {
    auto search_start = batteries_all.begin();
    for (int battery_idx = 0; battery_idx < n; ++battery_idx)
    {
      search_start              = max_element(search_start, batteries_all.end() + battery_idx - n);
      batteries_on[battery_idx] = *search_start++;
    }
    return ranges::fold_left_first(batteries_on,
                                   [](int64_t jolt_sum, int64_t jolt) noexcept
                                   { return jolt_sum * 10 + jolt; })
        .value();
  };
  return ranges::fold_left_first(read_file(file_name) | ranges::views::transform(joltage),
                                 std::plus<>{})
      .value();
}

int64_t solve1(const string& file_name)
{
  return solve(file_name, 2);
}

int64_t solve2(const string& file_name)
{
  return solve(file_name, 12);
}

int main()
{
  const string  INPUT_FILE{"day03.inp"};
  const string  EXAMPLE_FILE{"day03.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 357;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 16927;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 3121910778619;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 167384358365132;
  assert(part2 == CORRECT_ANSWER_PART2);
};