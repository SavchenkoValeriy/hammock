#pragma once

#include "hammock/utils/traversal.hpp"

#include <iterator>

namespace hammock::utils {

template <class Node> class Iterator {
public:
  constexpr Iterator(Node *TreeNode) noexcept : CorrespondingNode(TreeNode) {}

  using KeyValuePair = typename Node::Pair;
  using value_type = KeyValuePair;
  using reference = KeyValuePair &;
  using pointer = KeyValuePair *;

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  constexpr pointer operator->() const {
    return &CorrespondingNode->KeyValuePair;
  }

  constexpr reference operator*() { return CorrespondingNode->KeyValuePair; }

  constexpr Iterator operator++() {
    CorrespondingNode = successor<Direction::Right>(CorrespondingNode);
    return *this;
  }

  constexpr Iterator operator++(int) {
    Iterator Copy = *this;
    operator++();
    return Copy;
  }

  constexpr Iterator operator--() {
    CorrespondingNode = successor<Direction::Left>(CorrespondingNode);
    return *this;
  }

  constexpr Iterator operator--(int) {
    Iterator Copy = *this;
    operator--();
    return Copy;
  }

  constexpr bool operator==(const Iterator<Node> &RHS) const {
    return CorrespondingNode == RHS.CorrespondingNode;
  }

  constexpr bool operator!=(const Iterator<Node> &RHS) const {
    return !(*this == RHS);
  }

private:
  Node *CorrespondingNode;
};

} // end namespace hammock::utils
