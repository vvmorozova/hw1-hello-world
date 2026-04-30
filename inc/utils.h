#pragma once

#include <vector>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

// supported hashing algorithms
enum class hash_func { crc32, md5 };

// parsed and validated command-line configuration
struct config {
    std::vector<fs::path> scan_dirs;       // directories to scan
    std::vector<fs::path> exclude_dirs;    // directories to skip
    int scan_depth       = 0;              // 0 = no recursion
    std::uintmax_t min_size = 2;           // minimum file size in bytes (> 1)
    std::vector<std::string> masks;        // allowed filename masks (case-insensitive)
    std::size_t block_size = 4096;         // read block size S
    hash_func hash_algo = hash_func::md5; // hashing function H
};
