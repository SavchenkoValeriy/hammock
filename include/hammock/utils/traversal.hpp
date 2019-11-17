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

template <class NodeType, class KeyType>
constexpr inline std::pair<NodeType *, NodeType *&> find(NodeType *&Node,
                                                         const KeyType &Key) {
  NodeType *Parent = nullptr;
  NodeType **Result = &Node;
  while (*Result != nullptr) {
    Parent = *Result;
    if (Parent->Key() > Key) {
      Result = &Parent->Left;
    } else if (Parent->Key() < Key) {
      Result = &Parent->Right;
    } else {
      break;
    }
  }
  return {Parent, *Result};
}
} // end namespace hammock::utils
