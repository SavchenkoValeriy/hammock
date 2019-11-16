#pragma once

#include <utility>

namespace hammock::utils {
template <class KeyType, class ValueType> struct Node {
  using Pair = std::pair<KeyType, ValueType>;

  Node(const Pair &ToStore) : KeyValuePair(ToStore) {}

  KeyType &Key() { return KeyValuePair.first; }

  const KeyType &Key() const { return KeyValuePair.first; }

  ValueType &Value() { return KeyValuePair.second; }

  const ValueType &Value() const { return KeyValuePair.second; }

  Pair KeyValuePair;
  Node *Left = nullptr, *Right = nullptr, *Parent = nullptr;
};

} // end namespace hammock::utils
