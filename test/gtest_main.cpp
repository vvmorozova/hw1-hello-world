#include <gtest/gtest.h>
#include "lib.h"
#include "version.h"

TEST(VersionTest,ValidVersion) {
    EXPECT_EQ(version(), PROJECT_VERSION_PATCH);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}