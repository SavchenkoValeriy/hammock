#pragma once

#include "hammock/impl/splay.hpp"
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
