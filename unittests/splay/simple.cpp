#include "hammock/impl/splay.hpp"

#include <gtest/gtest.h>

using namespace hammock::impl;

TEST(SimpleSplayTest, OrderTest) {
  SplayTree<int, int> Tree;
  unsigned ExpectedSize = 0;
  EXPECT_TRUE(Tree.empty());

  for (int i = 0; i <= 20; ++i) {
    Tree.insert({i * 2, i});
    EXPECT_EQ(Tree.size(), ++ExpectedSize);
  }

  for (int i = 0; i <= 20; ++i) {
    Tree.insert({i * 2 + 1, i});
    EXPECT_EQ(Tree.size(), ++ExpectedSize);
  }

  int PreviousKey = -1;
  for (auto [Key, Value] : Tree) {
    EXPECT_GT(Key, PreviousKey);
    PreviousKey = Key;
  }

  EXPECT_EQ(Tree.size(), 42);
}
