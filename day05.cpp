#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iterator>
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

void read_file(const string&                   file_name,
               vector<pair<int64_t, int64_t>>& intervals,
               optional<vector<int64_t>*>      ids = nullopt)
{
  ifstream file(file_name);
  string   str;
  getline(file, str);
  while (!str.empty())
  {
    size_t dash_pos       = str.find_first_of('-');
    auto   interval_begin = stoll(str.substr(0, dash_pos));
    auto   interval_end   = stoll(str.substr(dash_pos + 1));
    intervals.push_back({interval_begin, interval_end});
    getline(file, str);
  }
  if (!ids)
  {
    return;
  }
  while (getline(file, str))
  {
    ids.value()->push_back(stoll(str));
  }
}

int64_t solve1(const string& file_name)
{
  vector<pair<int64_t, int64_t>> intervals;
  vector<int64_t>                ids;
  read_file(file_name, intervals, optional(&ids));
  return ranges::count_if(ids,
                          [&intervals](auto id) noexcept
                          {
                            for (const auto& interval : intervals)
                            {
                              if (interval.first <= id && id <= interval.second)
                              {
                                return true;
                              }
                            }
                            return false;
                          });
}

vector<pair<int64_t, int64_t>> combine_intervals(vector<pair<int64_t, int64_t>>& intervals)
{
  vector<pair<int64_t, int64_t>> non_overlaping_intervals;
  while (intervals.size() > 0)
  {
    auto interval1 = intervals.back();
    intervals.pop_back();
    bool add = true;
    for (const auto& interval2 : non_overlaping_intervals)
    {
      if (interval1.second < interval2.first || interval1.first > interval2.second)
      {
        // No overlap
        continue;
      }
      bool beginning_inside =
          interval1.first >= interval2.first && interval1.first <= interval2.second;
      bool end_inside = interval1.second >= interval2.first && interval1.second <= interval2.second;
      if (beginning_inside && end_inside)
      {
        // Interval 1 is fully enclosed by interval 2
        add = false;
        break;
      }
      if (beginning_inside)
      {
        interval1.first = interval2.second + 1;
        continue;
      }
      else if (end_inside)
      {
        interval1.second = interval2.first - 1;
        continue;
      }
      // Interval 1 is fully enclosing interval 2
      intervals.push_back({interval2.second + 1, interval1.second});
      interval1.second = interval2.first - 1;
    }
    if (add)
    {
      non_overlaping_intervals.push_back(interval1);
    }
  }
  return non_overlaping_intervals;
}

int64_t solve2(const string& file_name)
{
  vector<pair<int64_t, int64_t>> intervals;
  read_file(file_name, intervals);
  return ranges::fold_left_first(
             combine_intervals(intervals) |
                 ranges::views::transform([](const auto& interval) noexcept
                                          { return interval.second - interval.first + 1; }),
             plus<int64_t>{})
      .value();
}

int main()
{
  const string  INPUT_FILE{"day05.inp"};
  const string  EXAMPLE_FILE{"day05.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 3;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 885;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 14;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 348115621205535;
  assert(part2 == CORRECT_ANSWER_PART2);
};