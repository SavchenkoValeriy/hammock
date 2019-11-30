#pragma once

#include "hammock/utils/direction.hpp"
#include "hammock/utils/node.hpp"

#include <cassert>
#include <queue>
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
  for (; getChild<To>(Parent) == Node; Node = Parent, Parent = Node->Parent) {
  }
  return Parent;
}

template <class NodeType, class KeyType, class Compare>
constexpr inline std::pair<NodeType *, NodeType *&>
find(NodeType *Node, const KeyType &Key, Compare Comparator) {
  assert(("The node to start the search from should not be null" &&
          Node != nullptr));

  NodeType *Parent = nullptr;
  NodeType **Result = &Node;

  while (*Result != nullptr) {
    Parent = *Result;

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

template <class NodeType, class CallbackType>
constexpr inline void preOrderTraverse(NodeType *Root, CallbackType Callback) {
  static_assert(
      std::is_invocable_v<const CallbackType, NodeType *>,
      "traversal callback must be invocable with a node pointer argument");

  // TODO: get rid of the queue, there is no need in it
  std::queue<NodeType *> TraversalQueue;
  TraversalQueue.push(Root);

  while (not TraversalQueue.empty()) {
    auto *CurrentNode = TraversalQueue.front();
    TraversalQueue.pop();

    if (CurrentNode == nullptr)
      continue;
    TraversalQueue.push(CurrentNode->Left);
    TraversalQueue.push(CurrentNode->Right);

    Callback(CurrentNode);
  }
}

template <Direction To, class NodeType>
constexpr inline bool shouldGo(const NodeType *Origin, const NodeType *Copy) {
  return getChild<To>(Origin) != nullptr and getChild<To>(Copy) == nullptr;
}

template <Direction To, class NodeType, class CallbackType>
constexpr inline void copyAndGo(const NodeType *&Origin, NodeType *&Copy,
                                CallbackType Create) {
  getChild<To>(Copy) = Create(*getChild<To>(Origin));
  getChild<To>(Copy)->Parent = Copy;
  Origin = getChild<To>(Origin);
  Copy = getChild<To>(Copy);
}

template <class NodeType, class CallbackType>
constexpr inline NodeBase<NodeType>
copyTree(const NodeBase<NodeType> &OriginalHeader, CallbackType Create) {
  const auto *OriginRoot = OriginalHeader.getRoot();

  if (OriginRoot == nullptr) {
    return {};
  }

  auto NewHeader = NodeBase<NodeType>{true};
  NodeBase<NodeType> *Copy = Create(*OriginRoot);
  const NodeBase<NodeType> *Origin = OriginRoot;
  NewHeader.Parent = Copy;

  while (not Origin->isHeader()) {
    if (shouldGo<Direction::Left>(Origin, Copy)) {
      copyAndGo<Direction::Left>(Origin, Copy, Create);

    } else if (shouldGo<Direction::Right>(Origin, Copy)) {
      copyAndGo<Direction::Right>(Origin, Copy, Create);

    } else {
      Origin = Origin->Parent;
      Copy = Copy->Parent;
    }
  }

  return NewHeader;
}
} // end namespace hammock::utils
