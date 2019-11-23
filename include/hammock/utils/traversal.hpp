#pragma once

#include "hammock/utils/direction.hpp"

#include <utility>

namespace hammock::utils {

template <Direction To, class NodeType>
constexpr inline NodeType *getTheOutmost(NodeType *Node) {
  for (auto *Child = getChild<To>(Node); Child != nullptr;
       Node = Child, Child = getChild<To>(Node)) {
  }
  return Node;
}

template <class NodeType>
constexpr inline NodeType *getTheLeftmost(NodeType *Node) {
  return getTheOutmost<Direction::Left>(Node);
}

template <class NodeType>
constexpr inline NodeType *getTheRightmost(NodeType *Node) {
  return getTheOutmost<Direction::Right>(Node);
}

template <Direction TestedDirection, class NodeType>
constexpr inline bool isChild(NodeType *Node) {
  return getChild<TestedDirection>(Node->Parent) == Node;
}

template <class NodeType>
constexpr inline auto &getParentLocation(NodeType *Node) {
  if (isChild<Direction::Right>(Node)) {
    return Node->Parent->Right;
  } else {
    return Node->Parent->Left;
  }
}

template <Direction To, class NodeType>
constexpr inline NodeType *successor(NodeType *Node) {
  constexpr Direction From = invert(To);
  if (getChild<To>(Node) != nullptr) {
    return getTheOutmost<From>(getChild<To>(Node));
  }

  auto *Parent = Node->Parent;
  for (; Parent != nullptr and getChild<From>(Parent) != Node;
       Node = Parent, Parent = Node->Parent) {
  }
  return Parent;
}

template <class NodeType, class KeyType, class Compare>
constexpr inline std::pair<NodeType *, NodeType *&> find(NodeType *&Node,
                                                         const KeyType &Key,
                                                         Compare Comparator) {
  NodeType *Parent = nullptr;
  NodeType **Result = &Node;
  while (*Result != nullptr) {
    Parent = *Result;
    // TODO: use user-defined comparison operators
    if (Comparator(Parent->Key(), Key)) {
      Result = &Parent->Right;
    } else if (Comparator(Key, Parent->Key())) {
      Result = &Parent->Left;
    } else {
      break;
    }
  }
  return {Parent, *Result};
}
} // end namespace hammock::utils
