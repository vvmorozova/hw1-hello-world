#include "gtest/gtest.h

TEST(VersionTest,ValideVersion) {
    EXPECT_GT(version(), 0); // Assertion: Expect Factorial(0) to equal 1
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}