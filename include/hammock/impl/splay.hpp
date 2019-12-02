#pragma once

#include "hammock/utils/iterator.hpp"
#include "hammock/utils/node.hpp"
#include "hammock/utils/rotation.hpp"
#include "hammock/utils/transform.hpp"
#include "hammock/utils/traversal.hpp"

#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace hammock::impl {
template <class KeyType, class ValueType, class Compare = std::less<KeyType>,
          class AllocatorType = std::allocator<std::pair<KeyType, ValueType>>>
class SplayTree {
public:
  using Node = utils::Node<KeyType, ValueType>;
  using CompressedNode = typename Node::Header;
  using HeaderType = CompressedNode;
  using KeyValuePairType = typename Node::Pair;
  using NodeAllocatorType = typename std::allocator_traits<
      AllocatorType>::template rebind_alloc<Node>;

  using iterator = utils::Iterator<SplayTree>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_iterator = utils::Iterator<SplayTree, true>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
      : Size{std::exchange(Origin.Size, 0)},
        Comparator{Origin.Comparator}, Allocator{Origin.Allocator} {
    moveHeader(std::move(Origin.Header));
  }

  SplayTree(const SplayTree &Origin)
      : Size{Origin.Size},
        Comparator{Origin.Comparator}, Allocator{Origin.Allocator} {
    copyTree(Origin.Header);
  }

  void moveHeader(HeaderType &&Origin) noexcept {
    Header.Parent = std::exchange(Origin.Parent, nullptr);
    Header.Left = std::exchange(Origin.Left, nullptr);
    Header.Right = std::exchange(Origin.Right, nullptr);
    if (Header.Parent != nullptr) {
      Header.Parent->Parent = &Header;
    }
  }

  SplayTree &operator=(const SplayTree &Origin) {
    if (this != &Origin) {
      // unlike the case with copy construction we might
      // actually have some data in this tree, we need to clear it
      clear();
      copyTree(Origin.Header);
      Size = Origin.Size;
      Comparator = Origin.Comparator;
      Allocator = Origin.Allocator;
    }
    return *this;
  }

  SplayTree &operator=(SplayTree &&Origin) noexcept {
    moveHeader(std::move(Origin.Header));
    Size = std::exchange(Origin.Size, 0);
    Comparator = Origin.Comparator;
    Allocator = Origin.Allocator;
    return *this;
  }

  ~SplayTree() noexcept { clear(); }

  std::pair<iterator, bool> insert(const KeyValuePairType &ValueToInsert) {
    return insertImpl(ValueToInsert);
  }

  std::pair<iterator, bool> insert(KeyValuePairType &&ValueToInsert) {
    return insertImpl(std::move(ValueToInsert));
  }

  iterator erase(iterator ToErase) {
    if (ToErase == end())
      return ToErase;

    Node *NodeToErase = ToErase.getNode();
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
      utils::swap(NodeToErase, ToErase.getNode());
      // Successor could've had only the right child (otherwise its
      // left child would've been a successor).
      NodeToReplace = NodeToErase->Right;
    }

    // Even if the child doesn't exist it is still a valid replacement.
    utils::replace(NodeToErase, NodeToReplace);

    // If erased node was the root, we need also replace it
    if (getRoot() == NodeToErase) {
      assignRoot(NodeToReplace);
    }

    // Erased node could've been one (or even both) of the shortcuts
    if (NodeToErase == getShortcut<utils::Direction::Left>()) {
      decrementShortcut<utils::Direction::Left>();
    }
    if (NodeToErase == getShortcut<utils::Direction::Right>()) {
      decrementShortcut<utils::Direction::Right>();
    }

    destruct(NodeToErase);
    --Size;

