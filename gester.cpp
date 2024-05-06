#include "gester.hpp"

// I think I want to make this global
const struct option longopts[] = {
    {"file",  required_argument, 0, 'f'},
    {"small",  required_argument, 0, 's'},
    {"hash",   required_argument, 0, 'h'},
    {"policy", required_argument, 0, 'p'},
    {"digestion_scheme", required_argument, 0, 'd'},
    {"large",  required_argument, 0, 'l'},
    {"mod",  required_argument, 0, 'm'},
    {"congruence",  required_argument, 0, 'c'},
    {0, 0, 0, 0},
};

unsigned small_window;
unsigned large_window;
std::string filename;
digest::BadCharPolicy policy;
digest::MinimizedHashType ht;
MINSCHEME scheme;

int main(int argc, char* argv[]) {
    
    parse_default_options(argc, argv);

    std::cout << filename << std::endl;
    std::cout << small_window << std::endl;
    std::cout << (int)policy << " " << (int)ht << " " << (int)scheme << std::endl;


    return 0;
}

int parse_default_options(int argc, char* argv[]){
    // Parse command line arguments
    int option = 0, index = 0;
    while ((option = getopt_long(argc, argv, "f:s:h:p:d:", longopts, &index)) != -1) {
        switch (option) {
            case 'f':
                std::cout << 1 << std::endl;
                filename = std::string(optarg);
                break;
            case 's':
                std::cout << 2 << std::endl;
                small_window = std::stoul(std::string(optarg));
                break;
            case 'h':
                std::cout << 3 << std::endl;
                if(strcmp(optarg, "canon") == 0) {
                    ht = digest::MinimizedHashType::CANON;
                } else if(strcmp(optarg, "forward") == 0) {
                    ht = digest::MinimizedHashType::FORWARD;
                } else if(strcmp(optarg, "reverse") == 0) {
                    ht = digest::MinimizedHashType::REVERSE;
                }
                break;
            case 'p':
                std::cout << 4 << std::endl;
                if(strcmp(optarg, "skipover") == 0) {
                    policy = digest::BadCharPolicy::SKIPOVER;
                } else if(strcmp(optarg, "writeover") == 0) {
                    policy = digest::BadCharPolicy::WRITEOVER;
                }
                break;
            case 'd':
                std::cout << 5 << std::endl;
                if(strcmp(optarg, "mod") == 0) {
                    scheme = MINSCHEME::MOD;
                } else if(strcmp(optarg, "window") == 0) {
                    scheme = MINSCHEME::WINDOW;
                } else if(strcmp(optarg, "syncmer") == 0) {
                    scheme = MINSCHEME::SYNCMER;
                }
                break;
            default:
                std::cerr << "Invalid option" << std::endl;
                return 1;
        }
    }
    return 0;
}