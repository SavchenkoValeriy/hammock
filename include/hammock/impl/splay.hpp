#pragma once

#include "hammock/utils/iterator.hpp"
#include "hammock/utils/node.hpp"
#include "hammock/utils/rotation.hpp"
#include "hammock/utils/traversal.hpp"

#include <stdexcept>

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

    ++Size;
  }

  void erase(iterator ToErase) {
    // TODO: implement
  }

  ValueType &at(const KeyType &Key) {
    auto it = find(Key);
    if (it == end()) {
      throw std::out_of_range("SplayTree::at");
    }
    return it->second;
  }

  iterator find(const KeyType &Key) {
    [[maybe_unused]] const auto [Parent, Node] = utils::find(Root, Key);
    if (Node)
      splay(Node);
    return {Node};
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

  Node *Root = nullptr;
  std::size_t Size = 0;
};

} // end namespace hammock::impl
