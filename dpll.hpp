#pragma once

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

struct literal {
  int v; // SHOULD NOT BE 0

  literal(unsigned int var, bool polarity) { v = polarity ? var : -var; }
  literal(int v) { this->v = v; }

  unsigned int variable() const { return abs(v); }

  constexpr int polarity() const { return (v > 0) - (v < 0); }
};

struct sat {
  std::vector<std::vector<literal>> formula;
  std::vector<std::unordered_map<size_t, int>>
      var_occurences; // Index in first vector is var. Map from clause to the
                      // literal value in that clause

  std::vector<int> occur_list;
  std::vector<int> model; // -1 is false, 0 is unkown, 1 is true
  std::vector<unsigned int> decision_stack;
  std::vector<unsigned int> assignment_level;
  std::vector<std::vector<unsigned int>> implication_graph;

  // Misceleneous
  std::optional<std::vector<literal>> conflicting_clause;

  // Functions
  sat(std::vector<std::vector<literal>> &formula);
  bool is_valid();
  std::optional<std::pair<unsigned int, int>> decide();
  bool bcp(const std::pair<unsigned int, int> &var);
  std::optional<std::pair<unsigned int, int>> resolve_conflict();
  bool dpll();
};
