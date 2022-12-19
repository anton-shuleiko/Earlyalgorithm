#include "main.h"

#include <iostream>

using namespace std;


const string Rules::arrow_symbol = "->";



Rules::Rules(const string& raw_format) {
  if (raw_format.find(arrow_symbol) == (unsigned long)-1) {
    throw invalid_argument(
        "Error\n"
        "Format : [Left Part (not empty)] -> [Right Part (may be empty)]");
  }
  size_t entry_index = raw_format.find(arrow_symbol);
  *this = Rules(raw_format.substr(0, entry_index),
                raw_format.substr(entry_index + arrow_symbol.size()));
}

bool Rules::Symbol::operator==(const Symbol& other) const {
  return symbol_ == other.symbol_;
}
Rules::Rules(const string& arrow_last, const string& next_arrow) {
  if (arrow_last.empty()) {
    throw invalid_argument(
        "Error!\n"
        "Format : [Left Part (empty)] -> [ . . . ]");
  }

  if (arrow_last.size() > 1) {
    throw invalid_argument(
        "Error\n"
        "Format : [Left Part (more than 1 symbol)] -> [ . . . ]");
  }

  arrow_last_ = Symbol(arrow_last[0]);
  if (arrow_last_.is_terminal_) {
    throw invalid_argument(
        "Error\n"
        "Format : [Left Part (lower cays)] -> [ . . . ]");
  }

  for (const char& symbol : next_arrow) {
    next_arrow_.emplace_back(symbol);
  }
}
bool Rules::operator==(const Rules& other) const {
  return arrow_last_ == other.arrow_last_ &&
         next_arrow_ == other.next_arrow_;
}

bool Rules::operator<(const Rules& other) const {
  if (arrow_last_.symbol_ != other.arrow_last_.symbol_) {
    return arrow_last_.symbol_ < other.arrow_last_.symbol_;
  }
  size_t minafter_size = min(next_arrow_.size(), other.next_arrow_.size());
  for (size_t index = 0; index < minafter_size; ++index) {
    if (next_arrow_[index].symbol_ < other.next_arrow_[index].symbol_) {
      return true;
    }
  }
  return next_arrow_.size() < other.next_arrow_.size();
}


bool Sentence::CheckNextSymbol() const {
  if (CheckRule()) {
    return false;
  }
  return rule_.next_arrow_[pos_of_point].is_terminal_;
}

Rules::Rules(const string& arrow_last, const string& next_arrow) {
  if (arrow_last.empty()) {
    throw invalid_argument(
        "Error!\n"
        "Format : [Left Part (empty)] -> [ . . . ]");
  }

  if (arrow_last.size() > 1) {
    throw invalid_argument(
        "Error\n"
        "Format : [Left Part (more than 1 symbol)] -> [ . . . ]");
  }

  arrow_last_ = Symbol(arrow_last[0]);
  if (arrow_last_.is_terminal_) {
    throw invalid_argument(
        "Error\n"
        "Format : [Left Part (lower cays)] -> [ . . . ]");
  }

  for (const char& symbol : next_arrow) {
    next_arrow_.emplace_back(symbol);
  }
}

char Sentence::GetNextSymbol() const {
  return rule_.next_arrow_[pos_of_point].symbol_;
}

bool Sentence::CheckRule() const {
  return pos_of_point == rule_.next_arrow_.size();
}

Sentence Sentence::MoviePoint() const {
  return Sentence(rule_, pos_of_previous_lvl, pos_of_point + 1);
}

bool Sentence::operator==(const Sentence& other) const {
  return (rule_ == other.rule_ &&
          pos_of_previous_lvl == other.pos_of_previous_lvl &&
          pos_of_point == other.pos_of_point);
}

bool Sentence::operator<(const Sentence& other) const {
  return make_tuple(rule_, pos_of_previous_lvl, pos_of_point) <
         make_tuple(other.rule_, other.pos_of_previous_lvl, other.pos_of_point);
}


void Grammar::AddRule(const string& raw_format) {
  rules_.emplace_back(raw_format);
}

void Grammar::AddRule(const Rules& rule) { rules_.push_back(rule); }

bool Earley::Check(Grammar grammar, const string& word) {
  grammar_ = grammar;
  word_ = word;
  sent_lists_.clear();
  sent_lists_.resize(word.size() + 1);
  Sentence dopStart = DopStart();
  sent_lists_[0].insert(dopStart);
  for (size_t position = 0; position <= word.size(); ++position) {
    Scan(position);
    size_t old_sent_list_size = 0;
    do {
      old_sent_list_size = sent_lists_[position].size();
      Complete(position);
      Predict(position);
    } while (sent_lists_[position].size() != old_sent_list_size);
  }
  Sentence final_situation = dopStart.MoviePoint();
  const set<Sentence>& last_situation_list = sent_lists_[word.size()];
  return (find(last_situation_list.begin(), last_situation_list.end(),
               final_situation) != last_situation_list.end());
}

Sentence Earley::DopStart() {
  string doprule_raw_format =
      string(1, Grammar::start_symbol) + "->" + Grammar::start_symbol;
  Rules dop_rule(doprule_raw_format);
  dop_rule.arrow_last_.symbol_ = dop_start_symbol;
  grammar_.AddRule(dop_rule);
  return Sentence(dop_rule, 0, 0);
}

void Earley::Scan(size_t position) {
  if (position == 0) {
    return;
  }
  for (const Sentence& situation : sent_lists_[position - 1]) {
    if (situation.CheckNextSymbol() &&
        situation.GetNextSymbol() == word_[position - 1]) {
      sent_lists_[position].insert(situation.MoviePoint());
    }
  }
}

void Earley::Predict(size_t position) {
  for (Sentence sent : sent_lists_[position]) {
    if (!sent.CheckRule() && !sent.CheckNextSymbol()) {
      for (const Rules& rule : grammar_.rules_) {
        if (sent.GetNextSymbol() == rule.arrow_last_.symbol_) {
          sent_lists_[position].emplace(rule, position, 0);
        }
      }
    }
  }
}

void Earley::Complete(size_t position) {
  for (const Sentence& sent : sent_lists_[position]) {
    if (sent.CheckRule()) {
      for (const Sentence& upper_sent : sent_lists_[sent.pos_of_previous_lvl]) {
        if (!upper_sent.CheckRule() &&
            upper_sent.GetNextSymbol() == sent.rule_.arrow_last_.symbol_) {
          sent_lists_[position].insert(upper_sent.MoviePoint());
        }
      }
    }
  }
}


int main() {
  Earley early_algorithm;
  Grammar grammar;
  cout << "Number of rules : ";
  size_t num_rules = 0;
  cin >> num_rules;
  cout << "Rules : \n";
  for (size_t i = 0; i < num_rules; ++i) {
    string raw_format;
    cin >> raw_format;
    try {
      grammar.AddRule(raw_format);
    } catch (exception& error) {
      cerr << error.what() << endl;
      return 1;
    }
  }
  cout << "Word to search : ";
  string word;
  cin >> word;
  if (early_algorithm.Check(grammar, word)) {
    cout << "YES";
  } else {
    cout << "NO";
  }
  cout << endl;
  return 0;
}
