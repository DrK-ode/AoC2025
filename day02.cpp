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

const char* const INPUT_FILE   = "day02.inp";
const char* const EXAMPLE_FILE = "day02.ex";

static const int64_t pow10_array[] = {1,
                                      10,
                                      100,
                                      1000,
                                      10000,
                                      100000,
                                      1000000,
                                      10000000,
                                      100000000,
                                      1000000000,
                                      10000000000,
                                      100000000000,
                                      1000000000000,
                                      10000000000000,
                                      100000000000000,
                                      1000000000000000,
                                      10000000000000000,
                                      100000000000000000,
                                      1000000000000000000,
                                      numeric_limits<int64_t>::max()};

// inline constexpr uint32_t number_of_digits_base_10(uint32_t x)
// {
//   uint32_t floor_log2  = bit_width(x) - 1;
//   uint32_t int_log10   = ((floor_log2 * 1233) >> 12) + 1;
//   int_log10           += __builtin_expect(x <= pow10_array[int_log10], true) ? 0 : 1;
//   return int_log10;
// }

class NumInfo
{
 public:
  NumInfo() : number(0), digits(0)
  {
  }
  NumInfo(string_view strv)
  {
    this->digits = strv.size();
    this->number = stoll(strv.data());
  }
  int64_t number;
  int64_t digits;
};

inline constexpr int64_t pow10(int64_t x)
{
  x = min(x, int64_t(19));
  return pow10_array[x];
}

inline constexpr bool is_even(int64_t x)
{
  return (x & 1) == 0;
}

inline constexpr int64_t first_part_of_int(int64_t x, int64_t front_digits)
{
  int64_t ten_power = pow10(front_digits);
  return x / ten_power;
}

auto read_file(const char* file_name) -> vector<tuple<NumInfo, NumInfo>>
{
  ifstream file(file_name);
  string   str;
  getline(file, str);
  const basic_regex regex(R"((\d+)-(\d+))");
  const auto        match_begin = sregex_iterator(str.begin(), str.end(), regex);
  const auto        match_end   = sregex_iterator();
  const auto        make_tuple  = [](auto& match)
  { return tuple<NumInfo, NumInfo>({match[1].str()}, {match[2].str()}); };
  return ranges::subrange(match_begin, match_end) | ranges::views::transform(make_tuple) |
         ranges::to<vector>();
}

int64_t solve1(const char* file_name)
{
  int64_t sum = 0;
  for (const auto& interval : read_file(file_name))
  {
    auto [begin, end] = interval;
    while (begin.digits <= end.digits && begin.number <= end.number)
    {
      if (is_even(begin.digits))
      {
        int64_t       begin_upper = first_part_of_int(begin.number, begin.digits / 2);
        const int64_t factor      = pow10(begin.digits - begin.digits / 2);
        int64_t       number      = begin_upper * (factor + 1);
        while (begin_upper < factor && number <= end.number)
        {
          if (number >= begin.number)
          {
            sum += number;
          }
          ++begin_upper;
          number = begin_upper * (factor + 1);
        }
      }
      begin.number = pow(10, begin.digits);
      ++begin.digits;
    }
  }
  return sum;
};

constexpr bool has_pattern_n(NumInfo num_info, int64_t n)
{
  int64_t ten_pow       = pow10(n);
  int64_t digit_pattern = num_info.number % ten_pow;
  int64_t sum           = 0;
  for (int64_t i = 0; i < num_info.digits / n; ++i)
  {
    sum = sum * ten_pow + digit_pattern;
  }
  return sum == num_info.number;
}

constexpr vector<int64_t> patterns_to_check(int64_t digits)
{
  switch (digits)
  {
    default:
    case 0:
    case 1:
      return {};
    case 2:
    case 3:
    case 5:
    case 7:
    case 11:
      return {1};
      break;
    case 4:
      return {1, 2};
    case 6:
      return {1, 2, 3};
    case 8:
      return {1, 2, 4};
    case 9:
      return {1, 3};
    case 10:
      return {1, 2, 5};
    case 12:
      return {1, 2, 3, 4, 6};
  }
}

constexpr bool has_pattern(NumInfo num_info)
{
  for (int64_t n : patterns_to_check(num_info.digits))
  {
    if (has_pattern_n(num_info, n))
      return true;
  }
  return false;
}

int64_t solve2(const char* file_name)
{
  int64_t sum = 0;
  for (const auto& interval : read_file(file_name))
  {
    auto [begin, end] = interval;
    NumInfo n(begin);
    int64_t limit = pow10(n.digits);
    do
    {
      if (n.number == limit)
      {
        limit *= 10;
        ++n.digits;
      }
      if (has_pattern(n))
      {
        sum += n.number;
      }
    }
    while (n.number++ <= end.number);
  }
  return sum;
};

void submit_answers(const int64_t correct_example1, const int64_t correct_part1 = 0,
                    const int64_t correct_example2 = 0, const int64_t correct_part2 = 0)
{
  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  assert(example1 == correct_example1);

  auto          t1    = chrono::high_resolution_clock::now();
  const int64_t part1 = solve1(INPUT_FILE);
  auto          t2    = high_resolution_clock::now();
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(t2 - t1));
  assert(part1 == correct_part1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  assert(example2 == correct_example2);

  auto          t3    = chrono::high_resolution_clock::now();
  const int64_t part2 = solve2(INPUT_FILE);
  auto          t4    = chrono::high_resolution_clock::now();
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(t4 - t3));
  assert(part2 == correct_part2);
};

int main(int /*argc*/, char** /*argv*/)
{
  submit_answers(1227775554, 54641809925, 4174379265, 73694270688);
  return 0;
};