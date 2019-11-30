#pragma once

#include <cassert>
#include <utility>

namespace hammock::utils {
template <class Derived> class NodeBase {
public:
  Derived *getRealNode() {
    assert(("Header is not a real node" && not isHeader()));
    return static_cast<Derived *>(this);
  }

  const Derived *getRealNode() const {
    assert(("Header is not a real node" && not isHeader()));
    return static_cast<const Derived *>(this);
  }

  NodeBase() = default;

  NodeBase(bool Header) noexcept : HeaderFlag(Header) {}

  // copying pointers from another tree is pointless
  // Derived class can have its own copy construction/assignment
  NodeBase(const NodeBase &) = delete;
  NodeBase &operator=(const NodeBase &) = delete;

  NodeBase(NodeBase &&) noexcept = default;
  NodeBase &operator=(NodeBase &&) noexcept = default;

  bool isHeader() const { return HeaderFlag; }
  bool isRoot() const { return Parent->isHeader(); }

  Derived *getRoot() {
    assert(("Only header has a direct access to the root" && isHeader()));
    return Parent == nullptr ? nullptr : Parent->getRealNode();
  }

  const Derived *getRoot() const {
    assert(("Only header has a direct access to the root" && isHeader()));
    return Parent == nullptr ? nullptr : Parent->getRealNode();
  }

  NodeBase *Parent = nullptr;
  Derived *Left = nullptr, *Right = nullptr;

private:
  // TODO: figure out how to avoid having this extra bit of information
  bool HeaderFlag = false;
};

template <class KeyType, class ValueType>
struct Node : public NodeBase<Node<KeyType, ValueType>> {
  using Header = NodeBase<Node>;
  using Pair = std::pair<const KeyType, ValueType>;

  Node(const Pair &ToStore) : KeyValuePair(ToStore) {}
  Node(Pair &&ToStore) noexcept : KeyValuePair(std::move(ToStore)) {}
  // we intentionally do not copy node's edges
  Node(const Node &Origin) : KeyValuePair{Origin.KeyValuePair} {}

  const KeyType &Key() const { return KeyValuePair.first; }

  ValueType &Value() { return KeyValuePair.second; }

  const ValueType &Value() const { return KeyValuePair.second; }

  Pair KeyValuePair;
};

} // end namespace hammock::utils
