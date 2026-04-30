#include "comparator.h"

#include <stdexcept>

bool comparator::are_equal(reader& a, reader& b)
{
    // files of different sizes can never be equal
    if (a.total_blocks() != b.total_blocks()) return false;

    a.reset();
    b.reset();

    while (!a.exhausted() && !b.exhausted()) {
        block_hash ha = a.next_block();
        block_hash hb = b.next_block();
        if (ha != hb) return false;
    }

    // both must be exhausted together (guaranteed by totalBlocks check above)
    return a.exhausted() && b.exhausted();
}

std::vector<std::vector<fs::path>> comparator::find_duplicates(std::vector<std::shared_ptr<reader>> readers)
{
    // track which readers have already been assigned to a group
    std::vector<bool> used(readers.size(), false);
    std::vector<std::vector<fs::path>> groups;

    for (std::size_t i = 0; i < readers.size(); ++i) {
        if (used[i]) continue;

        std::vector<fs::path> group;
        group.push_back(readers[i]->path());

        for (std::size_t j = i + 1; j < readers.size(); ++j) {
            if (used[j]) continue;

            if (are_equal(*readers[i], *readers[j])) {
                group.push_back(readers[j]->path());
                used[j] = true;
            }
        }

        if (group.size() > 1) {
            groups.push_back(std::move(group));
            used[i] = true;
        }
    }

    return groups;
}