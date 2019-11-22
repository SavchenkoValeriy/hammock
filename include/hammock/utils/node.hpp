#pragma once

#include <utility>

namespace hammock::utils {
template <class KeyType, class ValueType> struct Node {
  using Pair = std::pair<const KeyType, ValueType>;

  constexpr Node(const Pair &ToStore) noexcept : KeyValuePair(ToStore) {}

  const KeyType &Key() const { return KeyValuePair.first; }

  ValueType &Value() { return KeyValuePair.second; }

  const ValueType &Value() const { return KeyValuePair.second; }

  Pair KeyValuePair;
  Node *Left = nullptr, *Right = nullptr, *Parent = nullptr;
};

} // end namespace hammock::utils
