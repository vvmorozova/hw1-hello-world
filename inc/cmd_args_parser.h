#pragma once

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "utils.h"
#include <string>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class cmd_args_parser {

public:
    cmd_args_parser();
    config parse(int argc, char* argv[]) const;
    std::string helpText() const;
    // thrown when --help is requested; carry the help text
    struct help_req : std::exception {
        explicit help_req(std::string msg) : message(std::move(msg)) {}
        const char* what() const noexcept override { return message.c_str(); }
        std::string message;
    };
private:
    po::options_description desc_;
};