    return ToErase;
  }

  void clear() noexcept {
    utils::preOrderTraverse(getRoot(),
                            [this](Node *ToDelete) { destruct(ToDelete); });
    assignRoot(nullptr);
    Size = 0;
  }

  bool contains(const KeyType &Key) { return find(Key) != end(); }

  std::size_t count(const KeyType &Key) { return contains(Key); }

  ValueType &at(const KeyType &Key) {
    auto it = find(Key);
    if (it == end()) {
      throw std::out_of_range("SplayTree::at");
    }
    return it->second;
  }

  iterator find(const KeyType &Key) {
    auto *Root = getRoot();

    if (Root != nullptr) {

      [[maybe_unused]] const auto [Parent, Node] =
          utils::find(getRoot(), Key, Comparator);
      if (Node) {
        splay(Node);
        return {getRoot()};
      }
    }

    return end();
  }

  iterator begin() { return {getShortcut<utils::Direction::Left>(this)}; }
  iterator end() { return {&Header}; }
  const_iterator begin() const {
    return {getShortcut<utils::Direction::Left>(this)};
  }
  const_iterator end() const { return {&Header}; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  std::size_t size() const { return Size; }
  bool empty() const { return Size == 0; }

private:
  template <class ArgumentType>
  std::pair<iterator, bool> insertImpl(ArgumentType &&ToInsert) {
    auto *Root = getRoot();

    if (Root == nullptr) {
      assignRoot(create(std::forward<ArgumentType>(ToInsert)));
      Root = getRoot();
      Root->Parent = &Header;
      Header.Left = Root;
      Header.Right = Root;

    } else {

      const auto [Parent, WhereTo] =
          utils::find(Root, ToInsert.first, Comparator);

      // We have a value with this key already
      if (WhereTo)
        return {{WhereTo}, false};

      WhereTo = create(std::forward<ArgumentType>(ToInsert));
      WhereTo->Parent = Parent;

      // The new node could've become the left/rightmost node
      // in the tree and we need to make an adjustment to the
      // shortcuts.
      //
      // It will take only O(1) time because it will become
      // either Header.Direction->Direction or stay Header.Direction
      adjustShortcut<utils::Direction::Left>(Header.Left);
      adjustShortcut<utils::Direction::Right>(Header.Right);

      splay(WhereTo);
    }

    ++Size;

    return {{Root}, true};
  }

  void splay(CompressedNode *NodeToMoveToTheTop) {
    utils::splay(NodeToMoveToTheTop);
    assignRoot(NodeToMoveToTheTop);
  }

  void copyTree(const HeaderType &Origin) {
    Header = utils::copyTree(
        Origin, [this](const Node &ToCopy) { return create(ToCopy); });
    if (Header.Parent) {
      Header.Parent->Parent = &Header;
      adjustShortcut<utils::Direction::Left>(getRoot());
      adjustShortcut<utils::Direction::Right>(getRoot());
    }
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

  Node *getRoot() { return Header.getRoot(); }
  void assignRoot(CompressedNode *NewRoot) { Header.Parent = NewRoot; }

  template <utils::Direction Which> void adjustShortcut(Node *Pivot) {
    utils::getChild<Which>(&Header) =
        Pivot == nullptr ? nullptr : utils::getTheOutmost<Which>(Pivot);
  }

  template <utils::Direction Which> auto *getShortcut() {
    return getShortcut<Which>(this);
  }

  template <utils::Direction Which, class ThisPointer>
  static auto *getShortcut(ThisPointer Pointer) {
    return Pointer->Header.Parent == nullptr
               ? &Pointer->Header
               : utils::getChild<Which>(&Pointer->Header);
  }

  template <utils::Direction Which> void decrementShortcut() {
    auto &Shortcut = utils::getChild<Which>(&Header);
    // if the shortcut is the left/rightmost leaf in the tree,
    // than the next node from that order will be its successor node from
    // the oposite direction
    auto *TheSecondOutmostNode = utils::successor<utils::invert(Which)>(
        static_cast<CompressedNode *>(Shortcut));
    if (TheSecondOutmostNode == &Header) {
      // there is no second outmost element, the shortcut should become null
      Shortcut = nullptr;
    } else {
      // it couldn't be a header (it should be only one header in a tree)
      Shortcut = TheSecondOutmostNode->getRealNode();
    }
  }

  HeaderType Header{true};
  std::size_t Size = 0;
  Compare Comparator{};
  NodeAllocatorType Allocator{};
};

} // end namespace hammock::impl
