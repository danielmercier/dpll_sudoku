#include "dpll.hpp"

template <std::size_t region_size> struct sudoku {
  static constexpr unsigned int size = region_size * region_size;
  std::vector<std::vector<literal>> formula;

  sudoku() {
    formula.clear();

    // Valid numbers
    for (unsigned int i = 1; i <= size; i++) {
      for (unsigned int j = 1; j <= size; j++) {
        one_of_nine(i, j);
        only_one_of_nine(i, j);
      }
    }

    // Valid row
    for (unsigned int i = 1; i <= size; i++) {
      std::vector<std::pair<unsigned int, unsigned int>> row;

      for (unsigned int j = 1; j <= size; j++) {
        row.push_back(std::pair(i, j));
      }

      valid(row);
    }

    // Valid col
    for (unsigned int j = 1; j <= size; j++) {
      std::vector<std::pair<unsigned int, unsigned int>> col;

      for (unsigned int i = 1; i <= size; i++) {
        col.push_back(std::pair(i, j));
      }

      valid(col);
    }

    // Valid region
    std::vector<unsigned int> regions;

    {
      unsigned int region = 1;

      while (region <= size) {
        regions.push_back(region);

        region = region + region_size;
      }
    }

    for (const auto &i : regions) {
      for (const auto &j : regions) {
        std::vector<std::pair<unsigned int, unsigned int>> region;

        for (int add_i = 0; add_i < region_size; add_i++) {
          for (int add_j = 0; add_j < region_size; add_j++) {
            region.push_back(std::pair(i + add_i, j + add_j));
          }
        }

        valid(region);
      }
    }
  }

  unsigned int cell(unsigned int i, unsigned int j, unsigned int d) {
    return (i * 100) + (j * 10) + d;
  }

  void one_of_nine(unsigned int i, unsigned int j) {
    std::vector<literal> clause;

    for (unsigned int d = 1; d <= size; d++) {
      clause.push_back(literal(cell(i, j, d), true));
    }

    formula.push_back(clause);
  }

  void only_one_of_nine(unsigned int i, unsigned int j) {
    for (unsigned int d1 = 1; d1 <= (size - 1); d1++) {
      for (unsigned int d2 = (d1 + 1); d2 <= size; d2++) {
        unsigned int cell1 = cell(i, j, d1);
        unsigned int cell2 = cell(i, j, d2);

        formula.push_back({literal(cell1, false), literal(cell2, false)});
      }
    }
  }

  void valid(const std::vector<std::pair<unsigned int, unsigned int>> &seq) {
    for (unsigned int i = 0; i < seq.size() - 1; i++) {
      for (unsigned int j = i + 1; j < seq.size(); j++) {
        for (unsigned int d = 1; d <= seq.size(); d++) {
          unsigned int cell1 = cell(seq[i].first, seq[i].second, d);
          unsigned int cell2 = cell(seq[j].first, seq[j].second, d);

          formula.push_back({literal(cell1, false), literal(cell2, false)});
        }
      }
    }
  }

  void read_grid(const std::vector<std::vector<unsigned int>> &grid) {
    for (size_t i = 0; i < size; i++) {
      for (size_t j = 0; j < size; j++) {
        int d = grid[i][j];

        if (d > 0) {
          formula.push_back({{cell(i + 1, j + 1, d), true}});
        }
      }
    }
  }
};
