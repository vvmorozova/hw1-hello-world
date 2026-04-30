#include <iostream>
#include <vector>
#include <memory>
#include "cmd_args_parser.h"
#include "storage.h"
#include "comparator.h"

int main(int argc, char** argv) {
    cmd_args_parser parser;
    try {
        config cfg = parser.parse(argc, argv);
        
        // create storage and collect readers
        storage st(cfg);
        auto readers = st.collect();
        
        // find duplicates
        auto duplicates = comparator::find_duplicates(readers);
        
        // print res
        if (duplicates.empty()) {
            std::cout << "No duplicates found.\n";
        } else {
            for (const auto& group : duplicates) {
                std::cout << "Duplicate group:\n";
                for (const auto& path : group) {
                    std::cout << "  " << path.string() << "\n";
                }
                std::cout << "\n";
            }
        }
        
    } catch (const cmd_args_parser::help_req& h) {
        std::cout << h.what() << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}