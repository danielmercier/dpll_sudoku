#include "sudoku.hpp"
#include <iostream>

int main() {
  sudoku sudoku(3);

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

  sat sat(sudoku.formula);

  std::cout << "Sudoku can be solved: " << sat.dpll() << std::endl;

  return 0;
}
