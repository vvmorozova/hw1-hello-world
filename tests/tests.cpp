#include <gtest/gtest.h>
#include "matrix.h"

TEST(MatrixTest, DefaultValueReturned) {
    Matrix<int, -1> mat;
    EXPECT_EQ((int)mat[0][0], -1);
    EXPECT_EQ((int)mat[5][10], -1);
    EXPECT_EQ((int)mat[100][200], -1);
}

TEST(MatrixTest, AssignAndRead) {
    Matrix<int, 0> mat;
    mat[3][4] = 42;
    EXPECT_EQ((int)mat[3][4], 42);
}

TEST(MatrixTest, AssignDefaultValueRemovesCell) {
    Matrix<int, 0> mat;
    mat[1][1] = 99;
    EXPECT_EQ(mat.getOcc(), 1);
    mat[1][1] = 0; // def val
    EXPECT_EQ(mat.getOcc(), 0);
}

TEST(MatrixTest, OccupancyCount) {
    Matrix<int, -1> mat;
    EXPECT_EQ(mat.getOcc(), 0);
    mat[0][0] = 1;
    mat[1][2] = 2;
    mat[3][3] = 3;
    EXPECT_EQ(mat.getOcc(), 3);
}

TEST(MatrixTest, OverwriteDoesNotIncreaseGetOcc) {
    Matrix<int, 0> mat;
    mat[2][2] = 10;
    mat[2][2] = 20;
    EXPECT_EQ(mat.getOcc(), 1);
    EXPECT_EQ((int)mat[2][2], 20);
}

TEST(MatrixTest, IterationOverOccCells) {
    Matrix<int, 0> mat;
    mat[0][0] = 1;
    mat[1][1] = 2;
    mat[2][2] = 3;

    int sum = 0;
    for (auto& [key, val] : mat) {
        sum += val;
    }
    EXPECT_EQ(sum, 6);
    EXPECT_EQ(mat.getOcc(), 3);
}

TEST(MatrixTest, NegativeIndices) {
    Matrix<int, -1> mat;
    mat[-1][-1] = 55;
    EXPECT_EQ((int)mat[-1][-1], 55);
    EXPECT_EQ((int)mat[-2][-2], -1);
}

TEST(MatrixTest, Manycells) {
    Matrix<int, 0> mat;
    for (int i = 0; i < 100; ++i) {
        mat[i][i] = i + 1;
    }
    EXPECT_EQ(mat.getOcc(), 100);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ((int)mat[i][i], i + 1);
    }
    // 
    EXPECT_EQ((int)mat[0][1], 0);
}

TEST(MatrixTest, IsolatedAssign) {
    Matrix<int, 0> mat;
    mat[5][5] = 77;
    EXPECT_EQ((int)mat[5][5], 77);
    EXPECT_EQ((int)mat[5][4], 0);
    EXPECT_EQ((int)mat[4][5], 0);
    EXPECT_EQ((int)mat[6][6], 0);
    EXPECT_EQ(mat.getOcc(), 1);
}