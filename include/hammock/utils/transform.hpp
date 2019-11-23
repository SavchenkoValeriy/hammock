#pragma once

#include "hammock/utils/traversal.hpp"

namespace hammock::utils {

template <class NodeType>
constexpr inline void swap(NodeType *LHS, NodeType *RHS) {
  std::swap(LHS->Right, RHS->Right);
  std::swap(LHS->Left, RHS->Left);
  if (LHS->Parent != nullptr) {
    getParentLocation(LHS) = RHS;
  }
  if (RHS->Parent != nullptr) {
    getParentLocation(RHS) = LHS;
  }
  std::swap(LHS->Parent, RHS->Parent);
}

template <class NodeType>
constexpr inline void replace(NodeType *Old, NodeType *New) {
  if (New != nullptr) {
    New->Right = Old->Right;
    New->Left = Old->Left;
    New->Parent = Old->Parent;
  }
  if (Old->Parent != nullptr) {
    getParentLocation(Old) = New;
  }
}

} // end namespace hammock::utils
