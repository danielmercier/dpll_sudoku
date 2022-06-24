#include "dpll.hpp"
#include <queue>

constexpr int operator*(int l, const literal &r) { return l * r.v; }

constexpr int operator*(const literal &l, int r) { return l.v * r; }

constexpr int operator*(const literal &l, const literal &r) {
  return l.v * r.polarity();
}

sat::sat(std::vector<std::vector<literal>> &formula) {
  this->formula = formula;

  unsigned int last_var = 0;

  for (const auto &clause : formula) {
    for (const auto &literal : clause) {
      unsigned int var = literal.variable();
      last_var = std::max(last_var, var);
    }
  }

  size_t size = last_var + 1;
  model.resize(size, 0);
  assignment_level.resize(size, 0);

  var_occurences.resize(size, std::unordered_map<size_t, int>());

  for (size_t clause = 0; clause < formula.size(); clause++) {
    for (const auto &literal : formula[clause]) {
      var_occurences[literal.variable()][clause] = literal.polarity();
    }
  }
}

bool sat::is_valid() {
  for (const auto &clause : formula) {
    for (const auto &literal : clause) {
      unsigned int variable = literal.variable();
      if (literal.polarity() * model[variable] > 0) {
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
    for (const auto &literal : clause) {
      unsigned int variable = literal.variable();
      if (model[variable] == 0) {
        int polarity = literal.polarity();
        model[variable] = polarity;
        decision_stack.push_back(variable);

        return std::pair(variable, polarity);
      } else if (model[variable] * literal > 0) {
        // True clause, do not continue
        break;
      }
    }
  }

  return std::nullopt;
}

bool sat::bcp(const std::pair<unsigned int, int> &variable) {
  bool found_unit_clause = true;

  while (found_unit_clause) {
    found_unit_clause = false;

    for (const auto &clause : formula) {
      for (auto it = std::begin(clause); it != std::end(clause); ++it) {
        unsigned int variable = it->variable();
        int polarity = it->polarity();
        int i = model[variable];
        int result = polarity * i;

        if (result > 0) {
          // Clause is true, continue
          goto next_clause;
        } else if (result == 0) {
          // Clause is uninterpreted, find if it is a unit clause
          for (auto inner_it = it + 1; inner_it != std::end(clause);
               ++inner_it) {
            int i = model[inner_it->variable()];
            int result = inner_it->polarity() * i;

            if (result > 0) {
              // Clause is true, continue
              goto next_clause;
            } else if (result == 0) {
              // Second uninterpreted. Stop
              goto next_clause;
            }
          }

          // This is a unit clause
          found_unit_clause = true;
          model[variable] = polarity;
          assignment_level[variable] = decision_stack.size();

          goto next_clause;
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

  unsigned int variable = decision_stack.back();
  unsigned int decision_level = decision_stack.size();
  decision_stack.pop_back();

  // Remove any invalidated assignments
  for (unsigned int variable = 1; variable < assignment_level.size();
       variable++) {
    if (assignment_level[variable] >= decision_level) {
      model[variable] = 0;
      assignment_level[variable] = 0;
    }
  }

  // Flip the polarity and add the decision level
  int polarity = -1 * model[variable];

  model[variable] = polarity;
  assignment_level[variable] = decision_stack.size();

  return std::pair(variable, polarity);
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
