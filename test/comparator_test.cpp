#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "reader.h"
#include "comparator.h"

namespace fs = boost::filesystem;

//           
// test fixture: temporary directory tree
//           
class file_system_test : public ::testing::Test {
protected:
    void SetUp() override
    {
        root_ = fs::temp_directory_path() / fs::unique_path("bayan_test_%%%%-%%%%");
        fs::create_directories(root_);
    }

    void TearDown() override { fs::remove_all(root_); }

    // writes content to a file and returns its path
    fs::path make_file(const std::string& name, const std::string& content)
    {
        fs::path p = root_ / name;
        std::ofstream f(p.string(), std::ios::binary);
        f.write(content.data(), static_cast<std::streamsize>(content.size()));
        return p;
    }

    // creates an empty file
    fs::path make_empty_file(const std::string& name)
    {
        return make_file(name, "");
    }

    // creates a subdirectory and returns its path
    fs::path make_dir(const std::string& name)
    {
        fs::path p = root_ / name;
        fs::create_directories(p);
        return p;
    }

    fs::path root_;
};

//           
// comparator tests
//           

class comparator_test : public file_system_test {};

TEST_F(comparator_test, identical_files_are_equal)
{
    auto p1 = make_file("a.txt", "Hello, World");
    auto p2 = make_file("b.txt", "Hello, World");
    reader r1(p1, 4, hash_func::md5);
    reader r2(p2, 4, hash_func::md5);
    EXPECT_TRUE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, different_files_not_equal)
{
    auto p1 = make_file("a.txt", "Hello, World");
    auto p2 = make_file("b.txt", "Hello, C+++");
    reader r1(p1, 4, hash_func::md5);
    reader r2(p2, 4, hash_func::md5);
    EXPECT_FALSE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, different_sizes_not_equal)
{
    auto p1 = make_file("a.txt", "Hello");
    auto p2 = make_file("b.txt", "Hello, World");
    reader r1(p1, 4, hash_func::md5);
    reader r2(p2, 4, hash_func::md5);
    EXPECT_FALSE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, two_empty_files_are_equal)
{
    auto p1 = make_empty_file("a.txt");
    auto p2 = make_empty_file("b.txt");
    reader r1(p1, 4, hash_func::md5);
    reader r2(p2, 4, hash_func::md5);
    EXPECT_TRUE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, same_first_block_different_second)
{
    // files share first block "Hello" but differ in second
    auto p1 = make_file("a.txt", "Hello, Worl");
    auto p2 = make_file("b.txt", "Hello, C+++");
    reader r1(p1, 5, hash_func::md5);
    reader r2(p2, 5, hash_func::md5);
    EXPECT_FALSE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, single_byte_files_equal_content)
{
    auto p1 = make_file("a.txt", "X");
    auto p2 = make_file("b.txt", "X");
    reader r1(p1, 1, hash_func::crc32);
    reader r2(p2, 1, hash_func::crc32);
    EXPECT_TRUE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, single_byte_files_different_content)
{
    auto p1 = make_file("a.txt", "X");
    auto p2 = make_file("b.txt", "Y");
    reader r1(p1, 1, hash_func::crc32);
    reader r2(p2, 1, hash_func::crc32);
    EXPECT_FALSE(comparator::are_equal(r1, r2));
}

TEST_F(comparator_test, find_duplicates_three_files)
{
    auto p1 = make_file("a.txt", "duplicate");
    auto p2 = make_file("b.txt", "duplicate");
    auto p3 = make_file("c.txt", "unique___");
    auto readers = std::vector<std::shared_ptr<reader>>{
        std::make_shared<reader>(p1, 4, hash_func::md5),
        std::make_shared<reader>(p2, 4, hash_func::md5),
        std::make_shared<reader>(p3, 4, hash_func::md5),
    };
    auto groups = comparator::find_duplicates(readers);
    ASSERT_EQ(groups.size(), 1u);
    EXPECT_EQ(groups[0].size(), 2u);
}

TEST_F(comparator_test, find_duplicates_no_duplicates)
{
    auto p1 = make_file("a.txt", "aaaaaaaaa");
    auto p2 = make_file("b.txt", "bbbbbbbbb");
    auto readers = std::vector<std::shared_ptr<reader>>{
        std::make_shared<reader>(p1, 4, hash_func::md5),
        std::make_shared<reader>(p2, 4, hash_func::md5),
    };
    auto groups = comparator::find_duplicates(readers);
    EXPECT_TRUE(groups.empty());
}

TEST_F(comparator_test, find_duplicates_all_identical)
{
    auto p1 = make_file("a.txt", "same");
    auto p2 = make_file("b.txt", "same");
    auto p3 = make_file("c.txt", "same");
    auto readers = std::vector<std::shared_ptr<reader>>{
        std::make_shared<reader>(p1, 4, hash_func::md5),
        std::make_shared<reader>(p2, 4, hash_func::md5),
        std::make_shared<reader>(p3, 4, hash_func::md5),
    };
    auto groups = comparator::find_duplicates(readers);
    ASSERT_EQ(groups.size(), 1u);
    EXPECT_EQ(groups[0].size(), 3u);
}

TEST_F(comparator_test, find_duplicates_empty_list)
{
    std::vector<std::shared_ptr<reader>> readers;
    auto groups = comparator::find_duplicates(readers);
    EXPECT_TRUE(groups.empty());
}

TEST_F(comparator_test, find_duplicates_two_separate_groups)
{
    auto p1 = make_file("a.txt", "group1__");
    auto p2 = make_file("b.txt", "group1__");
    auto p3 = make_file("c.txt", "group2__");
    auto p4 = make_file("d.txt", "group2__");
    auto readers = std::vector<std::shared_ptr<reader>>{
        std::make_shared<reader>(p1, 4, hash_func::md5),
        std::make_shared<reader>(p2, 4, hash_func::md5),
        std::make_shared<reader>(p3, 4, hash_func::md5),
        std::make_shared<reader>(p4, 4, hash_func::md5),
    };
    auto groups = comparator::find_duplicates(readers);
    EXPECT_EQ(groups.size(), 2u);
}