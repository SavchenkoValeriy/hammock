#pragma once

#include "hammock/utils/direction.hpp"
#include "hammock/utils/traversal.hpp"

namespace hammock::utils {
template <Direction To, class NodeType>
constexpr inline NodeType *rotate(NodeType *Node) {
  constexpr Direction From = invert(To);
  auto *NewTop = getChild<From>(Node);

  // couldn't swap the nodes (we have only one node)
  if (not NewTop)
    return Node;

  auto RelocatedChild = getChild<To>(NewTop);
  getChild<From>(Node) = RelocatedChild;
  getChild<To>(NewTop) = Node;

  if (RelocatedChild)
    RelocatedChild->Parent = Node;

  auto &Parent = Node->Parent;
  NewTop->Parent = Parent;

  if (Parent) {
    getParentLocation(Node) = NewTop;
  }

  Parent = NewTop;
  return NewTop;
}

template <class NodeType> constexpr inline void splay(NodeType *Node) {
  while (Node->Parent) {
    if (not Node->Parent->Parent) {
      if (isChild<Direction::Left>(Node))
        rotate<Direction::Right>(Node->Parent);

      else
        rotate<Direction::Left>(Node->Parent);

    } else if (isChild<Direction::Left>(Node) and
               isChild<Direction::Left>(Node->Parent)) {

      rotate<Direction::Right>(Node->Parent->Parent);
      rotate<Direction::Right>(Node->Parent);

    } else if (isChild<Direction::Right>(Node) and
               isChild<Direction::Right>(Node->Parent)) {

      rotate<Direction::Left>(Node->Parent->Parent);
      rotate<Direction::Left>(Node->Parent);

    } else if (isChild<Direction::Left>(Node) and
               isChild<Direction::Right>(Node->Parent)) {

      rotate<Direction::Right>(Node->Parent);
      rotate<Direction::Left>(Node->Parent);

    } else {

      rotate<Direction::Left>(Node->Parent);
      rotate<Direction::Right>(Node->Parent);
    }
  }
}
} // end namespace hammock::utils
