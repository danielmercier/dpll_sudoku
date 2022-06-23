#include "dpll.hpp"
#include <queue>

constexpr int operator*(int l, const atom &r) { return l * r.v; }

constexpr int operator*(const atom &l, int r) { return l.v * r; }

constexpr int operator*(const atom &l, const atom &r) {
  return l.v * r.value();
}

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

  lit_occurences.resize(size, std::unordered_map<size_t, int>());

  for (size_t clause = 0; clause < formula.size(); clause++) {
    for (const auto &atom : formula[clause]) {
      lit_occurences[atom.lit()][clause] = atom.value();
    }
  }
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

std::optional<std::pair<unsigned int, int>> sat::decide() {
  for (const auto &clause : formula) {
    for (const auto &atom : clause) {
      unsigned int lit = atom.lit();
      if (model[lit] == 0) {
        int value = atom.value();
        model[lit] = value;
        decision_stack.push_back(lit);

        return std::pair(lit, value);
      } else if (model[lit] * atom > 0) {
        // True clause, do not continue
        break;
      }
    }
  }

  return std::nullopt;
}

bool sat::bcp(const std::pair<unsigned int, int> &var) {
  std::queue<std::pair<unsigned int, int>> implied;
  implied.push(var);

  bool o = true;

  while (!implied.empty()) {
    const auto &atom = implied.front();
    unsigned int lit = atom.first;
    int value = atom.second;
    implied.pop();

    for (const auto &entry : lit_occurences[lit]) {
      // First check if the clause is true
      if (entry.second * value > 0) {
        goto next_clause;
      } else {
        const auto &clause = formula[entry.first];
        // Check all other literals in the clause
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
            for (auto inner_it = it + 1; inner_it != std::end(clause);
                 ++inner_it) {
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

            implied.push(std::pair(lit, value));

            goto next_clause;
          }
        }
      }

      return false;

    next_clause : {}
    }
  }

  return true;
}

std::optional<std::pair<unsigned int, int>> sat::resolve_conflict() {
  if (decision_stack.empty()) {
    return std::nullopt;
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
  int value = -1 * model[lit];

  model[lit] = value;
  assignment_level[lit] = decision_stack.size();

  return std::pair(lit, value);
}

bool sat::dpll() {
  while (true) {
    auto implied = decide();

    if (!implied.has_value()) {
      return true;
    }

    while (!bcp(implied.value())) {
      implied = resolve_conflict();

      if (!implied.has_value()) {
        return false;
      }
    }
  }
}
