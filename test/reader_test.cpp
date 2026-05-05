#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "reader.h"

namespace fs = boost::filesystem;

// test fixture: temporary directory tree
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

// reader tests

class reader_test : public file_system_test {};

// construction

TEST_F(reader_test, total_blocks_exact_multiple)
{
    auto p = make_file("a.txt", "ABCDEF"); // 6 bytes, block=3 → 2 blocks
    reader r(p, 3, hash_func::md5);
    EXPECT_EQ(r.total_blocks(), 2u);
}

TEST_F(reader_test, total_blocks_with_remainder)
{
    auto p = make_file("a.txt", "ABCDE"); // 5 bytes, block=3 → 2 blocks (last padded)
    reader r(p, 3, hash_func::md5);
    EXPECT_EQ(r.total_blocks(), 2u);
}

TEST_F(reader_test, total_blocks_smaller_than_block)
{
    auto p = make_file("a.txt", "Hi"); // 2 bytes, block=10 → 1 block
    reader r(p, 10, hash_func::md5);
    EXPECT_EQ(r.total_blocks(), 1u);
}

TEST_F(reader_test, empty_file_is_one_block)
{
    auto p = make_empty_file("empty.txt");
    reader r(p, 4, hash_func::md5);
    EXPECT_EQ(r.total_blocks(), 1u);
}

TEST_F(reader_test, block_size_zero_throws)
{
    auto p = make_file("a.txt", "data");
    EXPECT_THROW(reader(p, 0, hash_func::md5), std::invalid_argument);
}

TEST_F(reader_test, block_size_one_byte_exact)
{
    auto p = make_file("a.txt", "ABC"); // 3 bytes, block=1 → 3 blocks
    reader r(p, 1, hash_func::md5);
    EXPECT_EQ(r.total_blocks(), 3u);
}

// lazy open: no i/o before nextBlock() 

TEST_F(reader_test, not_exhausted_before_reading)
{
    auto p = make_file("a.txt", "data");
    reader r(p, 4, hash_func::md5);
    EXPECT_FALSE(r.exhausted());
    EXPECT_EQ(r.current_index(), 0u);
}

// nextBlock() 

TEST_F(reader_test, read_single_block)
{
    auto p = make_file("a.txt", "ABCD");
    reader r(p, 4, hash_func::md5);
    EXPECT_NO_THROW(r.next_block());
    EXPECT_TRUE(r.exhausted());
}

TEST_F(reader_test, read_all_blocks)
{
    auto p = make_file("a.txt", "ABCDEF"); // 6 bytes, block=2 → 3 blocks
    reader r(p, 2, hash_func::md5);
    for (std::size_t i = 0; i < 3; ++i) {
        EXPECT_FALSE(r.exhausted());
        r.next_block();
    }
    EXPECT_TRUE(r.exhausted());
}

TEST_F(reader_test, next_block_past_end_throws)
{
    auto p = make_file("a.txt", "AB");
    reader r(p, 2, hash_func::md5);
    r.next_block();
    EXPECT_THROW(r.next_block(), std::out_of_range);
}

TEST_F(reader_test, same_content_same_hash)
{
    auto p = make_file("a.txt", "Hello");
    reader r1(p, 5, hash_func::md5);
    reader r2(p, 5, hash_func::md5);
    EXPECT_EQ(r1.next_block(), r2.next_block());
}

TEST_F(reader_test, different_content_different_hash)
{
    auto p1 = make_file("a.txt", "Hello");
    auto p2 = make_file("b.txt", "World");
    reader r1(p1, 5, hash_func::md5);
    reader r2(p2, 5, hash_func::md5);
    EXPECT_NE(r1.next_block(), r2.next_block());
}

TEST_F(reader_test, last_block_zero_padded)
{
    // "AB\0" and "AB" with block=3 should produce same hash
    auto p1 = make_file("a.txt", std::string("AB\0", 3));
    auto p2 = make_file("b.txt", "AB"); // 2 bytes → padded to "AB\0"
    reader r1(p1, 3, hash_func::md5);
    reader r2(p2, 3, hash_func::md5);
    EXPECT_EQ(r1.next_block(), r2.next_block());
}

TEST_F(reader_test, crc32_hash_works)
{
    auto p = make_file("a.txt", "test data");
    reader r(p, 9, hash_func::crc32);
    block_hash h;
    EXPECT_NO_THROW(h = r.next_block());
    EXPECT_EQ(h.size(), 4u); // crc32 = 4 bytes
}

TEST_F(reader_test, md5_hash_works)
{
    auto p = make_file("a.txt", "test data");
    reader r(p, 9, hash_func::md5);
    block_hash h;
    EXPECT_NO_THROW(h = r.next_block());
    EXPECT_EQ(h.size(), 16u); // md5 = 16 bytes
}

//  reset() 

TEST_F(reader_test, reset_allows_rereading)
{
    auto p = make_file("a.txt", "ABCD");
    reader r(p, 4, hash_func::md5);
    block_hash h1 = r.next_block();
    r.reset();
    EXPECT_EQ(r.current_index(), 0u);
    block_hash h2 = r.next_block();
    EXPECT_EQ(h1, h2);
}