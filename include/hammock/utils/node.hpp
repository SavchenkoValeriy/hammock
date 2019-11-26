#pragma once

#include <utility>

namespace hammock::utils {
template <class KeyType, class ValueType> struct Node {
  using Pair = std::pair<const KeyType, ValueType>;

  constexpr Node(const Pair &ToStore) noexcept : KeyValuePair(ToStore) {}
  constexpr Node(Pair &&ToStore) noexcept : KeyValuePair(std::move(ToStore)) {}
  // we intentionally do not copy node's edges
  constexpr Node(const Node &Origin) : KeyValuePair{Origin.KeyValuePair} {}

  constexpr const KeyType &Key() const { return KeyValuePair.first; }

  constexpr ValueType &Value() { return KeyValuePair.second; }

  constexpr const ValueType &Value() const { return KeyValuePair.second; }

  Pair KeyValuePair;
  Node *Left = nullptr, *Right = nullptr, *Parent = nullptr;
};

} // end namespace hammock::utils
