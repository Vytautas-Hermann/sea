#include "sealib/collection/avltree.h"
#include <gtest/gtest.h>
#include <cstdio>
#include <random>

using namespace Sealib;  // NOLINT

TEST(AVLTreeTest, basicInsert) {
    AVLTree t;
    t.insert(5, 1);
    t.insert(0, 2);
    t.insert(10, 3);
    EXPECT_EQ(t.search(5), 1);
    EXPECT_EQ(t.search(0), 2);
    EXPECT_EQ(t.search(10), 3);
    t.insert(0, 5);
    EXPECT_EQ(t.search(0), 5);
}

TEST(AVLTreeTest, basicRemove) {
    AVLTree t;
    t.insert(5);
    t.insert(7);
    t.insert(3);
    t.insert(6);
    t.insert(1);
    t.remove(3);
    t.remove(5);
    t.remove(7);
    EXPECT_EQ(t.search(3), INVALID);
    EXPECT_EQ(t.search(5), INVALID);
    EXPECT_EQ(t.search(7), INVALID);
    EXPECT_EQ(t.search(6), 0);
    EXPECT_EQ(t.search(1), 0);
}

TEST(AVLTreeTest, insertList) {
    std::random_device rnd;
    std::uniform_int_distribution<uint64_t> dist(0, 1e9);
    AVLTree t;
    std::vector<uint64_t> v;
    for (uint64_t a = 0; a < 1e4; a++) {
        v.push_back(dist(rnd));
        t.insert(a, v.back());
    }
    for (uint64_t b = 0; b < v.size(); b++) {
        EXPECT_EQ(t.search(b), v[b]) << b;
    }
}

TEST(AVLTreeTest, random) {
    std::random_device rnd;
    std::uniform_int_distribution<uint64_t> dist(0, 1e9);
    AVLTree t;
    std::vector<uint64_t> i;
    std::vector<uint64_t> v;
    for (uint64_t a = 0; a < 10; a++) {
        v.push_back(dist(rnd));
        i.push_back(a);
    }
    std::shuffle(i.begin(), i.end(), rnd);
    for (uint64_t a = 0; a < i.size(); a++) {
        t.insert(i[a], v[a]);
    }
    for (uint64_t a = 0; a < i.size(); a++) {
        EXPECT_EQ(t.search(i[a]), v[a]);
    }
    for (uint64_t a = i.size() / 4; a < i.size() / 2; a++) {
        t.remove(i[a]);
    }
    for (uint64_t a = 0; a < i.size(); a++) {
        if (a == i.size() / 4) a = i.size() / 2;
        EXPECT_EQ(t.search(i[a]), v[a]) << a;
    }
}

TEST(AVLTreeTest, sample1) {
    AVLTree t;
    t.insert(2);
    t.insert(6);
    t.insert(9);
    t.insert(1);
    t.insert(3);
    t.insert(5);
    t.insert(0);
    t.insert(8);
    t.insert(4);
    t.insert(7);
    for (uint64_t a = 0; a < 10; a++) {
        EXPECT_EQ(t.search(a), 0) << a;
    }
}
TEST(AVLTreeTest, sample2) {
    AVLTree t;
    t.insert(8);
    t.insert(9);
    t.insert(4);
    t.insert(2);
    t.insert(6);
    t.insert(1);
    t.insert(0);
    t.insert(5);
    t.insert(7);
    t.insert(3);
    t.remove(4);
    for (uint64_t a = 0; a < 10; a++) {
        if (a == 4)
            EXPECT_EQ(t.search(a), INVALID);
        else
            EXPECT_EQ(t.search(a), 0);
    }
}
TEST(AVLTreeTest, sample3) {
    AVLTree t;
    t.insert(7);
    t.insert(9);
    t.insert(2);
    t.insert(8);
    t.insert(5);
    t.insert(3);
    t.insert(4);
    t.insert(0);
    t.insert(1);
    t.insert(6);
    t.remove(2);
    t.remove(8);
    for (uint64_t a = 0; a < 10; a++) {
        if (a == 2 || a == 8)
            EXPECT_EQ(t.search(a), INVALID);
        else
            EXPECT_EQ(t.search(a), 0);
    }
}

TEST(AVLTreeTest, swapLeavesR) {
    AVLTree t;
    t.insert(6, 1);
    t.insert(4, 2);
    t.insert(5, 3);
    EXPECT_EQ(t.search(5), 3);
    EXPECT_EQ(t.search(4), 2);
    EXPECT_EQ(t.search(6), 1);
}
TEST(AVLTreeTest, swapLeavesL) {
    AVLTree t;
    t.insert(4, 1);
    t.insert(6, 2);
    t.insert(5, 3);
    EXPECT_EQ(t.search(5), 3);
    EXPECT_EQ(t.search(6), 2);
    EXPECT_EQ(t.search(4), 1);
}

TEST(AVLTreeTest, rotateTreeR) {
    AVLTree t;
    t.insert(10, 1);
    t.insert(8, 2);
    t.insert(12, 0);
    t.insert(6, 0);
    t.insert(9, 0);
    t.insert(4, 3);
    EXPECT_EQ(t.search(6), 0);
    EXPECT_EQ(t.search(9), 0);
}
TEST(AVLTreeTest, rotateTreeL) {
    AVLTree t;
    t.insert(10, 1);
    t.insert(12, 2);
    t.insert(8, 0);
    t.insert(11, 0);
    t.insert(14, 0);
    t.insert(16, 3);
    EXPECT_EQ(t.search(11), 0);
    EXPECT_EQ(t.search(14), 0);
}

TEST(AVLTreeTest, spliceTreeR) {
    AVLTree t;
    t.insert(10, 1);
    t.insert(4, 2);
    t.insert(12, 0);
    t.insert(2, 0);
    t.insert(8, 0);
    t.insert(11, 0);
    t.insert(13, 0);
    t.insert(1, 0);
    t.insert(3, 0);
    t.insert(5, 0);
    t.insert(9, 0);
    t.insert(6, 3);
    EXPECT_EQ(t.search(5), 0);
    EXPECT_EQ(t.search(9), 0);
}
TEST(AVLTreeTest, spliceTreeL) {
    AVLTree t;
    t.insert(5, 1);
    t.insert(12, 2);
    t.insert(2, 0);
    t.insert(1, 0);
    t.insert(4, 0);
    t.insert(8, 0);
    t.insert(14, 0);
    t.insert(6, 0);
    t.insert(10, 0);
    t.insert(13, 0);
    t.insert(16, 0);
    t.insert(9, 3);
    EXPECT_EQ(t.search(6), 0);
    EXPECT_EQ(t.search(10), 0);
}
