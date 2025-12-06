#include <cassert>
#include <cstdlib>
#include <fstream>
#include <istream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

using namespace std;

const char* const INPUT_FILE   = "day01.inp";
const char* const EXAMPLE_FILE = "day01.ex";

class Line
{
 public:
  string      str;
  int         line_number{0};

  friend void operator>>(istream& input_stream, Line& line)
  {
    getline(input_stream, line.str);
    ++line.line_number;
  }
};

static vector<vector<string>> read_file(const char* file_name)
{
  ifstream    file(file_name);
  basic_regex regex(R"((L|R)(\d+))");
  return ranges::views::istream<Line>(file) |
         ranges::views::transform(
             [&regex](const Line& line)
             {
               match_results<string::const_iterator> match_results;
               regex_match(line.str, match_results, regex);
               auto to_string = [](auto& match) { return string(match.str()); };
               return match_results | ranges::views::drop(1) | ranges::views::transform(to_string) |
                      ranges::to<vector>();
             }) |
         ranges::to<vector>();
}

const int  DIAL_START       = 50;
const int  DIAL_UPPER_LIMIT = 100;

static int solve1(const char* file_name)
{
  int        dial   = DIAL_START;
  int        zeroes = 0;
  const auto vec    = read_file(file_name) | ranges::views::transform(
                                              [](const auto& line)
                                              {
                                                assert(line.size() == 2);
                                                auto number = stoi(line[1]);
                                                if (line[0][0] == 'L')
                                                {
                                                  number *= -1;
                                                }
                                                return number;
                                              });
  for (auto number : vec)
  {
    dial += number;
    dial %= DIAL_UPPER_LIMIT;
    if (dial == 0)
    {
      ++zeroes;
    }
  }
  return zeroes;
};

static int solve2(const char* file_name)
{
  int dial   = DIAL_START;
  int zeroes = 0;
  for (const auto& line : read_file(file_name))
  {
    assert(line.size() == 2);
    assert(line[0].size() == 1);

    auto number    = stoi(line[1]);
    auto direction = line[0][0];
    if (direction == 'L')
    {
      number *= -1;
    }
    auto remquot         = div(number, DIAL_UPPER_LIMIT);
    zeroes              += abs(remquot.quot);
    const bool was_zero  = dial == 0;
    dial                += remquot.rem;
    if (dial < 0)
    {
      if (!was_zero)
      {
        ++zeroes;
      }
      dial += DIAL_UPPER_LIMIT;
    }
    else if (dial >= DIAL_UPPER_LIMIT)
    {
      ++zeroes;
      dial -= DIAL_UPPER_LIMIT;
    }
    else if (dial == 0)
    {
      ++zeroes;
    }
    assert(dial >= 0);
    assert(dial < DIAL_UPPER_LIMIT);
  }
  return zeroes;
};

struct Answers
{
  int example1;
  int part1;
  int example2;
  int part2;
};

static void submit_answers(Answers correct_answers)
{
  const Answers calculated_answers = {
      .example1 = solve1(EXAMPLE_FILE),
      .part1    = solve1(INPUT_FILE),
      .example2 = solve2(EXAMPLE_FILE),
      .part2    = solve2(INPUT_FILE),
  };
  println("Example 1 part 1: {}", calculated_answers.example1);
  println("Answer    part 1: {}", calculated_answers.part1);
  println("Example 1 part 2: {}", calculated_answers.example2);
  println("Answer    part 2: {}", calculated_answers.part2);
  assert(calculated_answers.example1 == correct_answers.example1);
  assert(calculated_answers.part1 == correct_answers.part1);
  assert(calculated_answers.example2 == correct_answers.example2);
  assert(calculated_answers.part2 == correct_answers.part2);
};

int main(int /*argc*/, char** /*argv*/)
{
  const Answers answers = {.example1 = 3, .part1 = 1052, .example2 = 6, .part2 = 6295};
  submit_answers(answers);
  return 0;
};