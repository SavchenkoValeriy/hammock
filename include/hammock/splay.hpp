#pragma once

#include "hammock/splay-impl.hpp"
#include "hammock/utils/traversal.hpp"

#include <map>
#include <memory>

namespace hammock {
template <class KeyType, class ValueType> class SplayTree {
public:
private:
  impl::SplayTree<KeyType, ValueType> Impl;
};
} // end namespace hammock
