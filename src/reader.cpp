#include "reader.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

#include "cmd_args_parser.h"

namespace fs = boost::filesystem;

inline block_hash reader::compute_hash(const std::vector<char>& buf, hash_func algo)
{
    if (algo == hash_func::crc32) {
        boost::crc_32_type crc;
        crc.process_bytes(buf.data(), buf.size());
        uint32_t val = crc.checksum();
        block_hash h(4);
        // store in little-endian order
        h[0] = (val >>  0) & 0xff;
        h[1] = (val >>  8) & 0xff;
        h[2] = (val >> 16) & 0xff;
        h[3] = (val >> 24) & 0xff;
        return h;
    }

    // md5
    using md5_t = boost::uuids::detail::md5;
    md5_t hasher;
    hasher.process_bytes(buf.data(), buf.size());
    md5_t::digest_type digest;
    hasher.get_digest(digest);
    block_hash h(16);
    for (int i = 0; i < 4; ++i) {
        h[i * 4 + 0] = (digest[i] >>  0) & 0xff;
        h[i * 4 + 1] = (digest[i] >>  8) & 0xff;
        h[i * 4 + 2] = (digest[i] >> 16) & 0xff;
        h[i * 4 + 3] = (digest[i] >> 24) & 0xff;
    }
    return h;
}


reader::reader(fs::path path, std::size_t blockSize, hash_func algo)
    : path_(std::move(path)), block_size_(blockSize), algo_(algo),
    block_index_(0), file_size_(fs::file_size(path_)), total_blocks_(block_count())
{
    if (block_size_ == 0)
        throw std::invalid_argument("block size must be > 0");
}

const fs::path& reader::path() const {
    return path_;
}

std::size_t reader::total_blocks() const {
    return total_blocks_;
}

std::size_t reader::current_index() const {
    return block_index_;
}

bool reader::exhausted() const {
    return block_index_ >= total_blocks_;
}

block_hash reader::next_block()
{
    if (exhausted())
        throw std::out_of_range("no more blocks in: " + path_.string());

    // open on first read
    if (!stream_.is_open()) {
        stream_.open(path_.string(), std::ios::binary);
        if (!stream_)
            throw std::runtime_error("cannot open file: " + path_.string());
    }

    std::vector<char> buf(block_size_, '\0');
    stream_.read(buf.data(), static_cast<std::streamsize>(block_size_));
    // partial read on last block is fine; rest of buf is already zero

    ++block_index_;
    return compute_hash(buf, algo_);
}

void reader::reset()
{
    block_index_ = 0;
    if (stream_.is_open()) {
        stream_.close();
    }
}

std::size_t reader::block_count() const
{
    if (file_size_ == 0) return 1; // empty file = one all-zero block
    return (file_size_ + block_size_ - 1) / block_size_;
}