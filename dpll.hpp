#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

struct atom {
  int v; // SHOULD NOT BE 0

  atom(unsigned int lit, bool value) { v = value ? lit : -lit; }

  unsigned int lit() const { return abs(v); }

  constexpr int value() const { return (v > 0) - (v < 0); }
};

struct sat {
  std::vector<std::vector<atom>> formula;
  std::vector<int> model; // -1 is false, 0 is unkown, 1 is true
  std::vector<unsigned int> decision_stack;
  std::vector<unsigned int> assignment_level;
  std::optional<std::vector<atom>> conflicting_clause;

  sat(std::vector<std::vector<atom>> &formula);
  bool is_valid();
  bool decide();
  bool bcp();
  bool resolve_conflict();
  bool dpll();
};
