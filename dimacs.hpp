#pragma once

#include "dpll.hpp"
#include <fstream>
#include <iostream>
#include <vector>

std::vector<std::vector<literal>> read_file(const std::string &filename) {
  std::vector<std::vector<literal>> result;
  std::ifstream is(filename);

  if (is) {
    char tmp[256];
    char c;

    while (!is.eof()) {
      is >> c;

      if (c == 'c' || c == 'p') {
        is.getline(tmp, 256); // discard line
      } else if (c == '%' || is.eof()) {
        break;
      } else {
        is.unget();
        int a;
        std::vector<literal> clause;

        is >> a;

        while (a != 0 && !is.eof()) {
          clause.push_back(literal(a));
          is >> a;
        }

        result.push_back(clause);
      }
    }
  }

  return result;
}
