#pragma once
#include "utils.h"
#include <fstream>

// hash of a single block (stored as raw bytes, size depends on algorithm)
using block_hash = std::vector<uint8_t>;

// lazy block reader for a single file.
// reads one block of size blockSize at a time; pads the last block with zeros.
// no disk i/o happens until nextBlock() is called.
class reader {
public:
    inline block_hash compute_hash(const std::vector<char>& buf, hash_func algo);
    reader(fs::path path, std::size_t block_size, hash_func algo);

    void reset();

    // returns the file path
    const fs::path& path() const;

    // total number of blocks (ceil(fileSize / blockSize), min 1)
    std::size_t total_blocks() const;

    // index of the next block to be read (0-based)
    std::size_t current_index() const;

    // true if all blocks have been read
    bool exhausted() const;

    // reads the next block from disk, hashes it and returns the hash.
    // pads with zeros if the last block is shorter than blockSize.
    // throws std::runtime_error if the file cannot be opened/read.
    block_hash next_block();
private:
    std::size_t block_count() const;

    fs::path        path_;
    std::size_t     block_size_;
    hash_func       algo_;
    std::size_t     block_index_;
    std::uintmax_t  file_size_;
    std::size_t     total_blocks_;
    std::ifstream   stream_;
};
