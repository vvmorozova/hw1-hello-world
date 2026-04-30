#include "storage.h"

#include <algorithm>
#include <cctype>

#include <boost/regex.hpp>

#include "cmd_args_parser.h"
#include "reader.h"
#include "storage.h"

namespace fs = boost::filesystem;

std::vector<std::shared_ptr<reader>> storage::collect() const
{
    std::vector<std::shared_ptr<reader>> result;

    for (const auto& root : cfg_.scan_dirs) {
        scan_dir(root, root, 0, result);
    }

    return result;
}

void storage::scan_dir(const fs::path& dir, const fs::path& root, int current_depth,
                std::vector<std::shared_ptr<reader>>& result) const
{
    if (!fs::is_directory(dir) || is_excluded(dir)) {
        return;
    }      

    for (const auto& entry : fs::directory_iterator(dir)) {
        const fs::path& p = entry.path();

        if (fs::is_directory(p)) {
            // recurse only if depth limit allows
            if (cfg_.scan_depth == 0) continue;           // no recursion
            if (current_depth + 1 > cfg_.scan_depth) continue;
            scan_dir(p, root, current_depth + 1, result);
            continue;
        }

        if (!fs::is_regular_file(p)) {
            continue;
        }

        // size filter (> 1 byte enforced via min_size >= 2)
        if (fs::file_size(p) < cfg_.min_size) {
            continue;
        }

        // mask filter
        if (!cfg_.masks.empty() && !matches_any_mask(p.filename().string())) {
            continue;
        }

        result.push_back(
            std::make_shared<reader>(p, cfg_.block_size, cfg_.hash_algo));
    }
}

bool storage::is_excluded(const fs::path& p) const
{
    for (const auto& ex : cfg_.exclude_dirs) {
        // canonical paths, so prefix comparison is safe
        const std::string ps  = fs::canonical(p).string();
        const std::string exs = ex.string();
        if (ps == exs || ps.substr(0, exs.size() + 1) == exs + "/") {
            return true;
        }
    }
    return false;
}

bool storage::matches_any_mask(const std::string& filename) const
{
    for (const auto& mask : cfg_.masks) {
        if (glob_match(filename, mask)) {
            return true;
        }
    }
    return false;
}

bool storage::glob_match(const std::string& name, const std::string& pattern)
{
    std::string re;
    re.reserve(pattern.size() * 2 + 2);
    re += '^';
    for (char c : pattern) {
        switch (c) {
            case '*': re += ".*";  break;
            case '?': re += '.';   break;
            case '.': re += "\\."; break;
            case '+': re += "\\+"; break;
            case '^': re += "\\^"; break;
            case '$': re += "\\$"; break;
            case '(': re += "\\("; break;
            case ')': re += "\\)"; break;
            case '[': re += "\\["; break;
            case ']': re += "\\]"; break;
            case '{': re += "\\{"; break;
            case '}': re += "\\}"; break;
            case '|': re += "\\|"; break;
            case '\\': re += "\\\\"; break;
            default:  re += c;    break;
        }
    }
    re += '$';

    // case-insensitive match
    boost::regex rx(re, boost::regex::icase);
    return boost::regex_match(name, rx);
}