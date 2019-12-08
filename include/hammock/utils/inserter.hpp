#pragma once

namespace hammock::utils {
template <class KeyGetter, class NodeGetter> struct Inserter {
  Inserter(KeyGetter Key, NodeGetter Node) : getKey(Key), getNode(Node) {}

  KeyGetter getKey;
  NodeGetter getNode;
};
} // end namespace hammock::utils
