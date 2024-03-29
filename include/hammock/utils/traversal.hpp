#pragma once

#include "hammock/utils/direction.hpp"
#include "hammock/utils/node.hpp"

#include <cassert>
#include <stack>
#include <utility>

namespace hammock::utils {

/// @brief Get the left/rightmost node starting from the given node.
///
/// @tparam To  The direction for which the outmost node should be retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the left/rightmost child for.
///
/// @return  A pointer to the left/rightmost child of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <Direction To, class NodeType>
constexpr inline NodeType *getTheOutmost(NodeType *Node) {
  assert("The starting node should not be null" && Node != nullptr);

  for (auto *Child = getChild<To>(Node); Child != nullptr;
       Node = Child, Child = getChild<To>(Node)) {
  }
  return Node;
}

/// @brief Get the leftmost node starting from the given node.
///
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the leftmost child for.
///
/// @return  A pointer to the leftmost child of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <class NodeType>
constexpr inline NodeType *getTheLeftmost(NodeType *Node) {
  return getTheOutmost<Direction::Left>(Node);
}

/// @brief Get the rightmost node starting from the given node.
///
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the rightmost child for.
///
/// @return  A pointer to the rightmost child of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <class NodeType>
constexpr inline NodeType *getTheRightmost(NodeType *Node) {
  return getTheOutmost<Direction::Right>(Node);
}

/// @brief Get the left/rightmost leaf starting from the given node.
///
/// @tparam To  The direction for which the outmost leaf node should be
/// retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the left/rightmost child leaf for.
///
/// @return  A pointer to the left/rightmost child leaf of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <Direction To, class NodeType>
constexpr inline NodeType *getTheOutmostLeaf(NodeType *Node) {
  assert("The starting node should not be null" && Node != nullptr);

  constexpr Direction From = invert(To);
  Node = getTheOutmost<To>(Node);

  for (auto *Child = getChild<From>(Node); Child != nullptr;
       Node = getTheOutmost<To>(Child), Child = getChild<From>(Node)) {
  }

  return Node;
}

/// @brief Get the leftmost leaf starting from the given node.
///
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the leftmost child leaf for.
///
/// @return  A pointer to the leftmost child leaf of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <Direction To, class NodeType>
constexpr inline NodeType *getTheLeftmostLeaf(NodeType *Node) {
  return getTheOutmostLeaf<Direction::Left>(Node);
}

/// @brief Get the rightmost leaf starting from the given node.
///
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the rightmost child leaf for.
///
/// @return  A pointer to the rightmost child leaf of the given @p Node.
///          The result pointer can be equal to the given @p Node.
///
/// @pre  @p Node should not be null
template <Direction To, class NodeType>
constexpr inline NodeType *getTheRightmostLeaf(NodeType *Node) {
  return getTheOutmostLeaf<Direction::Right>(Node);
}

/// @brief Test whether the given node is the left/right child.
///
/// @tparam TestedDirection  The direction to check.
/// @tparam NodeType  Type of the node.
///
/// @param Node  A node to check.
///
/// @return  True if the given @p Node is its Parent's child from the
///          given direction and false otherwise.
///
/// @pre  The node's parent should not be null, which is always true
///       for the well-formed tree.
template <Direction TestedDirection, class NodeType>
constexpr inline bool isChild(NodeType *Node) {
  return getChild<TestedDirection>(Node->Parent) == Node;
}

/// @brief Return the reference to the parent's pointer to the given node.
///
/// @tparam NodeType  Type of the node.
///
/// @param Node  The node to get the location for.
///
/// @return  The reference to a child in the parent's node.
///
/// @pre  The given @p Node should not be neither the header nor the root.
///
/// @post  The return value casted to the pointer is equal to the given @p Node.
template <class NodeType>
constexpr inline auto &getParentLocation(NodeType *Node) {
  assert("The node should not be the root node" && not Node->isRoot());
  assert("The node should not be the header node" && not Node->isHeader());

  if (isChild<Direction::Right>(Node)) {
    return Node->Parent->Right;
  } else {
    // We could've made another check and have the third branch for the case
    // when Node->Parent->Parent == Node, but it is a pretty rare case for big
    // trees. It is only two nodes like this in every tree and we can check that
    // only on occasions when it is possible that we deal with special nodes.
    return Node->Parent->Left;
  }
}

/// @brief Return the inorder successor of the given node.
///
/// @tparam To  The direction for which the successor should be retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  A node to retrieve the successor for.
///
/// @return  A pointer to the next node according to the inorder traversal.
///          The successor of the last node is the header of the tree.
///
/// @pre  The given @p Node should not be null.
template <Direction To, class NodeType>
constexpr inline NodeType *successorInOrder(NodeType *Node) {
  assert("Successor can be calculated only for a valid node" &&
         Node != nullptr);

  constexpr Direction From = invert(To);

  // This case is here specifically for decrementing the end() iterator.
  //
  // If we are in the header node, we should start the traversing from
  // the very beginning, i.e. from the outmost leaf in the 'From' direction.
  //
  // It might be included in Direction::Right case, but we assume that
  // incrementing the end() iterator shouldn't happen at all and we can
  // save an additional check for that matter.
  if constexpr (To == Direction::Left) {
    if (Node->isHeader()) {
      // Header's right child points to the rightmost node of the whole tree.
      // If we want to go backwards from starting the header node, this is where
      // we should start.
      return Node->Right;
    }
  }

  // If there is a sub-tree in a direction of traversal, we should
  // definitely go there...
  if (getChild<To>(Node) != nullptr) {
    // ...starting from the closest node
    return getTheOutmost<From>(getChild<To>(Node));
  }

  // Otherwise we should try our best higher in the tree.
  auto *Parent = Node->Parent;
  // If we come to the parent from the direction 'To', we are done with
  // sub-tree and we should keep going up.
  //
  // If we came from the 'From' direction, it is the right time to visit
  // this parent (in terms of inorder traversal).
  //
  // This loop is not infinite as every tree is guaranteed to have a header,
  // which we'll be the parent of the root node.
  for (; getChild<To>(Parent) == Node and not Parent->isHeader();
       Node = Parent, Parent = Node->Parent) {
  }
  return Parent;
}

/// @brief Return the postorder successor of the given node.
///
/// @tparam To  The direction for which the successor should be retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  A node to retrieve the successor for.
///
/// @return  A pointer to the next node according to the postorder traversal.
///          The successor of the last node is the header of the tree.
///
/// @pre  @p Node should not be null.
template <Direction To, class NodeType>
constexpr inline NodeType *successorPostOrder(NodeType *Node) {
  assert("Successor can be calculated only for a valid node" &&
         Node != nullptr);

  constexpr Direction From = invert(To);

  // This case is here specifically for decrementing the end() iterator.
  //
  // If we are in the header node, we should start the traversing from
  // the very beginning, i.e. from the outmost leaf in the 'From' direction.
  //
  // It might be included in Direction::Right case, but we assume that
  // incrementing the end() iterator shouldn't happen at all and we can
  // save an additional check for that matter.
  if constexpr (To == Direction::Left) {
    if (Node->isHeader()) {
      // We need to start over from the outmost leaf of the whole tree
      return getTheOutmostLeaf<From>(Node->getRoot());
    }
  }

  // It's a post-order traversal that means that we already visited
  // Node's left and right subtrees.
  auto *Parent = Node->Parent;
  if (Parent->isHeader())
    return Parent;

  // If we are coming from the 'From' sub-tree, we should try to go
  // to the 'To' sub-tree...
  if (isChild<From>(Node) and getChild<To>(Parent) != nullptr) {
    return getTheOutmostLeaf<From>(getChild<To>(Parent));
  }

  // ...if there is no 'To' sub-tree or we just came from there,
  // we have visited all of the nodes reachable from the 'Parent' node.
  //
  // This means that we should visit it right now.
  return Parent;
}

/// @brief Return the preorder successor of the given node.
///
/// @tparam To  The direction for which the successor should be retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  A node to retrieve the successor for.
///
/// @return  A pointer to the next node according to the preorder traversal.
///          The successor of the last node is the header of the tree.
///
/// @pre  @p Node should not be null.
template <Direction To, class NodeType>
constexpr inline NodeType *successorPreOrder(NodeType *Node) {
  assert("Successor can be calculated only for a valid node" &&
         Node != nullptr);

  constexpr Direction From = invert(To);

  // This case is here specifically for decrementing the end() iterator.
  //
  // If we are in the header node, we should start the traversing from
  // the very beginning, i.e. from the outmost leaf in the 'From' direction.
  //
  // It might be included in Direction::Right case, but we assume that
  // incrementing the end() iterator shouldn't happen at all and we can
  // save an additional check for that matter.
  if constexpr (To == Direction::Left) {
    if (Node->isHeader()) {
      // Starting a new traversal from root node
      return Node->getRoot();
    }
  }

  // If we can go to the From direction - do it!
  if (getChild<From>(Node) != nullptr) {
    return getChild<From>(Node);
  }

  // At this point, we either should go to the 'To' child (if we have one),
  // or start walking back up.
  //
  // All the parents of this node, though, have been visited already and the
  // only possibility left is to go from the parent node to the 'To' child as
  // well (if we didn't not come from it, of course).
  //
  // These cases can be united in the following algorithm:
  //   Try to go to the 'To' child if we didn't visit all the subtree of this
  //   node already. If we couldn't or we did visit it, try further up.
  //   We start with the current node and the fact that we didn't visit all of
  //   the child nodes. When we go up, we think that we are done with the
  //   subtree if we came from the 'To' child.
  for (bool VisitTheWholeSubTree = false; not Node->isHeader();
       VisitTheWholeSubTree = isChild<To>(Node), Node = Node->Parent) {
    if (not VisitTheWholeSubTree and getChild<To>(Node) != nullptr) {
      return getChild<To>(Node);
    }
  }

  // Didn't find any nodes to go further - simply return the header (the end).
  return Node;
}

enum class TraversalKind { InOrder, PreOrder, PostOrder };

/// @brief Return successor of the given node.
///
/// @tparam Order  Order of traversal
///
/// @tparam To  The direction for which the successor should be retrieved.
/// @tparam NodeType  Type of the node.
///
/// @param Node  A node to retrieve the successor for.
///
/// @return  A pointer to the next node according to the inorder traversal.
///          The successor of the last node is the header of the tree.
///
/// @pre  @p Node should not be null.
template <Direction To, TraversalKind Order, class NodeType>
constexpr inline NodeType *successor(NodeType *Node) {
  if constexpr (Order == TraversalKind::InOrder) {
    return successorInOrder<To>(Node);
  } else if constexpr (Order == TraversalKind::PreOrder) {
    return successorPreOrder<To>(Node);
  } else {
    return successorPostOrder<To>(Node);
  }
}

/// @brief Find the node by its key in the valid binary search tree.
///
/// @tparam NodeType  Type of the node.
/// @tparam Compare  Type of the comparator function.
///
/// @param Node  A node to start the search from.
/// @param Key  The key of the node (or location) to find.
/// @param Comparator  A comparator function for keys.
///
/// @return  A pair, of which the first element is a pointer to the parent
///          of the node to be found. It is always non-null. However, it is
///          valid iff the node was not found. The second element is a reference
///          to the node pointer. This pointer is either null if the node was
///          not found or the actual pointer. The reference though is guaranteed
///          to be valid and refers to the place whether the node is located or
///          should have been located.
///
/// @pre  The tree rooted in the given @p Node should be a valid BST w.r.t the
///       given @p Comparator.
/// @pre  The given @p Node is not null.
template <class NodeType, class Compare>
constexpr inline std::pair<NodeType *, NodeType *&>
find(NodeType *Node, const typename NodeType::KeyType &Key,
     Compare Comparator) {
  assert(("The node to start the search from should not be null" &&
          Node != nullptr));

  NodeType *Parent = nullptr;
  // Node is a local variable and Result should never maintain this value
  // and that is why Node should not be null.
  // We should have at least one iteration of the loop.
  NodeType **Result = &Node;

  // We denote the relationship between keys provided by the comparator
  // as the follows:
  //   A ≺ B <=> Comparator(A, B) == true
  //
  // We base the search procedure on the fact that for every node N with
  // the key P the following is always true:
  //   * for every key K from the left subtree of N -> K ≺ P
  //   * for every key K from the right subtree of N -> P ≺ K

  // Iterate till we get to the point where there is no node
  while (*Result != nullptr) {
    Parent = *Result;

    if (Comparator(Parent->Key(), Key)) {
      // If the current node's key P ≺ K, we need to go to the right subtree
      Result = &Parent->Right;

    } else if (Comparator(Key, Parent->Key())) {
      // ...otherwise if K ≺ P, we should go to the left subtree
      Result = &Parent->Left;

    } else {
      // ...there is only one possibility left - P == K,
      // which means that we found the requested node.
      //
      // *Result holds it already, so we can simply break out of the loop.
      break;
    }
  }
  return {Parent, *Result};
}

/// @brief Check if we should go in the given direction for copying.
///
/// @param Origin  A node from the original tree.
/// @param Copy  A corresponding node from the copied tree.
///
/// @tparam To  A direction to check.
/// @tparam NodeType  Type of the node.
///
/// @return  true if we should visit the given direction for the purpose of
///          copying and false otherwise
///
/// @pre  @p Origin is not null
/// @pre  @p Copy is not null
///
/// @note  This function makes sense only in the context of the @ref copyTree
///        function.
template <Direction To, class NodeType>
constexpr inline bool shouldGo(const NodeType *Origin, const NodeType *Copy) {
  // We should go in this direction if the original node has a child here,
  // while the copy still doesn't
  return getChild<To>(Origin) != nullptr and getChild<To>(Copy) == nullptr;
}

/// @brief Copy the child from the given direction and move in this direction.
///
/// @param Origin  A node from the original tree.
/// @param Copy  A corresponding node from the copied tree.
/// @param Create  A function that copies the given node and creates a new node.
///
/// @tparam To  A direction to copy the child from and to go.n
/// @tparam NodeType  Type of the node.
/// @tparam CallbackType  Type of the @p Create function.
///
/// @pre  @p Origin is not null
/// @pre  @p getChild<To>(Origin) is not null
/// @pre  @p Copy is not null
/// @pre  @tp NodeType is copyable
///
/// @note  This function makes sense only in the context of the @ref copyTree
///        function.
template <Direction To, class NodeType, class CallbackType>
constexpr inline void copyAndGo(const NodeType *&Origin, NodeType *&Copy,
                                CallbackType Create) {
  // Copy the node first
  getChild<To>(Copy) = Create(*getChild<To>(Origin));
  getChild<To>(Copy)->Parent = Copy;
  // And go in that direction with both trees
  Origin = getChild<To>(Origin);
  Copy = getChild<To>(Copy);
}

/// @brief Copy the given tree.
///
/// @param OriginalHeader  A header node of the original tree.
/// @param Create  A function that copies the given node and creates a new node.
///
/// @tparam NodeType  Type of the node.
/// @tparam CallbackType  Type of the @p Create function.
///
/// @return  A header node of the copied tree.
///
/// @pre  @tp NodeType is copyable
///
/// @note  This function does NOT copy left/right pointers of the header node
///        so it is the caller's responsibility to fix them after the copying.
template <class NodeType, class CallbackType>
constexpr inline NodeBase<NodeType>
copyTree(const NodeBase<NodeType> &OriginalHeader, CallbackType Create) {
  using Base = NodeBase<NodeType>;
  const auto *OriginRoot = OriginalHeader.getRoot();

  // It looks like we are copying an empty tree
  if (OriginRoot == nullptr) {
    return {true};
  }

  auto NewHeader = Base{true};
  Base *Copy = Create(*OriginRoot);
  // In order for us to traverse the tree up and down without additional
  // conversions, we should do it with a pointer to the base class of the node.
  const Base *Origin = OriginRoot;
  NewHeader.Parent = Copy;

  // The main idea of the following algorithm is to traverse the tree with two
  // pointers at once. We want to copy children of the node only after we copy
  // the node itself. For this reason, pre-order traversal is the best fit.
  //
  // We don't use a standard pre-order traversal (@ref successorPreOrder)
  // because we need repeat all of the transitions for the copying pointer as
  // well.
  //
  // We should stop if we got back to the header node.
  while (not Origin->isHeader()) {
    // Try to copy the left sub-tree...
    if (shouldGo<Direction::Left>(Origin, Copy)) {
      copyAndGo<Direction::Left>(Origin, Copy, Create);

    } else if (shouldGo<Direction::Right>(Origin, Copy)) {
      // ...or the right sub-tree if the left one is already copied.
      copyAndGo<Direction::Right>(Origin, Copy, Create);

    } else {
      // Otherwise it looks like all the children of the node are copied and
      // we should walk the tree further up.
      Origin = Origin->Parent;
      Copy = Copy->Parent;
    }
  }

  return NewHeader;
}
} // end namespace hammock::utils
