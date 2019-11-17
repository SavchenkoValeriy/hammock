#include "hammock/utils/traversal.hpp"

namespace hammock {
  namespace utils {

    template <class Node>
    class Iterator {
    public:
      constexpr Iterator(Node *TreeNode) noexcept : CorrespondingNode(TreeNode) {}

      constexpr Node *operator->() const {
        return CorrespondingNode;
      }

      constexpr Node &operator*() {
        return *CorrespondingNode;
      }

      constexpr Iterator operator++() {
        CorrespondingNode = successor<Direction::Right>(CorrespondingNode);
        return *this;
      }

      constexpr Iterator operator++(int) {
        Iterator Copy = *this;
        operator++();
        return Copy;
      }

      constexpr Iterator operator--() {
        CorrespondingNode = successor<Direction::Left>(CorrespondingNode);
        return *this;
      }

      constexpr Iterator operator--(int) {
        Iterator Copy = *this;
        operator--();
        return Copy;
      }

      constexpr bool operator==(const Iterator<Node> &RHS) const {
        return CorrespondingNode == RHS.CorrespondingNode;
      }

      constexpr bool operator!=(const Iterator<Node> &RHS) const {
        return !(*this == RHS);
      }

    private:
      Node *CorrespondingNode;
    };

  } // end namespace utils
} // end namespace hammock
