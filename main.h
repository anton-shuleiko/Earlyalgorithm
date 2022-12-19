#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "set"

using namespace std;

struct Rules {
  explicit Rules(const string&);
  Rules(const string&, const string&);
  Rules(const Rules&) = default;
  bool operator<(const Rules&) const;
  bool operator==(const Rules&) const;
  struct Symbol {
    Symbol() = default;
    Symbol(const Symbol&) = default;
    explicit Symbol(const char symbol) {
      symbol_ = symbol;
      is_terminal_ = !isupper(symbol);
    }
    bool operator==(const Symbol&) const;
    char symbol_;
    bool is_terminal_;
  };
  Symbol arrow_last_;
  vector<Symbol> next_arrow_;
  static const string arrow_symbol;
};

struct Sentence {
  Sentence(const Rules& rule, size_t position_of_previous_level,
           size_t position_of_point)
      : rule_(rule),
        pos_of_previous_lvl(position_of_previous_level),
        pos_of_point(position_of_point) {}
  Sentence(const Sentence&) = default;
  bool CheckNextSymbol() const;
  bool CheckRule() const;
  char GetNextSymbol() const;
  Sentence MoviePoint() const;
  bool operator==(const Sentence&) const;
  bool operator<(const Sentence&) const;
  Rules rule_;
  size_t pos_of_previous_lvl;
  size_t pos_of_point;
};

struct Grammar {
  void AddRule(const string&);
  void AddRule(const Rules&);
  static const char start_symbol = 'S';
  vector<Rules> rules_;
};

class Earley {
 public:
  bool Check(Grammar, const string&);

 private:
  Sentence DopStart();
  void Scan(size_t);
  void Predict(size_t);
  void Complete(size_t);
  Grammar grammar_;
  string word_;
  vector<set<Sentence>> sent_lists_;
  static const char dop_start_symbol = '$';
};
