#include "cmd_args_parser.h"

#include <vector>
#include <stdexcept>

cmd_args_parser::cmd_args_parser()
    : desc_("Duplicate file finder options")
{
    // define all supported options
    desc_.add_options()
        ("help,h", "show help message")

        ("scan-dir,d", po::value<std::vector<std::string>>()->multitoken()->composing(),
            "directories to scan (repeatable)")

        ("exclude-dir,e", po::value<std::vector<std::string>>()->multitoken()->composing(),
            "directories to exclude (repeatable)")

        ("depth,l", po::value<int>()->default_value(0),
            "scan depth: 0 = only top-level directory")

        ("min-size,m", po::value<std::uintmax_t>()->default_value(2),
            "minimum file size in bytes (default 2, i.e. > 1)")

        ("mask,f", po::value<std::vector<std::string>>()->multitoken()->composing(),
            "filename masks to include, case-insensitive (e.g. *.txt)")

        ("block-size,s", po::value<std::size_t>()->default_value(4096),
            "block size S for file reading in bytes")

        ("hash,a", po::value<std::string>()->default_value("md5"),
            "hash algorithm: crc32 or md5");
}

// parse argv and return validated config; throws on error
config cmd_args_parser::parse(int argc, char* argv[]) const
{
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc_).run(), vm);
        po::notify(vm);
    } catch (const po::error& ex) {
        throw std::invalid_argument(std::string("argument error: ") + ex.what());
    }

    if (vm.count("help")) {
        throw help_req(helpText());
    }

    config cfg;

    // resolve scan directories
    if (vm.count("scan-dir")) {
        for (const auto& s : vm["scan-dir"].as<std::vector<std::string>>()) {
            fs::path p(s);
            if (!fs::is_directory(p)) {
                throw std::invalid_argument("not a directory: " + s);
            }
            cfg.scan_dirs.push_back(fs::canonical(p));
        }
    }
    if (cfg.scan_dirs.empty()) {
        throw std::invalid_argument("at least one --scan-dir is required");
    }

    // resolve exclude directories (non-existent ones are silently ignored)
    if (vm.count("exclude-dir")) {
        for (const auto& s : vm["exclude-dir"].as<std::vector<std::string>>()) {
            fs::path p(s);
            if (fs::is_directory(p)) {
                cfg.exclude_dirs.push_back(fs::canonical(p));
            }
        }
    }

    // scan depth
    cfg.scan_depth = vm["depth"].as<int>();
    if (cfg.scan_depth < 0) {
        throw std::invalid_argument("--depth must be >= 0");
    }

    // minimum file size
    cfg.min_size = vm["min-size"].as<std::uintmax_t>();
    if (cfg.min_size < 2) {
        throw std::invalid_argument("--min-size must be at least 2 (files > 1 byte)");
    }

    // filename masks
    if (vm.count("mask")) {
        cfg.masks = vm["mask"].as<std::vector<std::string>>();
    }

    // block size
    cfg.block_size = vm["block-size"].as<std::size_t>();
    if (cfg.block_size == 0) {
        throw std::invalid_argument("--block-size must be > 0");
    }

    // hash algorithm
    const std::string algo = vm["hash"].as<std::string>();
    if (algo == "crc32") {
        cfg.hash_algo = hash_func::crc32;
    } else if (algo == "md5") {
        cfg.hash_algo = hash_func::md5;
    } else {
        throw std::invalid_argument("unknown hash algorithm: " + algo +
                                    " (use crc32 or md5)");
    }

    return cfg;
}

// returns formatted help text
std::string cmd_args_parser::helpText() const
{
    std::ostringstream oss;
    oss << desc_;
    return oss.str();
}
