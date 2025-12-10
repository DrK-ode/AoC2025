#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <list>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <valarray>
#include <vector>

using namespace std;
using chrono::duration_cast;
using chrono::high_resolution_clock;
using chrono::microseconds;

enum DIMS
{
  X = 0,
  Y = 1,
  Z = 2,
} DIMS;

class Coordinates : public valarray<int64_t>
{
 public:
  using valarray::valarray;
  auto operator<=>(const Coordinates& other) const
  {
    auto x = (*this)[DIMS::X] <=> other[DIMS::X];
    auto y = (*this)[DIMS::Y] <=> other[DIMS::Y];
    auto z = (*this)[DIMS::Z] <=> other[DIMS::Z];
    if (x == strong_ordering::equal)
    {
      if (y == strong_ordering::equal)
      {
        if (z == strong_ordering::equal)
        {
          return strong_ordering::equal;
        }
        return z;
      }
      return y;
    }
    return x;
  };
};

class Connection
{
 public:
  Connection(const Coordinates* a, const Coordinates* b)
      : a_ptr(a), b_ptr(b), distance_squared(calc_distance(*a, *b))
  {
  }
  bool operator<(const Connection& other) const noexcept
  {
    return this->distance_squared < other.distance_squared;
  }
  bool operator==(const Connection& other) const noexcept
  {
    return (this->a_ptr == other.a_ptr && this->b_ptr == other.b_ptr) ||
           (this->a_ptr == other.b_ptr && this->b_ptr == other.a_ptr);
  }
  static int64_t calc_distance(const Coordinates& a, const Coordinates& b)
  {
    return pow(b - a, 2).sum();
  }
  const Coordinates* a_ptr            = nullptr;
  const Coordinates* b_ptr            = nullptr;
  int64_t            distance_squared = numeric_limits<int64_t>::max();
};

using Volume       = pair<Coordinates, Coordinates>;
using NeighbourMap = map<Coordinates, pair<Coordinates, int64_t>>;
using Circuit      = set<const Coordinates*>;

vector<Coordinates> read_file(const string& file_name)
{
  vector<Coordinates> cvec;
  ifstream            file(file_name);
  string              str;
  const char          divider = ',';
  while (getline(file, str))
  {
    stringstream ss(str);
    int64_t      c[3];
    for (int i = 0; i < 3; ++i)
    {
      getline(ss, str, divider);
      c[i] = stoll(str);
    }
    cvec.emplace_back(Coordinates{c, 3});
  }
  return cvec;
}

vector<Connection> all_connections(const vector<Coordinates>& coordinates_vector)
{
  vector<Connection> all_connections;
  all_connections.reserve(coordinates_vector.size() * (coordinates_vector.size() - 1) / 2);
  for (const auto& [i, c] : coordinates_vector | ranges::views::enumerate)
  {
    for (const auto& d : coordinates_vector | ranges::views::drop(i + 1))
    {
      all_connections.push_back(Connection{&c, &d});
    }
  }
  return all_connections;
}

list<Circuit> connect(const vector<Connection>& closest_connections, int64_t num_pairs)
{
  list<Circuit> circuits;
  for (const auto& close_connection : closest_connections | ranges::views::take(num_pairs))
  {
    vector<Circuit*> to_be_removed;
    Circuit          new_circuit{close_connection.a_ptr, close_connection.b_ptr};
    for (auto& circuit : circuits)
    {
      if (circuit.contains(close_connection.a_ptr) || circuit.contains(close_connection.b_ptr))
      {
        new_circuit.merge(circuit);
        to_be_removed.push_back(&circuit);
      }
    }
    for (auto circuit_ptr : to_be_removed)
    {
      circuits.remove(*circuit_ptr);
    }
    circuits.push_back(new_circuit);
  }
  return circuits;
}

int64_t solve1(const string& file_name, int64_t num_pairs)
{
  const auto cvec        = read_file(file_name);
  auto       connections = all_connections(cvec);
  sort(connections.begin(), connections.end());
  const auto circuits = connect(connections, num_pairs);
  auto       circuit_sizes =
      circuits |
      ranges::views::transform([](const auto& circuit) noexcept { return circuit.size(); }) |
      ranges::to<vector>();
  sort(circuit_sizes.begin(), circuit_sizes.end(), greater{});
  return ranges::fold_left_first(circuit_sizes | ranges::views::take(3), multiplies{}).value();
}

int64_t connect_all(const vector<Connection>& closest_connections, size_t number_of_nodes)
{
  list<Circuit> circuits;
  for (const auto& close_connection : closest_connections)
  {
    vector<Circuit*> to_be_removed;
    Circuit          new_circuit{close_connection.a_ptr, close_connection.b_ptr};
    for (auto& circuit : circuits)
    {
      if (circuit.contains(close_connection.a_ptr) || circuit.contains(close_connection.b_ptr))
      {
        new_circuit.merge(circuit);
        to_be_removed.push_back(&circuit);
      }
    }
    if (new_circuit.size() == number_of_nodes)
    {
      return (*close_connection.a_ptr)[DIMS::X] * (*close_connection.b_ptr)[DIMS::X];
    }
    for (auto circuit_ptr : to_be_removed)
    {
      circuits.remove(*circuit_ptr);
    }
    circuits.push_back(new_circuit);
  }
  return 0;  // Should not be reached
}

int64_t solve2(const string& file_name)
{
  const auto cvec        = read_file(file_name);
  auto       connections = all_connections(cvec);
  sort(connections.begin(), connections.end());
  return connect_all(connections, cvec.size());
}

int main()
{
  const string INPUT_FILE{"day08.inp"};
  const string EXAMPLE_FILE{"day08.ex"};

  const int64_t example1 = solve1(EXAMPLE_FILE, 10);
  println("Example part 1: {}", example1);
  const int64_t CORRECT_ANSWER_EXAMPLE_1 = 40;
  assert(example1 == CORRECT_ANSWER_EXAMPLE_1);

  auto          start1    = chrono::high_resolution_clock::now();
  const int64_t part1     = solve1(INPUT_FILE, 1000);
  auto          duration1 = high_resolution_clock::now() - start1;
  println("Answer  part 1: {} ({})", part1, duration_cast<microseconds>(duration1));
  const int64_t CORRECT_ANSWER_PART_1 = 46398;
  assert(part1 == CORRECT_ANSWER_PART_1);

  const int64_t example2 = solve2(EXAMPLE_FILE);
  println("Example part 2: {}", example2);
  const int64_t CORRECT_ANSWER_EXAMPLE_2 = 25272;
  assert(example2 == CORRECT_ANSWER_EXAMPLE_2);

  auto          start2    = chrono::high_resolution_clock::now();
  const int64_t part2     = solve2(INPUT_FILE);
  auto          duration2 = chrono::high_resolution_clock::now() - start2;
  println("Answer  part 2: {} ({})", part2, duration_cast<microseconds>(duration2));
  const int64_t CORRECT_ANSWER_PART2 = 8141888143;
  assert(part2 == CORRECT_ANSWER_PART2);
};