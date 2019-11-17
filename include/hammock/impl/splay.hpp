#pragma once

#include "hammock/utils/iterator.hpp"
#include "hammock/utils/node.hpp"
#include "hammock/utils/rotation.hpp"
#include "hammock/utils/traversal.hpp"

namespace hammock::impl {
template <class KeyType, class ValueType> class SplayTree {
public:
  using Node = utils::Node<KeyType, ValueType>;
  using KeyValuePairType = typename Node::Pair;

  using iterator = utils::Iterator<Node>;

  // TODO: change to pair<iterator, bool>
  void insert(const KeyValuePairType &ValueToInsert) {
    const auto [Parent, WhereTo] = utils::find(Root, ValueToInsert.first);

    // We have a value with this key already
    if (WhereTo)
      return;

    WhereTo = alloc(ValueToInsert);
    WhereTo->Parent = Parent;

    splay(WhereTo);
  }

  void erase(iterator ToErase) {
    // TODO: implement
  }

  iterator find(const KeyType &Key) {
    [[maybe_unused]] const auto [Parent, Node] = utils::find(Root, Key);
    if (Node)
      splay(Node);
    return {Node};
  }

  iterator begin() { return {utils::getTheLeftmost(Root)}; }
  iterator end() { return {nullptr}; }

private:
  void splay(Node *NodeToMoveToTheTop) {
    utils::splay(NodeToMoveToTheTop);
    Root = NodeToMoveToTheTop;
  }

  static Node *alloc(const KeyValuePairType &ValueToStore) {
    // TODO: change to allocators
    return new Node(ValueToStore);
  }

  Node *Root = nullptr;
};

} // end namespace hammock::impl
