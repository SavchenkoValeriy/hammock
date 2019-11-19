#include "hammock/impl/splay.hpp"

#include <gtest/gtest.h>

using namespace hammock::impl;

TEST(SimpleSplayTest, OrderTest) {
  SplayTree<int, int> Tree;
  for (int i = 0; i <= 20; ++i) {
    Tree.insert({i * 2, i});
  }

  for (int i = 0; i <= 20; ++i) {
    Tree.insert({i * 2 + 1, i});
  }

  int PreviousKey = -1;
  for (auto [Key, Value] : Tree) {
    EXPECT_GT(Key, PreviousKey);
    PreviousKey = Key;
  }
}
