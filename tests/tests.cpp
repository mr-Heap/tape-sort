#include <gtest/gtest.h>
#include <random>

#include "../include/sort.h"

std::vector<int> generate_random_vector() {
    std::vector<int> vec;
    vec.reserve(100);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, 100);

    size_t sz = dist(mt);
    for (int i = 0; i < sz; ++i) {
        vec.emplace_back(dist(mt));
    }
    return vec;
}

TEST(Read, DoubleRead) {
    tape::Tape<int> a("../tmp/file1.txt");
    int first = a.read();
    int second = a.read();
    EXPECT_EQ(first, second);
}

TEST(Write, ConsistentWrite) {
    tape::Tape<int> a("../tmp/file1.txt");
    int number = 12345;
    a.write(number);
    int first = a.read();
    a.left();
    a.write(number + 12345);
    int second = a.read();
    EXPECT_EQ(first, 12345);
    EXPECT_EQ(second, 24690);
}

TEST(ReadWrite, RadAllTape) {
    tape::Tape<int> a("../tmp/file1.txt");
    std::vector<int> vec = generate_random_vector();
    a.move_to_start();
    for (int & i : vec) {
        a.write(i);
        a.left();
    }
    a.right();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], a.read());
        a.right();
    }
}

TEST(Sort, SortRandom) {
    tape::Tape<int> a("../tmp/file1.txt");
    std::vector<int> vec = generate_random_vector();
    a.move_to_start();
    for (int & i : vec) {
        a.write(i);
        a.left();
    }
    a.right();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], a.read());
        a.right();
    }
    tape::Tape<int> b("../tmp/file1-out.txt");

    Sort<int>::sort(a, b, 5);
    std::sort(vec.begin(), vec.end());
    b.move_to_start();
    for(int & i : vec) {
        EXPECT_EQ(i, b.read());
        b.left();
    }
}

TEST(Sort, SortWithComparator) {
    tape::Tape<int> a("../tmp/file1.txt");
    tape::Tape<int> b("../tmp/file1-out.txt");
    std::vector<int> vec = generate_random_vector();
    a.move_to_start();
    for (int & i : vec) {
        a.write(i);
        a.left();
    }
    a.right();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], a.read());
        a.right();
    }

    Sort<int, std::greater<>>::sort(a, b, 5);
    std::sort(vec.begin(), vec.end());
    b.move_to_start();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], b.read());
        b.left();
    }
}

TEST(Sort, SortEmptyFile) {
    tape::Tape<int> a("../tmp/file-empty.txt");
    tape::Tape<int> b("../tmp/file-empty-out.txt");

    Sort<int>::sort(a, b, 5);

    EXPECT_TRUE(b.is_start());
    EXPECT_TRUE(b.is_eot());
}

TEST(Sort, SortCheckM2) {
    tape::Tape<int> a("../tmp/file1.txt");
    std::vector<int> vec = generate_random_vector();
    a.move_to_start();
    for (int & i : vec) {
        a.write(i);
        a.left();
    }
    a.right();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], a.read());
        a.right();
    }
    tape::Tape<int> b("../tmp/file1-out.txt");

    Sort<int>::sort(a, b, 2);
    std::sort(vec.begin(), vec.end());
    b.move_to_start();
    for(int & i : vec) {
        EXPECT_EQ(i, b.read());
        b.left();
    }
}

TEST(Sort, SortBigM) {
    tape::Tape<int> a("../tmp/file1.txt");
    std::vector<int> vec = generate_random_vector();
    a.move_to_start();
    for (int & i : vec) {
        a.write(i);
        a.left();
    }
    a.right();
    for(int i = vec.size() - 1; i >= 0; --i) {
        EXPECT_EQ(vec[i], a.read());
        a.right();
    }
    tape::Tape<int> b("../tmp/file1-out.txt");

    Sort<int>::sort(a, b, 100);
    std::sort(vec.begin(), vec.end());
    b.move_to_start();
    for(int & i : vec) {
        EXPECT_EQ(i, b.read());
        b.left();
    }
}


int main(int argc, char**argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
