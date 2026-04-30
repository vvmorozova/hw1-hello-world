#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#include "cmd_args_parser.h"

namespace fs = boost::filesystem;

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

// build a null-terminated argv from a list of string literals
struct Argv {
    explicit Argv(std::vector<std::string> args) : storage(std::move(args))
    {
        for (auto& s : storage) ptrs.push_back(const_cast<char*>(s.data()));
        ptrs.push_back(nullptr);
    }
    int argc() const { return static_cast<int>(storage.size()); }
    char** argv() { return ptrs.data(); }

private:
    std::vector<std::string> storage;
    std::vector<char*>       ptrs;
};
// fixture that creates two temporary directories and cleans them up
class cmd_args_parser_test : public ::testing::Test {
protected:
    void SetUp() override
    {
        dir1 = fs::temp_directory_path() / fs::unique_path("bayan_test_%%%%-%%%%");
        dir2 = fs::temp_directory_path() / fs::unique_path("bayan_test_%%%%-%%%%");
        fs::create_directories(dir1);
        fs::create_directories(dir2);
    }

    void TearDown() override
    {
        fs::remove_all(dir1);
        fs::remove_all(dir2);
    }

    fs::path dir1, dir2;
    cmd_args_parser parser;
};

// ---------------------------------------------------------------------------
// 1. --scan-dir
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, SingleScanDir)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    ASSERT_EQ(cfg.scan_dirs.size(), 1u);
    EXPECT_EQ(cfg.scan_dirs[0], fs::canonical(dir1));
}

TEST_F(cmd_args_parser_test, MultipleScanDirs)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), dir2.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_dirs.size(), 2u);
}

TEST_F(cmd_args_parser_test, NoScanDirThrows)
{
    Argv a({"bayan"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, NonExistentScanDirThrows)
{
    Argv a({"bayan", "--scan-dir", "/this/path/does/not/exist"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, FilenameAsScanDirThrows)
{
    // create a regular file, not a directory
    fs::path f = dir1 / "file.txt";
    { std::ofstream(f.string()); }
    Argv a({"bayan", "--scan-dir", f.string()});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, ShortFlagScanDir)
{
    Argv a({"bayan", "-d", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_dirs.size(), 1u);
}

// ---------------------------------------------------------------------------
// 2. --exclude-dir
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, ExcludeDirIsStored)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--exclude-dir", dir2.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    ASSERT_EQ(cfg.exclude_dirs.size(), 1u);
    EXPECT_EQ(cfg.exclude_dirs[0], fs::canonical(dir2));
}

TEST_F(cmd_args_parser_test, NonExistentExcludeDirSilentlyIgnored)
{
    Argv a({"bayan", "--scan-dir", dir1.string(),
            "--exclude-dir", "/no/such/dir"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_TRUE(cfg.exclude_dirs.empty());
}

TEST_F(cmd_args_parser_test, MultipleExcludeDirs)
{
    Argv a({"bayan", "--scan-dir", dir1.string(),
            "--exclude-dir", dir1.string(), dir2.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.exclude_dirs.size(), 2u);
}

// ---------------------------------------------------------------------------
// 3. --depth
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, DepthDefaultIsZero)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_depth, 0);
}

TEST_F(cmd_args_parser_test, DepthZeroExplicit)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--depth", "0"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_depth, 0);
}

TEST_F(cmd_args_parser_test, DepthPositive)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--depth", "5"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_depth, 5);
}

TEST_F(cmd_args_parser_test, DepthNegativeThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--depth", "-1"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, DepthShortFlag)
{
    Argv a({"bayan", "-d", dir1.string(), "-l", "3"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_depth, 3);
}

// ---------------------------------------------------------------------------
// 4. --min-size
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, MinSizeDefaultIsTwo)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.min_size, 2u);
}

TEST_F(cmd_args_parser_test, MinSizeBoundaryTwo)
{
    // 2 is the minimum valid value
    Argv a({"bayan", "--scan-dir", dir1.string(), "--min-size", "2"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.min_size, 2u);
}

TEST_F(cmd_args_parser_test, MinSizeOneThrows)
{
    // 1 means "files > 1 byte" is not satisfied — must be >= 2
    Argv a({"bayan", "--scan-dir", dir1.string(), "--min-size", "1"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, MinSizeZeroThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--min-size", "0"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, MinSizeLargeValue)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--min-size", "1073741824"}); // 1 GiB
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.min_size, 1073741824u);
}

// ---------------------------------------------------------------------------
// 5. --mask
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, MasksDefaultEmpty)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_TRUE(cfg.masks.empty());
}

TEST_F(cmd_args_parser_test, SingleMask)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--mask", "*.txt"});
    auto cfg = parser.parse(a.argc(), a.argv());
    ASSERT_EQ(cfg.masks.size(), 1u);
    EXPECT_EQ(cfg.masks[0], "*.txt");
}

TEST_F(cmd_args_parser_test, MultipleMasks)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--mask", "*.txt", "*.cpp", "*.h"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.masks.size(), 3u);
}

