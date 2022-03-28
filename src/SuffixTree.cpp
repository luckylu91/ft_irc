#include "SuffixTree.hpp"

#include <algorithm>

#include "utils.hpp"

typedef SuffixTree::Node Node;
typedef std::vector<SuffixTree::Node>::iterator node_iterator;

struct NodeHasChar {
  bool operator()(char c, Node const &node) {
    return c == node.get_char();
  }
};

Node *SuffixTree::find_char_or_add(char c) {
  Node *node = this->find_char(c);
  if (node != NULL)
    return node;
  else {
    this->children.push_back(Node(c));
    return &this->children.back();
  }
}

Node *SuffixTree::find_char(char c) {
  node_iterator it = find_in_vector<NodeHasChar>(c, this->children);
  if (it != this->children.end())
    return &*it;
  else
    return NULL;
}

Node const *SuffixTree::find_char(char c) const {
  return const_cast<SuffixTree *>(this)->find_char(c);
}

Node const *SuffixTree::find_string(char const *s) const {
  if (!s[0])
    return NULL;
  Node const *next = this->find_char(s[0]);
  if (next == NULL)
    return NULL;
  if (!s[1])
    return next;
  return next->sub.find_string(s + 1);
}

void SuffixTree::add_string(char const *current) {
  if (!current[0])
    throw AddEmptyStringException();
  Node *node = this->find_char_or_add(current[0]);
  node->add_string(current + 1);
  this->weight++;
}

void SuffixTree::add_string(std::string const &s) {
  this->add_string(s.c_str());
}

void Node::add_string(char const *current) {
  if (!current[0]) {
    this->this_is_word = true;
  } else
    this->sub.add_string(current);
}

struct PushBackWeight {
  void operator()(std::vector<std::size_t> &vec_weights, Node const &child) {
    vec_weights.push_back(child.get_weight());
  }
};

void SuffixTree::select_random_word(std::string &word_so_far) const {
  if (this->weight == 0)
    throw SearchEmptyTreeException();
  std::vector<std::size_t> children_weigths;
  for_each_in_vector<PushBackWeight>(children_weigths, this->children);
  std::size_t r = static_cast<std::size_t>(rand() % static_cast<int>(this->weight));
  std::size_t index = 0;
  std::size_t cum_sum = children_weigths[index];
  while (r > cum_sum) {
    index++;
    if (index == children_weigths.size()) {
    }
    cum_sum += children_weigths[index];
  }
  word_so_far.push_back(this->children[index].get_char());
  return this->children[index].select_random_word(word_so_far);
}

void Node::select_random_word(std::string &word_so_far) const {
  if (this->is_word()) {
    std::size_t r = static_cast<std::size_t>(rand() % static_cast<int>(1 + this->sub.weight));
    if (r == 0)
      return;
  }
  this->sub.select_random_word(word_so_far);
}

std::string SuffixTree::select_random_word_with_prefix(std::string const &prefix) const {
  Node const *end_of_prefix = this->find_string(prefix.c_str());
  if (end_of_prefix == NULL)
    return "No match found in the dictionnary for '" + prefix + "'";
  std::string result = prefix;
  end_of_prefix->select_random_word(result);
  return result;
}
