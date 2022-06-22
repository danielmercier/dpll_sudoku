#include "dpll.hpp"

struct sudoku {
  unsigned int region_size;
  unsigned int size;
  std::vector<std::vector<atom>> formula;

  sudoku(unsigned int region_size = 3) {
    this->region_size = region_size;
    size = region_size * region_size;
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
      std::vector<std::pair<unsigned int, unsigned int>> region;

      for (const auto &j : regions) {
        region.push_back(std::pair(i, j));
      }

      valid(region);
    }
  }

  unsigned int cell(unsigned int i, unsigned int j, unsigned int d) {
    return (i * 100) + (j * 10) + d;
  }

  void one_of_nine(unsigned int i, unsigned int j) {
    std::vector<atom> clause;

    for (unsigned int d = 1; d <= size; d++) {
      clause.push_back(atom(cell(i, j, d), true));
    }

    formula.push_back(clause);
  }

  void only_one_of_nine(unsigned int i, unsigned int j) {
    for (unsigned int d1 = 1; d1 <= (size - 1); d1++) {
      for (unsigned int d2 = (d1 + 1); d2 <= size; d2++) {
        unsigned int cell1 = cell(i, j, d1);
        unsigned int cell2 = cell(i, j, d2);

        formula.push_back({atom(cell1, false), atom(cell2, false)});
      }
    }
  }

  void valid(const std::vector<std::pair<unsigned int, unsigned int>> &seq) {
    for (unsigned int i = 0; i < seq.size() - 1; i++) {
      for (unsigned int j = i + 1; j < seq.size(); j++) {
        for (unsigned int d = 1; d <= seq.size(); d++) {
          unsigned int cell1 = cell(seq[i].first, seq[i].second, d);
          unsigned int cell2 = cell(seq[j].first, seq[j].second, d);

          formula.push_back({atom(cell1, false), atom(cell2, false)});
        }
      }
    }
  }
};
