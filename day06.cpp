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

int64_t read_file1(const string& file_name)
{
  ifstream                file(file_name);
  string                  str;
  vector<vector<int64_t>> number_rows;
  vector<char>            ops;
  while (getline(file, str))
  {
    stringstream ss(move(str));
    file.peek();
    if (file.eof())
    {
      const istream_iterator<char> ops_iter(ss);
      ops.insert(ops.end(), ops_iter, istream_iterator<char>{});
    }
    else
    {
      number_rows.push_back({});
      const istream_iterator<int64_t> iter(ss);
      number_rows.back().insert(number_rows.back().end(), iter, istream_iterator<int64_t>());
    }
  }
  int64_t         result = 0;
  vector<int64_t> number_col;
  for (size_t col = 0; col < ops.size(); ++col)
  {
    number_col.clear();
    for (size_t row = 0; row < number_rows.size(); ++row)
    {
      number_col.push_back(number_rows[row][col]);
    }
    if (ops[col] == '+')
    {
      result += ranges::fold_left_first(number_col, plus<int64_t>{}).value();
    }
    else
    {
      result += ranges::fold_left_first(number_col, multiplies<int64_t>{}).value();
    }
  }
  return result;
}

int64_t read_file2(const string& file_name)
{
  ifstream       file(file_name);
  vector<string> lines;
  string         str;
  while (getline(file, str))
  {
    lines.emplace_back(str);
  }

  const int       num_col = lines.front().size();
  const int       num_row = lines.size();
  int64_t         result  = 0;
  vector<int64_t> numbers;
  str.clear();
  for (int col = num_col - 1; col >= 0; --col)
  {
    for (int row = 0; row < num_row - 1; ++row)
    {
      str.push_back(lines[row][col]);
    }
    numbers.push_back(stoll(str));
    str.clear();
    char op = lines.back()[col];
    switch (op)
    {
      default:
        break;
      case '+':
        result += ranges::fold_left_first(numbers, plus<int64_t>{}).value();
        numbers.clear();
        --col;
        break;
      case '*':
        result += ranges::fold_left_first(numbers, multiplies<int64_t>{}).value();
        numbers.clear();
        --col;
        break;
    }
  }
  return result;
}

int64_t solve1(const string& file_name)
{
  return read_file1(file_name);
}

int64_t solve2(const string& file_name)
{
  return read_file2(file_name);
}

int main()
{
  const string  INPUT_FILE{"day06.inp"};
  const string  EXAMPLE_FILE{"day06.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 4277556;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 6503327062445;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 3263827;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 9640641878593;
  assert(part2 == CORRECT_ANSWER_PART2);
};