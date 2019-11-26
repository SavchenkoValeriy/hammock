#pragma once

#include "hammock/utils/iterator.hpp"
#include "hammock/utils/node.hpp"
#include "hammock/utils/rotation.hpp"
#include "hammock/utils/transform.hpp"
#include "hammock/utils/traversal.hpp"

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace hammock::impl {
template <class KeyType, class ValueType, class Compare = std::less<KeyType>,
          class AllocatorType = std::allocator<std::pair<KeyType, ValueType>>>
class SplayTree {
public:
  using Node = utils::Node<KeyType, ValueType>;
  using KeyValuePairType = typename Node::Pair;
  using NodeAllocatorType = typename std::allocator_traits<
      AllocatorType>::template rebind_alloc<Node>;

  using iterator = utils::Iterator<Node>;
  using allocator_type = AllocatorType;

  static_assert(
      std::is_invocable_v<Compare &, const KeyType &, const KeyType &>,
      "comparison object must be invocable with two arguments of key type");

  static_assert(
      std::is_invocable_v<const Compare &, const KeyType &, const KeyType &>,
      "comparison object must be invocable as const");

  SplayTree(std::initializer_list<KeyValuePairType> Initializer) noexcept {
    // TODO: it is a very-very basic version of this insertion
    // it might need to replace existing values in order to
    // replicate std::map's behavior
    for (auto &Pair : Initializer) {
      insert(Pair);
    }
  };

  constexpr SplayTree() noexcept = default;

  SplayTree(SplayTree &&Origin) noexcept
      : Root{std::exchange(Origin.Root, nullptr)}, Size{std::exchange(
                                                       Origin.Size, 0)},
        Comparator{Origin.Comparator}, Allocator{Origin.Allocator} {}

  SplayTree(const SplayTree &Origin)
      : Size{Origin.Size},
        Comparator{Origin.Comparator}, Allocator{Origin.Allocator} {
    Root = copyTree(Origin.Root);
  }

  SplayTree &operator=(const SplayTree &Origin) {
    if (this != &Origin) {
      // unlike the case with copy construction we might
      // actually have some data in this tree, we need to clear it
      clear();
      Root = copyTree(Origin.Root);
      Size = Origin.Size;
      Comparator = Origin.Comparator;
      Allocator = Origin.Allocator;
    }
    return *this;
  }

  SplayTree &operator=(SplayTree &&Origin) noexcept {
    Root = std::exchange(Origin.Root, nullptr);
    Size = std::exchange(Origin.Size, 0);
    Comparator = Origin.Comparator;
    Allocator = Origin.Allocator;
    return *this;
  }

  ~SplayTree() noexcept { clear(); }

  std::pair<iterator, bool> insert(const KeyValuePairType &ValueToInsert) {
    const auto [Parent, WhereTo] =
        utils::find(Root, ValueToInsert.first, Comparator);

    // We have a value with this key already
    if (WhereTo)
      return {{WhereTo}, false};

    WhereTo = create(ValueToInsert);
    WhereTo->Parent = Parent;

    splay(WhereTo);

    ++Size;

    return {{Root}, true};
  }

  iterator erase(iterator ToErase) {
    if (ToErase == end())
      return ToErase;

    Node *NodeToErase = ToErase.CorrespondingNode;
    Node *NodeToReplace = nullptr;

    ++ToErase;
    // Check if we have a right sub-tree...
    if (NodeToErase->Right == nullptr) {
      // ...if we don't, we can simply replace the node of
      // interest with its left child.
      NodeToReplace = NodeToErase->Left;
    } else {
      // ...otherwise the successor of our node is in the right sub-tree.
      // We can swap the node of interest with its successor.
      utils::swap(NodeToErase, ToErase.CorrespondingNode);
      // Successor could've had only the right child (otherwise its
      // left child would've been a successor).
      NodeToReplace = NodeToErase->Right;
    }

    // Even if the child doesn't exist it is still a valid replacement.
    utils::replace(NodeToErase, NodeToReplace);

    // If erased node was the root, we need also replace it
    if (Root == NodeToErase) {
      Root = NodeToReplace;
    }

    destruct(NodeToErase);
    --Size;

    return ToErase;
  }

  void clear() noexcept {
    utils::preOrderTraverse(Root,
                            [this](Node *ToDelete) { destruct(ToDelete); });
    Root = nullptr;
    Size = 0;
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

  Node *copyTree(Node *Origin) {
    return utils::copyTree(
        Origin, [this](const Node &ToCopy) { return create(ToCopy); });
  }

  template <class... ArgsTypes>
  [[nodiscard]] Node *create(ArgsTypes &&... Args) {
    auto *DataChunk =
        std::allocator_traits<NodeAllocatorType>::allocate(Allocator, 1);
    std::allocator_traits<NodeAllocatorType>::construct(
        Allocator, DataChunk, std::forward<ArgsTypes>(Args)...);
    return DataChunk;
  }

  void destruct(Node *ToDealloc) {
    std::allocator_traits<NodeAllocatorType>::destroy(Allocator, ToDealloc);
    std::allocator_traits<NodeAllocatorType>::deallocate(Allocator, ToDealloc,
                                                         1);
  }

  Node *Root = nullptr;
  std::size_t Size = 0;
  Compare Comparator{};
  NodeAllocatorType Allocator{};
};

} // end namespace hammock::impl
