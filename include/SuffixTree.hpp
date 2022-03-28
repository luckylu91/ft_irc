#pragma once

#include <exception>
#include <set>
#include <string>
#include <vector>

class SuffixTree {
 public:
  class Node;

  SuffixTree() : children(), weight(0) {}

  bool empty() const { return this->children.size() == 0; }
  std::size_t get_weight() const { return this->weight; }

  Node *find_char_or_add(char);
  Node *find_char(char);
  Node const *find_char(char c) const;
  // void add_child(char);
  void add_string(std::string const &);
  void add_string(char const *start);
  Node const *find_string(char const *s) const;
  void select_random_word(std::string &word_so_far) const;
  std::string select_random_word_with_prefix(std::string const &prefix) const;

 private:
  std::vector<Node> children;
  std::size_t weight;
};

class SuffixTree::Node {
 public:
  friend class SuffixTree;

  Node(char c) : c(c), sub(), this_is_word(false) {}

  bool is_word() const { return this->this_is_word; }
  char get_char() const { return this->c; };
  std::string get_word() const { return this->word; };
  std::size_t get_weight() const { return this->sub.get_weight() + (this->this_is_word ? 1 : 0); };

  void add_string(char const *start);
  void select_random_word(std::string &word_so_far) const;

 private:
  char c;
  SuffixTree sub;
  bool this_is_word;
  std::string word;
};

struct AddEmptyStringException : std::exception {
  virtual char const *what() const throw() { return "Trying to add en empty string to the tri"; }
};

struct SearchEmptyTreeException : std::exception {
  virtual char const *what() const throw() { return "Trying to search an empty tri"; }
};
