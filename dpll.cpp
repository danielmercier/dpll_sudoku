#include "dpll.hpp"

sat::sat(std::vector<std::vector<atom>> &formula) {
  this->formula = formula;

  unsigned int last_lit = 0;

  for (const auto &clause : formula) {
    for (const auto &atom : clause) {
      unsigned int lit = atom.lit();
      last_lit = std::max(last_lit, lit);
    }
  }

  size_t size = last_lit + 1;
  model.resize(size, 0);
  assignment_level.resize(size, 0);
}

bool sat::is_valid() {
  for (const auto &clause : formula) {
    for (const auto &atom : clause) {
      unsigned int lit = atom.lit();
      if (atom.value() * model[lit] > 0) {
        goto next_clause;
      }
    }

    return false;

  next_clause : {}
  }

  return true;
}

bool sat::decide() {
  for (const auto &clause : formula) {
    for (const auto &atom : clause) {
      unsigned int lit = atom.lit();
      if (model[lit] == 0) {
        model[lit] = atom.value();
        decision_stack.push_back(lit);

        return true;
      } else if (model[lit] * atom.value() > 0) {
        // True clause, do not continue
        break;
      }
    }
  }

  return false;
}

bool sat::bcp() {
  for (const auto &clause : formula) {
    for (auto it = std::begin(clause); it != std::end(clause); ++it) {
      unsigned int lit = it->lit();
      int value = it->value();
      int i = model[lit];
      int result = value * i;

      if (result > 0) {
        // Clause is true, continue
        goto next_clause;
      } else if (result == 0) {
        // Clause is uninterpreted, find if it is a unit clause
        for (auto inner_it = it + 1; inner_it != std::end(clause); ++inner_it) {
          int i = model[inner_it->lit()];
          int result = inner_it->value() * i;

          if (result > 0) {
            // Clause is true, continue
            goto next_clause;
          } else if (result == 0) {
            // Second uninterpreted. Stop
            goto next_clause;
          }
        }

        // This is a unit clause
        model[lit] = value;
        assignment_level[lit] = decision_stack.size();

        goto next_clause;
      }
    }

    return false;

  next_clause : {}
  }

  return true;
}

bool sat::resolve_conflict() {
  if (decision_stack.empty()) {
    return false;
  }

  unsigned int lit = decision_stack.back();
  unsigned int decision_level = decision_stack.size();
  decision_stack.pop_back();

  // Remove any invalidated assignments
  for (unsigned int lit = 1; lit < assignment_level.size(); lit++) {
    if (assignment_level[lit] >= decision_level) {
      model[lit] = 0;
      assignment_level[lit] = 0;
    }
  }

  // Flip the value and add the decision level
  model[lit] = -1 * model[lit];
  assignment_level[lit] = decision_stack.size();

  return true;
}

bool sat::dpll() {
  while (true) {
    while (!bcp()) {
      if (!resolve_conflict()) {
        return false;
      }
    }

    if (!decide()) {
      return true;
    }
  }
}
