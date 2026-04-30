#pragma once

#include "reader.h"
#include <vector>
#include <memory>

// compares files, block by block.
// reads from disk only when needed: both files read together,
// one block at time, stopping as soon as a mismatch found.
// if all block hashes match files considered identical.
class comparator {
public:
    // returns true if the two files have identical content.
    // uses the readers' current state - reset() them before calling if needed.
    // disk i/o happens here for the first time.
    static bool are_equal(reader& a, reader& b);

    // groups files into duplicate sets.
    // returns a list of groups; each group contains paths of identical files.
    // single-file groups (no duplicate found) are omitted.
    // i/o is minimized: the first file in each comparison pair is read only
    // as far as needed to find a mismatch or confirm equality.
    static std::vector<std::vector<fs::path>> find_duplicates(std::vector<std::shared_ptr<reader>> readers);
};