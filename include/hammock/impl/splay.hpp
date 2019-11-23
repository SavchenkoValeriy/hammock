#pragma once

#include "hammock/utils/iterator.hpp"
#include "hammock/utils/node.hpp"
#include "hammock/utils/rotation.hpp"
#include "hammock/utils/transform.hpp"
#include "hammock/utils/traversal.hpp"

#include <stdexcept>
#include <type_traits>

namespace hammock::impl {
template <class KeyType, class ValueType, class Compare = std::less<KeyType>>
class SplayTree {
public:
  using Node = utils::Node<KeyType, ValueType>;
  using KeyValuePairType = typename Node::Pair;

  using iterator = utils::Iterator<Node>;

  static_assert(
      std::is_invocable_v<Compare &, const KeyType &, const KeyType &>,
      "comparison object must be invocable with two arguments of key type");

  static_assert(
      std::is_invocable_v<const Compare &, const KeyType &, const KeyType &>,
      "comparison object must be invocable as const");

  constexpr SplayTree() noexcept = default;
  constexpr SplayTree(
      std::initializer_list<KeyValuePairType> Initializer) noexcept {
    // TODO: it is a very-very basic version of this insertion
    // it might need to replace existing values in order to
    // replicate std::map's behavior
    for (auto &Pair : Initializer) {
      insert(Pair);
    }
  };

  std::pair<iterator, bool> insert(const KeyValuePairType &ValueToInsert) {
    const auto [Parent, WhereTo] =
        utils::find(Root, ValueToInsert.first, Comparator);

    // We have a value with this key already
    if (WhereTo)
      return {{WhereTo}, false};

    WhereTo = alloc(ValueToInsert);
    WhereTo->Parent = Parent;

    splay(WhereTo);

    ++Size;

    return {{Root}, true};
  }

  iterator erase(iterator ToErase) {
    if (ToErase == end())
      return ToErase;

    auto *NodeToErase = ToErase.CorrespondingNode;
    Node *&WhereToReplace = NodeToErase == Root ? Root : NodeToErase;

    ++ToErase;
    // Check if we have a right sub-tree...
    if (NodeToErase->Right == nullptr) {
      // ...if we don't, we can simply replace the node of
      // interest with its left child.
      utils::replace(WhereToReplace, NodeToErase->Left);
    } else {
      // ...otherwise the successor of our node is in the right sub-tree.
      // We can swap the node of interest with its successor.
      utils::swap(NodeToErase, ToErase.CorrespondingNode);
      // Successor could've had only the right child (otherwise its
      // left child would've been a successor).
      //
      // Even if the right child doesn't exist it is still a valid
      // replacement.
      utils::replace(WhereToReplace, NodeToErase->Right);
    }

    dealloc(NodeToErase);
    --Size;
    return ToErase;
  }

  ValueType &at(const KeyType &Key) {
    auto it = find(Key);
    if (it == end()) {
      throw std::out_of_range("SplayTree::at");
    }
    return it->second;
  }

  iterator find(const KeyType &Key) {
    [[maybe_unused]] const auto [Parent, Node] =
        utils::find(Root, Key, Comparator);
    if (Node)
      splay(Node);
    return {Root};
  }

  iterator begin() { return {utils::getTheLeftmost(Root)}; }
  iterator end() { return {nullptr}; }

  std::size_t size() const { return Size; }
  bool empty() const { return Size == 0; }

private:
  void splay(Node *NodeToMoveToTheTop) {
    utils::splay(NodeToMoveToTheTop);
    Root = NodeToMoveToTheTop;
  }

  static Node *alloc(const KeyValuePairType &ValueToStore) {
    // TODO: change to allocators
    return new Node(ValueToStore);
  }

  static void dealloc(Node *ToDealloc) {
    // TODO: change to allocators
    delete ToDealloc;
  }

  Node *Root = nullptr;
  std::size_t Size = 0;
  Compare Comparator{};
};

} // end namespace hammock::impl
