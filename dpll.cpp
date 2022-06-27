#include "dpll.hpp"
#include <iostream>
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
  implication_graph.resize(size);

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
        assignment_level[variable] = decision_stack.size();

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
      for (auto it = clause.begin(); it != clause.end(); ++it) {
        unsigned int variable = it->variable();
        int polarity = it->polarity();
        int i = model[variable];
        int result = polarity * i;

        if (result > 0) {
          // Clause is true, continue
          goto next_clause;
        } else if (result == 0) {
          // Clause is uninterpreted, find if it is a unit clause
          for (++it; it != clause.end(); ++it) {
            int i = model[it->variable()];
            int result = it->polarity() * i;

            if (result >= 0) {
              // Clause is true or second uninterpreted
              goto next_clause;
            }
          }

          // This is a unit clause
          found_unit_clause = true;
          model[variable] = polarity;
          assignment_level[variable] = decision_stack.size();

          implication_graph[variable].reserve(clause.size() - 1);

          for (const auto &literal : clause) {
            unsigned int clause_variable = literal.variable();

            if (clause_variable != variable) {
              implication_graph[variable].push_back(clause_variable);
            }
          }

          goto next_clause;
        }
      }

      // Register the conflicting clause @0
      for (const auto &literal : clause) {
        implication_graph[0].push_back(literal.variable());
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

  std::vector<literal> new_clause;

  unsigned int bt_level = 0;
  std::queue<unsigned int> q;

  for (const auto &pred : implication_graph[0]) {
    q.push(pred);
  }

  while (!q.empty()) {
    unsigned int variable = q.front();
    q.pop();

    if (implication_graph[variable].empty() ||
        assignment_level[variable] < decision_stack.size()) {
      bt_level = std::max(bt_level, assignment_level[variable]);
      new_clause.push_back(literal(variable * -model[variable]));
    }

    for (const auto &pred : implication_graph[variable]) {
      q.push(pred);
    }
  }

  formula.push_back(new_clause);

  decision_stack.resize(bt_level);

  unsigned int variable = decision_stack.back();
  int polarity = model[variable];
  unsigned int decision_level = decision_stack.size();
  decision_stack.pop_back();

  implication_graph[0].clear();

  // Remove any invalidated assignments
  for (unsigned int variable = 1; variable < assignment_level.size();
       variable++) {
    if (assignment_level[variable] >= decision_level) {
      model[variable] = 0;
      assignment_level[variable] = 0;
      implication_graph[variable].clear();
    }
  }

  // Flip the polarity and add the decision level
  model[variable] = -1 * polarity;
  assignment_level[variable] = decision_stack.size();
  implication_graph[variable].push_back(decision_stack.back());

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
