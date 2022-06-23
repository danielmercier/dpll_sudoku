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
  std::vector<std::unordered_map<size_t, int>>
      lit_occurences; // Index in first vector is lit. Map from clause to the
                      // atom value in that clause

  std::vector<int> occur_list;
  std::vector<int> model; // -1 is false, 0 is unkown, 1 is true
  std::vector<unsigned int> decision_stack;
  std::vector<unsigned int> assignment_level;

  // Misceleneous
  std::optional<std::vector<atom>> conflicting_clause;

  // Functions
  sat(std::vector<std::vector<atom>> &formula);
  bool is_valid();
  std::optional<std::pair<unsigned int, int>> decide();
  bool bcp(const std::pair<unsigned int, int> &var);
  std::optional<std::pair<unsigned int, int>> resolve_conflict();
  bool dpll();
};
