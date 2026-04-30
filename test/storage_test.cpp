#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "storage.h"
#include "utils.h"

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
// storage tests
//           

class storage_test : public file_system_test {
protected:
    // builds a minimal config pointing at root_
    config make_config()
    {
        config cfg;
        cfg.scan_dirs  = { fs::canonical(root_) };
        cfg.scan_depth = 0;
        cfg.min_size   = 2;
        cfg.block_size = 4;
        cfg.hash_algo  = hash_func::md5;
        return cfg;
    }
};

TEST_F(storage_test, collects_files_in_top_dir)
{
    make_file("a.txt", "hello");
    make_file("b.txt", "world");
    auto cfg = make_config();
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 2u);
}

TEST_F(storage_test, excludes_subdirs_when_depth_zero)
{
    make_file("top.txt", "top");
    auto sub = make_dir("sub");
    { std::ofstream f((sub / "deep.txt").string()); f << "deep"; }
    auto cfg = make_config();
    cfg.scan_depth = 0;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 1u);
}

TEST_F(storage_test, includes_subdirs_when_depth_one)
{
    make_file("top.txt", "topfile");
    auto sub = make_dir("sub");
    { std::ofstream f((sub / "deep.txt").string()); f << "deepfile"; }
    auto cfg = make_config();
    cfg.scan_depth = 1;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 2u);
}

TEST_F(storage_test, depth_two_reaches_nested_dir)
{
    auto sub1 = make_dir("a");
    auto sub2 = sub1 / "b";
    fs::create_directories(sub2);
    { std::ofstream f((sub2 / "deep.txt").string()); f << "deep"; }
    make_file("top.txt", "topfile");
    auto cfg = make_config();
    cfg.scan_depth = 2;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 2u);
}

TEST_F(storage_test, depth_one_does_not_reach_nested)
{
    auto sub1 = make_dir("a");
    auto sub2 = sub1 / "b";
    fs::create_directories(sub2);
    { std::ofstream f((sub2 / "deep.txt").string()); f << "deep"; }
    auto cfg = make_config();
    cfg.scan_depth = 1;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 0u);
}

TEST_F(storage_test, min_size_filters_small_files)
{
    make_file("big.txt", "bigfile"); // 7 bytes
    make_file("tiny.txt", "x");     // 1 byte → excluded (min_size=2)
    auto cfg = make_config();
    cfg.min_size = 2;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 1u);
}

TEST_F(storage_test, min_size_boundary_exactly_equal)
{
    make_file("exact.txt", "AB"); // exactly 2 bytes = min_size → included
    auto cfg = make_config();
    cfg.min_size = 2;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 1u);
}

TEST_F(storage_test, min_size_boundary_one_below_excluded)
{
    make_file("small.txt", "A"); // 1 byte < min_size=2 → excluded
    auto cfg = make_config();
    cfg.min_size = 2;
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_TRUE(readers.empty());
}

TEST_F(storage_test, excluded_dir_is_skipped)
{
    auto ex = make_dir("excluded");
    { std::ofstream f((ex / "file.txt").string()); f << "secret"; }
    make_file("visible.txt", "visible");
    auto cfg = make_config();
    cfg.scan_depth   = 1;
    cfg.exclude_dirs = { fs::canonical(ex) };
    storage st(cfg);
    auto readers = st.collect();
    ASSERT_EQ(readers.size(), 1u);
    EXPECT_EQ(readers[0]->path().filename().string(), "visible.txt");
}

TEST_F(storage_test, mask_filters_extension)
{
    make_file("a.txt", "hello1");
    make_file("b.cpp", "hello2");
    auto cfg = make_config();
    cfg.masks = { "*.txt" };
    storage st(cfg);
    auto readers = st.collect();
    ASSERT_EQ(readers.size(), 1u);
    EXPECT_EQ(readers[0]->path().extension().string(), ".txt");
}

TEST_F(storage_test, mask_is_case_insensitive)
{
    make_file("readme.TXT", "content");
    auto cfg = make_config();
    cfg.masks = { "*.txt" }; // lower-case mask should match .TXT
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 1u);
}

TEST_F(storage_test, multiple_masks_any_match)
{
    make_file("a.txt", "hello1");
    make_file("b.cpp", "hello2");
    make_file("c.log", "hello3");
    auto cfg = make_config();
    cfg.masks = { "*.txt", "*.cpp" };
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 2u);
}

TEST_F(storage_test, no_mask_accepts_all_files)
{
    make_file("a.txt", "hello1");
    make_file("b.cpp", "hello2");
    make_file("c.log", "hello3");
    auto cfg = make_config();
    cfg.masks = {}; // no filter
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 3u);
}

TEST_F(storage_test, empty_directory_returns_no_readers)
{
    auto cfg = make_config();
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_TRUE(readers.empty());
}

TEST_F(storage_test, multiple_scan_dirs)
{
    auto dir2 = fs::temp_directory_path() / fs::unique_path("bayan2_%%%%-%%%%");
    fs::create_directories(dir2);
    make_file("a.txt", "hello1");
    { std::ofstream f((dir2 / "b.txt").string()); f << "hello2"; }

    auto cfg = make_config();
    cfg.scan_dirs.push_back(fs::canonical(dir2));
    storage st(cfg);
    auto readers = st.collect();
    EXPECT_EQ(readers.size(), 2u);

    fs::remove_all(dir2);
}