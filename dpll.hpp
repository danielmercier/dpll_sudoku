#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

struct atom {
  int v; // SHOULD NOT BE 0

  atom(unsigned int lit, bool value) { v = value ? lit : -lit; }

  constexpr unsigned int lit() const { return (v > 0 ? v : -v); }

  constexpr int value() const { return std::clamp(v, -1, 1); }
};

struct sat {
  std::vector<std::vector<atom>> formula;
  std::vector<int> model; // -1 is false, 0 is unkown, 1 is true
  std::vector<unsigned int> decision_stack;
  std::unordered_map<unsigned int, unsigned int> assignment_level;
  std::optional<std::vector<atom>> conflicting_clause;

  sat(std::vector<std::vector<atom>> &formula);
  bool decide();
  bool bcp();
  bool resolve_conflict();
  bool dpll();
};
