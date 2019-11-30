#pragma once

#include "hammock/utils/traversal.hpp"

#include <cassert>

namespace hammock::utils {

template <class NodeType>
constexpr inline void swap(NodeType *LHS, NodeType *RHS) {
  assert(("Header nodes should never be swapped" && not LHS->isHeader()));
  assert(("Header nodes should never be swapped" && not RHS->isHeader()));

  std::swap(LHS->Right, RHS->Right);
  std::swap(LHS->Left, RHS->Left);
  if (not LHS->isRoot()) {
    getParentLocation(LHS) = RHS;
  }
  if (not RHS->isRoot()) {
    getParentLocation(RHS) = LHS;
  }
  std::swap(LHS->Parent, RHS->Parent);
}

template <class NodeType>
constexpr inline void replace(NodeType *Old, NodeType *New) {
  assert(("Header nodes should not be replaced" && not Old->isHeader()));

  if (New != nullptr) {
    New->Right = Old->Right;
    New->Left = Old->Left;
    New->Parent = Old->Parent;
  }
  if (not Old->isRoot()) {
    getParentLocation(Old) = New;
  }
}

} // end namespace hammock::utils
