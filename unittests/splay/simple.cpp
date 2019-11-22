#include "hammock/impl/splay.hpp"

#include <gtest/gtest.h>

using namespace hammock::impl;

TEST(SplayTest, OrderTest) {
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

TEST(SplayTest, AtTest) {
  SplayTree<int, int> Tree;
  EXPECT_THROW(Tree.at(10), std::out_of_range);
  Tree.insert({1, 20});
  EXPECT_EQ(Tree.at(1), 20);
  Tree.at(1) = 42;
  EXPECT_EQ(Tree.at(1), 42);
}
