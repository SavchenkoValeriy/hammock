#include "hammock/impl/splay.hpp"

#include <gtest/gtest.h>
#include <iterator>

using namespace hammock::impl;

unsigned insertTypicalSequence(SplayTree<int, int> &Tree) {
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
  return ExpectedSize;
}

void checkKeysIncrease(SplayTree<int, int> &Tree) {
  int PreviousKey = -1;
  for (auto [Key, Value] : Tree) {
    EXPECT_GT(Key, PreviousKey);
    PreviousKey = Key;
  }
}

TEST(SplayTest, OrderTest) {
  SplayTree<int, int> Tree;
  auto ExpectedSize = insertTypicalSequence(Tree);
  checkKeysIncrease(Tree);
  EXPECT_EQ(Tree.size(), ExpectedSize);
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

TEST(SplayTest, EraseTest) {
  SplayTree<int, std::string> Tree = {{1, "hello"}, {3, "world"}, {4, "!"}};

  EXPECT_EQ(Tree.size(), 3);
  Tree.insert({2, ","});
  EXPECT_EQ(Tree.size(), 4);

  auto AfterErased = Tree.erase(std::next(Tree.begin(), 2));
  EXPECT_EQ(Tree.size(), 3);
  auto [Key, Value] = *AfterErased;
  EXPECT_EQ(Key, 4);
  EXPECT_EQ(Value, "!");

  AfterErased = Tree.erase(AfterErased);
  EXPECT_EQ(Tree.size(), 2);
  EXPECT_EQ(AfterErased, Tree.end());

  AfterErased = Tree.erase(AfterErased);
  EXPECT_EQ(Tree.size(), 2);
  EXPECT_EQ(AfterErased, Tree.end());

  AfterErased = Tree.erase(Tree.begin());
  EXPECT_EQ(Tree.size(), 1);
  EXPECT_EQ(AfterErased, Tree.begin());

  AfterErased = Tree.erase(AfterErased);
  EXPECT_TRUE(Tree.empty());
  EXPECT_EQ(AfterErased, Tree.end());

  Tree.insert({1, "hello, world!"});
  EXPECT_EQ(Tree.size(), 1);
  auto [NewRootKey, NewRootValue] = *Tree.begin();
  EXPECT_EQ(NewRootKey, 1);
  EXPECT_EQ(NewRootValue, "hello, world!");
}

TEST(SplayTest, InsertUniqueTest) {
  SplayTree<int, unsigned> Tree;
  std::vector Source = {1, 42, 52, 1, 2, 2, 42, 0, -1, 38, 1, 0, 0, 0};

  for (int Element : Source) {
    if (auto [Iterator, FirstTime] = Tree.insert({Element, 1}); not FirstTime) {
      ++(Iterator->second);
    }
  }

  EXPECT_EQ(Tree.at(0), 4);
  EXPECT_EQ(Tree.at(-1), 1);
  EXPECT_EQ(Tree.at(1), 3);
  EXPECT_EQ(Tree.at(2), 2);
  EXPECT_EQ(Tree.at(38), 1);
  EXPECT_EQ(Tree.at(42), 2);
  EXPECT_EQ(Tree.at(52), 1);
}

struct VerySimpleStruct {
  int Value = 10;
};

struct ComparatorForAVerySimpleStruct {
  bool operator()(const VerySimpleStruct &LHS,
                  const VerySimpleStruct &RHS) const {
    return LHS.Value > RHS.Value;
  }
};

TEST(SplayTest, CustomComparator) {
  SplayTree<VerySimpleStruct, double, ComparatorForAVerySimpleStruct> Tree;
  Tree.insert({{20}, 22.4});
  Tree.insert({{1}, 36.6});
  Tree.insert({{}, 0.0});

  int PreviousKeyValue = INT_MAX;
  for (auto [Key, Value] : Tree) {
    EXPECT_LT(Key.Value, PreviousKeyValue);
    PreviousKeyValue = Key.Value;
  }
}

TEST(SplayTest, CopyAndClearTest) {
  SplayTree<int, int> Tree;
  auto ExpectedSize = insertTypicalSequence(Tree);

  SplayTree<int, int> Copy{Tree};
  EXPECT_EQ(Copy.size(), Tree.size());
  Tree.clear();
  EXPECT_TRUE(Tree.empty());
  EXPECT_EQ(Copy.size(), ExpectedSize);

  checkKeysIncrease(Copy);
  EXPECT_EQ(Copy.size(), ExpectedSize);
}

TEST(SplayTest, MoveTest) {
  SplayTree<int, int> Tree;
  auto ExpectedSize = insertTypicalSequence(Tree);

  SplayTree<int, int> Copy{std::move(Tree)};
  EXPECT_TRUE(Tree.empty());
  EXPECT_EQ(Copy.size(), ExpectedSize);

  checkKeysIncrease(Copy);
  EXPECT_EQ(Copy.size(), ExpectedSize);
}

TEST(SplayTest, CopyAssignmentAndClearTest) {
  SplayTree<int, int> Tree, Copy;
  auto ExpectedSize = insertTypicalSequence(Tree);
  Copy = Tree;

  EXPECT_EQ(Copy.size(), Tree.size());
  Tree.clear();
  EXPECT_TRUE(Tree.empty());
  EXPECT_EQ(Copy.size(), ExpectedSize);

  checkKeysIncrease(Copy);
  EXPECT_EQ(Copy.size(), ExpectedSize);
}

TEST(SplayTest, MoveAssignmentTest) {
  SplayTree<int, int> Tree, Copy;
  auto ExpectedSize = insertTypicalSequence(Tree);

  Copy = std::move(Tree);
  EXPECT_TRUE(Tree.empty());
  EXPECT_EQ(Copy.size(), ExpectedSize);

  checkKeysIncrease(Copy);
  EXPECT_EQ(Copy.size(), ExpectedSize);
}
