#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <print>
#include <ranges>
#include <string>
#include <valarray>
#include <vector>

using namespace std;
using chrono::duration_cast;
using chrono::high_resolution_clock;
using chrono::microseconds;

enum Dim
{
  X = 0,
  Y = 1,
} Dim;

enum class Direction
{
  UP,
  RIGHT,
  DOWN,
  LEFT,
  UNKNOWN,
};

using Coordinates = valarray<int64_t>;

vector<Coordinates> read_file(const string& file_name)
{
  const int           ndim = 2;
  vector<Coordinates> cvec;
  ifstream            file(file_name);
  string              str;
  const char          divider = ',';
  while (getline(file, str))
  {
    stringstream ss(str);
    int64_t      c[ndim];
    for (int i = 0; i < ndim; ++i)
    {
      getline(ss, str, divider);
      c[i] = stoll(str);
    }
    cvec.emplace_back(Coordinates{c, ndim});
  }
  return cvec;
}

class Rectangle
{
 public:
  Rectangle(const Coordinates a, const Coordinates b)
      : top(max(a[Dim::Y], b[Dim::Y])),
        right(max(a[Dim::X], b[Dim::X])),
        bottom(min(a[Dim::Y], b[Dim::Y])),
        left(min(a[Dim::X], b[Dim::X]))
  {
  }
  int64_t area() const
  {
    return (right - left + 1) * (top - bottom + 1);
  }
  int64_t top;
  int64_t right;
  int64_t bottom;
  int64_t left;
};

class LineSegment
{
 public:
  LineSegment(const Coordinates& a, const Coordinates& b)
      : start(a), stop(b)
  {
  }
  Direction direction() const
  {
    auto compare_x = start[Dim::X] <=> stop[Dim::X];
    if (compare_x == strong_ordering::less)
    {
      return Direction::RIGHT;
    }
    if (compare_x == strong_ordering::greater)
    {
      return Direction::LEFT;
    }
    auto compare_y = start[Dim::Y] <=> stop[Dim::Y];
    if (compare_y == strong_ordering::less)
    {
      return Direction::UP;
    }
    if (compare_y == strong_ordering::greater)
    {
      return Direction::DOWN;
    }
    return Direction::UNKNOWN;
  }
  const Coordinates start;
  const Coordinates stop;
};

vector<Rectangle> get_rectangles(const vector<Coordinates>& coordinates_vector)
{
  vector<Rectangle> rectangles;
  rectangles.reserve(coordinates_vector.size() * (coordinates_vector.size() - 1) / 2);
  for (const auto& [i, c] : coordinates_vector | ranges::views::enumerate)
  {
    for (const auto& d : coordinates_vector | ranges::views::drop(i + 1))
    {
      rectangles.push_back(Rectangle{c, d});
    }
  }
  return rectangles;
}

int64_t solve1(const string& file_name)
{
  const auto coordinates  = read_file(file_name);
  int64_t    biggest_area = 0;
  for (const auto& [i, c] : coordinates | ranges::views::enumerate)
  {
    for (const auto& d : coordinates | ranges::views::drop(i + 1))
    {
      Rectangle  r{c, d};
      const auto area = r.area();
      if (area > biggest_area)
      {
        biggest_area = area;
      }
    }
  }
  return biggest_area;
}

vector<LineSegment> get_line_segments(const vector<Coordinates>& coordinates_vector,
                                      bool&                      is_clockwise)
{
  vector<LineSegment> lines;
  lines.reserve(coordinates_vector.size());
  auto last_dir  = Direction::UNKNOWN;
  int  clockwise = 0;
  for (const auto& [start, stop] :
       ranges::views::concat(coordinates_vector, vector{coordinates_vector[0]}) |
           ranges::views::adjacent<2> | ranges::views::take(coordinates_vector.size()))
  {
    lines.push_back(LineSegment{start, stop});
    auto current_dir = lines.back().direction();
    if ((last_dir == Direction::UP && current_dir == Direction::RIGHT) ||
        (last_dir == Direction::RIGHT && current_dir == Direction::DOWN) ||
        (last_dir == Direction::DOWN && current_dir == Direction::LEFT) ||
        (last_dir == Direction::LEFT && current_dir == Direction::UP))
    {
      ++clockwise;
    }
    else
    {
      --clockwise;
    }
    last_dir = current_dir;
  }
  is_clockwise = clockwise > 0;
  return lines;
}

// Caveats:
//   - Rectangles with unit width or height will always be discarded
//   - Lines are assumed not to be drawn directly next to each other
bool overlap(const Rectangle& r, const LineSegment& ls, bool left_is_outside)
{
  const auto miny = min(ls.start[Dim::Y], ls.stop[Dim::Y]);
  const auto maxy = max(ls.start[Dim::Y], ls.stop[Dim::Y]);
  const auto minx = min(ls.start[Dim::X], ls.stop[Dim::X]);
  const auto maxx = max(ls.start[Dim::X], ls.stop[Dim::X]);
  if (miny >= r.top || maxy <= r.bottom || minx >= r.right || maxx <= r.left)
  {
    return false;
  }
  const auto direction = ls.direction();
  if (ls.start[Dim::X] == ls.stop[Dim::X])  // Vertical
  {
    if ((ls.start[Dim::X] == r.left && ((direction == Direction::UP && left_is_outside) ||
                                        (direction == Direction::DOWN && !left_is_outside))) ||
        (ls.start[Dim::X] == r.right && ((direction == Direction::UP && !left_is_outside) ||
                                         (direction == Direction::DOWN && left_is_outside))))
    {
      return false;
    }
  }
  else  // Horisontal
  {
    if ((ls.start[Dim::Y] == r.bottom && ((direction == Direction::LEFT && left_is_outside) ||
                                          (direction == Direction::RIGHT && !left_is_outside))) ||
        (ls.start[Dim::Y] == r.top && ((direction == Direction::LEFT && left_is_outside) ||
                                       (direction == Direction::RIGHT && !left_is_outside))))
    {
      return false;
    }
  }
  return true;
}

int64_t biggest_area(const vector<Coordinates>& coordinates,
                     const vector<LineSegment>& line_segments,
                     bool                       left_is_outside)
{
  auto biggest_area = 0;
  for (const auto& [i, c] : coordinates | ranges::views::enumerate)
  {
    for (const auto& d : coordinates | ranges::views::drop(i + 1))
    {
      Rectangle  r{c, d};
      const auto area = r.area();
      if (area < biggest_area)
      {
        continue;
      }
      bool overlapping = false;
      for (const auto& ls : line_segments)
      {
        overlapping = overlap(r, ls, left_is_outside);
        if (overlapping)
        {
          break;
        }
      }
      if (!overlapping)
      {
        biggest_area = area;
      }
    }
  }
  return biggest_area;
}

int64_t solve2(const string& file_name)
{
  const auto the_coordinates = read_file(file_name);
  bool       left_is_outside;
  const auto the_line_segments = get_line_segments(the_coordinates, left_is_outside);
  return biggest_area(the_coordinates, the_line_segments, left_is_outside);
}

int main()
{
  const string INPUT_FILE{"day09.inp"};
  const string EXAMPLE_FILE{"day09.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 50;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 4767418746;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 24;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 1461987144;
  assert(part2 == CORRECT_ANSWER_PART2);
};