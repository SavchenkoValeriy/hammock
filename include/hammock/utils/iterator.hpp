#include "hammock/utils/traversal.hpp"

namespace hammock {
  namespace utils {

    template <class Node>
    class Iterator {
    public:
      Node *operator->() {
        return CorrespondingNode;
      }

      Node &operator*() {
        return *CorrespondingNode;
      }

      Iterator operator++() {
        CorrespondingNode = successor<Direction::Right>(CorrespondingNode);
        return *this;
      }

      Iterator operator++(int) {
        Iterator Copy = *this;
        operator++();
        return Copy;
      }

      Iterator operator--() {
        CorrespondingNode = successor<Direction::Left>(CorrespondingNode);
        return *this;
      }

      Iterator operator--(int) {
        Iterator Copy = *this;
        operator--();
        return Copy;
      }

    private:
      Node *CorrespondingNode;
    };

  } // end namespace utils
} // end namespace hammock
