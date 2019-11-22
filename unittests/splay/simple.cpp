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

TEST(SplayTest, InitializationTest) {
  std::map<int, double> Standard = {{1, 12.2}, {10, 3.6}, {-5, 36.6}};
  SplayTree<int, double> Tree({{1, 12.2}, {10, 3.6}, {-5, 36.6}});

  auto [StandardIt, TreeIt] =
      std::mismatch(Standard.begin(), Standard.end(), Tree.begin());
  EXPECT_EQ(StandardIt, Standard.end());
  EXPECT_EQ(TreeIt, Tree.end());
}
