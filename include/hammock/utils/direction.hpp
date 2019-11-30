#pragma once

namespace hammock::utils {
enum class Direction : bool { Left, Right };

constexpr inline Direction invert(Direction Original) {
  return Original == Direction::Left ? Direction::Right : Direction::Left;
}

template <Direction To, class NodeType>
constexpr inline auto &getChild(NodeType *X) {
  if constexpr (To == Direction::Left) {
    return X->Left;
  } else {
    return X->Right;
  }
}
} // end namespace hammock::utils
