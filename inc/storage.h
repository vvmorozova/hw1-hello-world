#pragma once

#include <string>
#include <vector>
#include <memory>

#include "reader.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

// scans directories according to config and builds a list of FileReaders.
// applies depth limit, exclusion list, size filter and filename mask filter.
class storage {
public:
    explicit storage(const config& cfg) : cfg_(cfg) {}

    // performs the scan and returns one FileReader per accepted file.
    // no file content is read here - readers are lazy.
    std::vector<std::shared_ptr<reader>> collect() const;

private:
    // recursively scans dir; baseDepth is the depth of the scan root
    void scan_dir(const fs::path& dir, const fs::path& root, int current_depth,
                 std::vector<std::shared_ptr<reader>>& result) const;

    // returns true if path is inside any excluded directory
    bool is_excluded(const fs::path& p) const;

    // case-insensitive glob match against all configured masks
    bool matches_any_mask(const std::string& filename) const;

    // converts a glob pattern (*, ?) to a boost::regex and matches
    static bool glob_match(const std::string& name, const std::string& pattern);

    const config& cfg_;
};