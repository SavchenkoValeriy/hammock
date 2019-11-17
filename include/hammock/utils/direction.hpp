#pragma once

namespace hammock::utils {
enum class Direction : bool { Left, Right };

constexpr inline Direction invert(Direction Original) {
  return Original == Direction::Left ? Direction::Right : Direction::Left;
}

template <class NodeType> constexpr inline auto getMemberPointer(Direction To) {
  return To == Direction::Left ? &NodeType::Left : &NodeType::Right;
}

template <Direction To, class NodeType>
constexpr inline auto &getChild(NodeType *X) {
  constexpr auto MemberPointer = getMemberPointer<NodeType>(To);
  return X->*MemberPointer;
}
} // end namespace hammock::utils
