#pragma once
#include "dpll.hpp"
#include <iostream>
#include <vector>

enum result { Unknown, Sat, Unsat, Unit };

result eval(const std::vector<literal> &clause, const std::vector<int> &model) {
  bool possible_unit_clause = false;

  for (const auto &literal : clause) {
    if (model[literal.variable()] * literal.polarity() > 0) {
      return Sat;
    } else if (model[literal.variable()] * literal.polarity() == 0) {
      if (possible_unit_clause) {
        return Unknown; // 2 uninterpreted
      } else {
        possible_unit_clause = true;
      }
    }
  }

  return (possible_unit_clause ? Unit : Unsat);
}

void print_model(const std::vector<int> &model) {
  std::cout << "{";

  for (unsigned int variable = 1; variable < model.size(); ++variable) {
    if (model[variable] != 0) {
      std::cout << model[variable] * variable << ", ";
    }
  }

  std::cout << "}" << std::endl;
}

void print_clause(const std::vector<literal> &clause) {
  std::cout << "{";

  for (const auto &literal : clause) {
    std::cout << literal << ", ";
  }

  std::cout << "}" << std::endl;
}

void print_clause_eval(const std::vector<literal> &clause,
                       const std::vector<int> &model) {
  switch (eval(clause, model)) {
  case Unknown:
  case Unit:

    std::cout << "{";

    for (const auto &literal : clause) {
      if (model[literal.variable()] == 0) {
        std::cout << literal << ", ";
      }
    }

    std::cout << "}" << std::endl;
    break;
  case Unsat:
    std::cout << "Unsat" << std::endl;
    break;
  case Sat:
    std::cout << "Sat" << std::endl;
    break;
  }
}
