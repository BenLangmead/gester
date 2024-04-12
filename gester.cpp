#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include "digest/digester.hpp"
#include "digest/mod_minimizer.hpp"
#include "digest/window_minimizer.hpp"

int main(int argc, char* argv[]) {
    int l = 0, s = 0;
    digest::BadCharPolicy policy{digest::BadCharPolicy::WRITEOVER};
    digest::MinimizedHashType ht{digest::MinimizedHashType::CANON};

    const struct option longopts[] = {
        {"small",  required_argument, 0, 's'},
        {"large",  required_argument, 0, 'l'},
        {"policy", required_argument, 0, 'p'},
        {"type",   required_argument, 0, 't'},
        {0, 0, 0, 0},
    };

    // Parse command line arguments
    int option = 0, index = 0;
    while ((option = getopt_long(argc, argv, "l:s:p:t:", longopts, &index)) != -1) {
        switch (option) {
            case 'l':
                l = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'p':
                if(strcmp(optarg, "skip") == 0) {
                    policy = digest::BadCharPolicy::SKIPOVER;
                } else if(strcmp(optarg, "overwrite") == 0) {
                    policy = digest::BadCharPolicy::WRITEOVER;
                }
                break;
            case 't':
                if(strcmp(optarg, "canon") == 0) {
                    ht = digest::MinimizedHashType::CANON;
                } else if(strcmp(optarg, "forward") == 0) {
                    ht = digest::MinimizedHashType::FORWARD;
                } else if(strcmp(optarg, "reverse") == 0) {
                    ht = digest::MinimizedHashType::REVERSE;
                }
                break;
            default:
                std::cerr << "Invalid option" << std::endl;
                return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "Positional argument 1 should specify method" << std::endl;
        return 1;
    }

    std::string method = argv[optind];
    std::string dna{"ACGATCGATCGTATCGAGTCTCGAG"};
    const size_t len = dna.length();
	std::vector<uint32_t> vec;
    if(method == "adaptive") {
        digest::WindowMin<digest::BadCharPolicy::WRITEOVER, digest::ds::Adaptive> digester(dna, s, l);
    	digester.roll_minimizer(len, vec);
    } else if(method == "min") {
        digest::WindowMin<digest::BadCharPolicy::WRITEOVER, digest::ds::Adaptive> digester(dna, s, l);
    	digester.roll_minimizer(len, vec);
    } else if(method == "mod") {
        digest::ModMin<digest::BadCharPolicy::WRITEOVER> digester(dna, s, l);
    	digester.roll_minimizer(len, vec);
    } else if(method == "sync") {
        // Perform action for "sync" choice
    } else {
        std::cerr << "Invalid method argument \"" << method << "\"" << std::endl;
        return 1;
    }
    return 0;
}
