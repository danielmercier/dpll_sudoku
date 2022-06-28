#include "dimacs.hpp"
#include "sudoku.hpp"
#include <iostream>

int main(int argc, char **argv) {
  sudoku<3> sudoku;

  // for (const auto &clause : sudoku.formula) {
  //  std::cout << "&& ";
  //  for (const auto atom : clause) {
  //    if (atom.value()) {
  //      std::cout << atom.lit();
  //    } else {
  //      std::cout << "!" << atom.lit();
  //    }

  //    std::cout << " || ";
  //  }

  //  std::cout << std::endl;
  //}

  // std::vector<std::vector<unsigned int>> grid = {
  //    {0, 2, 0, 8, 0, 0, 6, 0, 0}, {7, 0, 0, 1, 0, 0, 2, 0, 4},
  //    {0, 5, 0, 0, 0, 0, 0, 3, 0}, {4, 0, 0, 0, 0, 0, 0, 6, 0},
  //    {0, 0, 3, 0, 0, 0, 7, 0, 0}, {6, 0, 0, 0, 0, 0, 0, 0, 0},
  //    {0, 0, 0, 2, 1, 3, 0, 0, 0}, {9, 0, 0, 0, 0, 7, 0, 1, 0},
  //    {0, 0, 5, 0, 0, 6, 0, 4, 0}};

  //  std::vector<std::vector<unsigned int>> grid = {
  //      {0, 0, 5, 3, 0, 0, 0, 0, 0}, {8, 0, 0, 0, 0, 0, 0, 2, 0},
  //      {0, 7, 0, 0, 1, 0, 5, 0, 0}, {4, 0, 0, 0, 0, 5, 3, 0, 0},
  //      {0, 1, 0, 0, 7, 0, 0, 0, 6}, {0, 0, 3, 2, 0, 0, 0, 8, 0},
  //      {0, 6, 0, 5, 0, 0, 0, 0, 9}, {0, 0, 4, 0, 0, 0, 0, 3, 0},
  //      {0, 0, 0, 0, 0, 9, 7, 0, 0}};

  std::vector<std::vector<unsigned int>> grid = {
      {8, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 3, 6, 0, 0, 0, 0, 0},
      {0, 7, 0, 0, 9, 0, 2, 0, 0}, {0, 5, 0, 0, 0, 7, 0, 0, 0},
      {0, 0, 0, 0, 4, 5, 7, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 3, 0},
      {0, 0, 1, 0, 0, 0, 0, 6, 8}, {0, 0, 8, 5, 0, 0, 0, 1, 0},
      {0, 9, 0, 0, 0, 0, 4, 0, 0}};

  sudoku.read_grid(grid);
  sat sudoku_sat(sudoku.formula);

  std::cout << "Sudoku can be solved: " << sudoku_sat.dpll() << std::endl;

  for (unsigned int i = 1; i <= sudoku.size; i++) {
    for (unsigned int j = 1; j <= sudoku.size; j++) {
      std::optional<unsigned int> val;
      for (unsigned int d = 1; d <= sudoku.size; d++) {
        if (sudoku_sat.model[sudoku.cell(i, j, d)] > 0) {
          if (val.has_value()) {
            std::cout << "ERROR, two different digits" << std::endl;
            return 1;
          } else {
            val = d;
          }
        }
      }

      if (!val.has_value()) {
        std::cout << "ERROR, no value" << std::endl;
        return 1;
      }

      std::cout << val.value() << " ";
    }

    std::cout << std::endl;
  }

  for (size_t i = 1; i < argc; i++) {
    auto formula = read_file(argv[i]);

    sat sat(formula);

    std::cout << argv[i] << " is SAT: " << sat.dpll() << std::endl;
  }

  return 0;
}
