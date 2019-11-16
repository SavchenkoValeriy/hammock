#pragma once

#include "hammock/utils/direction.hpp"

namespace hammock::utils {

template <class NodeType, Direction To>
constexpr inline NodeType *getTheOutmost(NodeType *Node) {
  for (auto *Child = getChild<To>(Node); Child != nullptr;
       Node = Child, Child = getChild<To>(Node)) {
  }
  return Node;
}

template <class NodeType, Direction To>
constexpr inline NodeType *successor(NodeType *Node) {
  constexpr Direction From = invert(To);
  if (getChild<To>(Node) != nullptr) {
    return getTheOutmost<From>(getChild<To>(Node));
  }

  auto *Parent = Node->Parent;
  for (; Parent != nullptr and getChild<To>(Parent) != Node;
       Node = Parent, Parent = Node->Parent) {
  }
  return Parent;
}

template <class NodeType, class KeyType>
constexpr inline NodeType *&find(NodeType *Node, const KeyType &Key) {
  NodeType *&Result = Node;
  while (Result != nullptr) {
    if (Result->Key > Key) {
      Result = Result->Left;
    } else if (Result->Key < Key) {
      Result = Result->Right;
    } else {
      return Result;
    }
  }
  return Result;
}
} // end namespace hammock::utils