TEST_F(cmd_args_parser_test, MaskPreservedAsIs)
{
    // parser stores masks verbatim; case folding is the scanner's job
    Argv a({"bayan", "--scan-dir", dir1.string(), "--mask", "*.TXT"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.masks[0], "*.TXT");
}

// ---------------------------------------------------------------------------
// 6. --block-size
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, BlockSizeDefault)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.block_size, 4096u);
}

TEST_F(cmd_args_parser_test, BlockSizeOne)
{
    // minimum valid block size
    Argv a({"bayan", "--scan-dir", dir1.string(), "--block-size", "1"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.block_size, 1u);
}

TEST_F(cmd_args_parser_test, BlockSizeZeroThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--block-size", "0"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, BlockSizeLargeValue)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--block-size", "1048576"}); // 1 MiB
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.block_size, 1048576u);
}

TEST_F(cmd_args_parser_test, BlockSizeShortFlag)
{
    Argv a({"bayan", "-d", dir1.string(), "-s", "512"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.block_size, 512u);
}

// ---------------------------------------------------------------------------
// 7. --hash
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, HashDefaultIsMd5)
{
    Argv a({"bayan", "--scan-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.hash_algo, hash_func::md5);
}

TEST_F(cmd_args_parser_test, HashMd5Explicit)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--hash", "md5"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.hash_algo, hash_func::md5);
}

TEST_F(cmd_args_parser_test, HashCrc32)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--hash", "crc32"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.hash_algo, hash_func::crc32);
}

TEST_F(cmd_args_parser_test, HashUnknownThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--hash", "sha256"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, HashEmptyStringThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--hash", ""});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, HashCaseWrongThrows)
{
    // algorithm names are case-sensitive
    Argv a({"bayan", "--scan-dir", dir1.string(), "--hash", "MD5"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// 8. --help
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, HelpThrowshelp_req)
{
    Argv a({"bayan", "--help"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), cmd_args_parser::help_req);
}

TEST_F(cmd_args_parser_test, HelpShortFlag)
{
    Argv a({"bayan", "-h"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), cmd_args_parser::help_req);
}

TEST_F(cmd_args_parser_test, HelpMessageNotEmpty)
{
    Argv a({"bayan", "--help"});
    try {
        parser.parse(a.argc(), a.argv());
        FAIL() << "expected help_req";
    } catch (const cmd_args_parser::help_req& hr) {
        EXPECT_FALSE(std::string(hr.what()).empty());
    }
}

// ---------------------------------------------------------------------------
// 9. unknown / malformed arguments
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, UnknownOptionThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--unknown-opt"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, DepthNonIntegerThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--depth", "abc"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

TEST_F(cmd_args_parser_test, BlockSizeNonIntegerThrows)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--block-size", "4kb"});
    EXPECT_THROW(parser.parse(a.argc(), a.argv()), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// 10. combined / integration scenarios
// ---------------------------------------------------------------------------

TEST_F(cmd_args_parser_test, AllOptionsAtOnce)
{
    Argv a({
        "bayan",
        "--scan-dir",    dir1.string(), dir2.string(),
        "--exclude-dir", dir2.string(),
        "--depth",       "2",
        "--min-size",    "1024",
        "--mask",        "*.txt", "*.log",
        "--block-size",  "8192",
        "--hash",        "crc32"
    });
    auto cfg = parser.parse(a.argc(), a.argv());

    EXPECT_EQ(cfg.scan_dirs.size(),    2u);
    EXPECT_EQ(cfg.exclude_dirs.size(), 1u);
    EXPECT_EQ(cfg.scan_depth,          2);
    EXPECT_EQ(cfg.min_size,            1024u);
    EXPECT_EQ(cfg.masks.size(),        2u);
    EXPECT_EQ(cfg.block_size,          8192u);
    EXPECT_EQ(cfg.hash_algo,           hash_func::crc32);
}

TEST_F(cmd_args_parser_test, ScanDirAndExcludeDirSamePath)
{
    // excluding the scan dir itself is allowed at parse level
    Argv a({"bayan", "--scan-dir", dir1.string(), "--exclude-dir", dir1.string()});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_dirs[0], cfg.exclude_dirs[0]);
}

TEST_F(cmd_args_parser_test, DepthVeryLarge)
{
    Argv a({"bayan", "--scan-dir", dir1.string(), "--depth", "999"});
    auto cfg = parser.parse(a.argc(), a.argv());
    EXPECT_EQ(cfg.scan_depth, 999);
}

TEST_F(cmd_args_parser_test, EmptyMaskStringStoredAsIs)
{
    // an explicit empty mask token is stored verbatim
    Argv a({"bayan", "--scan-dir", dir1.string(), "--mask", ""});
    auto cfg = parser.parse(a.argc(), a.argv());
    ASSERT_EQ(cfg.masks.size(), 1u);
    EXPECT_EQ(cfg.masks[0], "");
}