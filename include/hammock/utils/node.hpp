#pragma once

#include <cassert>
#include <type_traits>
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
  bool isSpecial() const { return Parent->Parent == this; }

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

template <class KeyTypeT, class ValueTypeT>
struct Node : public NodeBase<Node<KeyTypeT, ValueTypeT>> {
  using Header = NodeBase<Node>;
  using KeyType = KeyTypeT;
  using ValueType = ValueTypeT;
  using Pair = std::pair<const KeyType, ValueType>;

  const KeyType &Key() const { return KeyValuePair().first; }

  ValueType &Value() { return KeyValuePair().second; }
  const ValueType &Value() const { return KeyValuePair().second; }

  Pair *Pointer() { return std::addressof(KeyValuePair()); }
  const Pair *Pointer() const { return std::addressof(KeyValuePair()); }

  Pair &KeyValuePair() { return *reinterpret_cast<Pair *>(&KeyValueBuffer); }
  const Pair &KeyValuePair() const {
    return *reinterpret_cast<const Pair *>(&KeyValueBuffer);
  }

  std::aligned_storage_t<sizeof(Pair), alignof(Pair)> KeyValueBuffer;
};

} // end namespace hammock::utils
