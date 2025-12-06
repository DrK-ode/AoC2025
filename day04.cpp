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

struct Coords
{
  int x = 0;
  int y = 0;
};

template <>
struct std::formatter<Coords> : formatter<string_view>
{
  auto format(const Coords& c, std::format_context& ctx) const
  {
    string temp;
    format_to(std::back_inserter(temp), "({}, {})", c.x, c.y);
    return formatter<string_view>::format(temp, ctx);
  }
};

template <typename T>
class Map;

template <typename T>
struct std::formatter<Map<T>>;

template <typename T>
class Map
{
 public:
  Map(const string&           file_name,
      const function<T(char)> item_parser,
      optional<T>             border_opt = nullopt)
      : border(border_opt)
  {
    ifstream   file(file_name);
    const auto insert_data_row = [this, item_parser](const string& str)
    {
      if (this->border)
      {
        this->data.push_back(this->border.value());
      }
      this->data.insert_range(this->data.end(), str | ranges::views::transform(item_parser));
      if (this->border)
      {
        this->data.push_back(this->border.value());
      }
    };
    const auto insert_border_row = [this]()
    { this->data.insert(this->data.end(), this->width, this->border.value()); };
    string str;
    this->height = 0;
    // Read first line to know width
    getline(file, str);
    this->width = str.size();
    ++this->height;
    if (border_opt)
    {
      this->width += 2;
      ++this->height;
      insert_border_row();
    }
    this->data.reserve(this->width * this->width);
    insert_data_row(str);
    // Read the rest of the lines
    while (getline(file, str))
    {
      ++this->height;
      insert_data_row(str);
    }
    // End with border
    if (border_opt)
    {
      ++this->height;
      insert_border_row();
    }
    if (this->border)
    {
      this->coords_begin_indices = {1, 1};
      this->coords_end_indices   = {this->width - 1, this->height - 1};
    }
    else
    {
      this->coords_begin_indices = {0, 0};
      this->coords_end_indices   = {this->width, this->height};
    }
  };
  template <typename U>
  U fold(U start_value, const function<U(U, Coords)> folding_function)
  {
    U value = start_value;
    for (Coords c = this->coords_begin_indices; c.y < coords_end_indices.y; ++c.y)
    {
      for (c.x = this->coords_begin_indices.x; c.x < coords_end_indices.x; ++c.x)
      {
        value = folding_function(value, c);
      }
    }
    return value;
  }
  void for_each(const function<void(Coords)> function)
  {
    for (Coords c = this->coords_begin_indices; c.y < coords_end_indices.y; ++c.y)
    {
      for (c.x = this->coords_begin_indices.x; c.x < coords_end_indices.x; ++c.x)
      {
        function(c);
      }
    }
  }
  T operator[](Coords coords) const
  {
    return this->data[coords.y * this->width + coords.x];
  };
  T& operator[](Coords coords)
  {
    return this->data[coords.y * this->width + coords.x];
  };
  Coords get_coords_begin_indices() const
  {
    return this->coords_begin_indices;
  };
  Coords get_coords_end_indices() const
  {
    return this->coords_end_indices;
  };
  int get_height() const
  {
    return this->height;
  };
  int get_width() const
  {
    return this->width;
  };

 private:
  int         width                = 0;
  int         height               = 0;
  Coords      coords_begin_indices = {.x = 0, .y = 0};
  Coords      coords_end_indices   = {.x = 0, .y = 0};
  optional<T> border               = nullopt;
  vector<T>   data;
};

template <typename T>
  requires formattable<T, char>
struct formatter<Map<T>> : formatter<string_view>
{
  auto format(const Map<T>& map, std::format_context& ctx) const
  {
    string temp;
    format_to(std::back_inserter(temp), "Map size: {} x {}\n", map.get_width(), map.get_height());
    for (Coords c =
             {
                 0,
             };
         c.y < map.get_height();
         ++c.y)
    {
      for (c.x = 0; c.x < map.get_width(); ++c.x)
      {
        format_to(std::back_inserter(temp), "{}", map[c]);
      }
      if (c.y != map.get_height() - 1)
      {
        temp.push_back('\n');
      }
    }
    return formatter<string_view>::format(temp, ctx);
  }
};

int64_t solve1(const string& file_name)
{
  const auto item_parser = [](char c) noexcept
  {
    switch (c)
    {
      case '@':
        return 1;
      default:
        return 0;
    }
  };
  Map<int>   map(file_name, item_parser, optional<char>(0));
  auto const count_neighbours = [&map](int accessible, Coords center) -> int
  {
    if (map[center] == 1)
    {
      int    sum = 0;
      Coords c;
      for (c.x = center.x - 1; c.x <= center.x + 1; ++c.x)
      {
        for (c.y = center.y - 1; c.y <= center.y + 1; ++c.y)
        {
          sum += map[c];
        }
      }
      if (sum < 5)
      {
        ++accessible;
      }
    }
    return accessible;
  };
  int accessible = map.fold<int>(0, count_neighbours);
  return accessible;
}

int64_t solve2(const string& file_name)
{
  const auto item_parser = [](char c) noexcept
  {
    switch (c)
    {
      case '@':
        return 1;
      default:
        return 0;
    }
  };
  Map<int>       map(file_name, item_parser, optional<char>(0));
  vector<Coords> removable;
  auto const     count_neighbours = [&map, &removable](Coords center)
  {
    if (map[center] == 1)
    {
      int    sum = 0;
      Coords c;
      for (c.x = center.x - 1; c.x <= center.x + 1; ++c.x)
      {
        for (c.y = center.y - 1; c.y <= center.y + 1; ++c.y)
        {
          sum += map[c];
        }
      }
      if (sum < 5)
      {
        removable.push_back(center);
      }
    }
  };
  int total_removed = 0;
  int removed;
  do
  {
    removable.clear();
    map.for_each(count_neighbours);
    removed = removable.size();
    for (const auto c : removable)
    {
      map[c] = 0;
    }
    total_removed += removed;
  } while (removed > 0);

  return total_removed;
}

int main()
{
  const string  INPUT_FILE{"day04.inp"};
  const string  EXAMPLE_FILE{"day04.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 13;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 1578;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 43;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 10132;
  assert(part2 == CORRECT_ANSWER_PART2);
};