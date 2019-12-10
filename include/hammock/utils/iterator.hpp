#pragma once

#include "hammock/utils/traversal.hpp"
#include "hammock/utils/type_traits.hpp"

#include <iterator>

namespace hammock::utils {

template <class Tree, bool Const = false,
          TraversalKind Order = TraversalKind::InOrder>
class Iterator {
public:
  using Node = AddConst<typename Tree::Node, Const>;
  using NodeBase = AddConst<typename Node::Header, Const>;

  constexpr Iterator(NodeBase *TreeNode) noexcept
      : CorrespondingNode(TreeNode) {}

  using KeyValuePair = typename Node::Pair;
  using value_type = AddConst<KeyValuePair, Const>;
  using reference = value_type &;
  using pointer = value_type *;

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  constexpr pointer operator->() const { return &getNode()->KeyValuePair(); }

  constexpr reference operator*() { return getNode()->KeyValuePair(); }

  constexpr Iterator operator++() {
    CorrespondingNode = successor<Direction::Right, Order>(CorrespondingNode);
    return *this;
  }

  constexpr Iterator operator++(int) {
    Iterator Copy = *this;
    operator++();
    return Copy;
  }

  constexpr Iterator operator--() {
    CorrespondingNode = successor<Direction::Left, Order>(CorrespondingNode);
    return *this;
  }

  constexpr Iterator operator--(int) {
    Iterator Copy = *this;
    operator--();
    return Copy;
  }

  constexpr bool operator==(const Iterator &RHS) const {
    return CorrespondingNode == RHS.CorrespondingNode;
  }

  constexpr bool operator!=(const Iterator &RHS) const {
    return !(*this == RHS);
  }

private:
  friend Tree;

  Node *getNode() const { return CorrespondingNode->getRealNode(); }

  NodeBase *CorrespondingNode;
};

} // end namespace hammock::utils
