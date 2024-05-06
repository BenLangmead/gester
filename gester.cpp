#include "gester.hpp"

// I think I want to make this global
const struct option longopts[] = {
    {"file",  required_argument, 0, 'f'},
    {"small",  required_argument, 0, 's'},
    {"hash",   required_argument, 0, 'h'},
    {"policy", required_argument, 0, 'p'},
    {"digestion_scheme", required_argument, 0, 'd'},
    {"large",  optional_argument, 0, 'l'},
    {"mod",  optional_argument, 0, 'm'},
    {"congruence",  optional_argument, 0, 'c'},
    {0, 0, 0, 0},
};

unsigned small_window;
unsigned large_window;
std::string filename;
std::string seq;
digest::BadCharPolicy policy;
digest::MinimizedHashType ht;
MINSCHEME scheme;
unsigned mod;
unsigned congruence;

bool lwind_flag = 0;
uint8_t mod_scheme_flags = 0;

int main(int argc, char* argv[]) {
    
    parse_default_options(argc, argv);

    std::cout << filename << std::endl;
    std::cout << small_window << std::endl;
    std::cout << (int)policy << " " << (int)ht << " " << (int)scheme << std::endl;

    // do the minimization here, in a function
    if(lwind_flag){
        std::cout << large_window << std::endl;
    }else if(mod_scheme_flags == 3){
        std::cout << mod << " " << congruence << std::endl;
    }else{
        std::cout << "bad" << std::endl;
    }

    return 0;
}

void parse_default_options(int argc, char* argv[]){
    // Parse command line arguments
    int option = 0, index = 0;
    while ((option = getopt_long(argc, argv, "f:s:h:p:d:l:m:c:", longopts, &index)) != -1) {
        switch (option) {
            case 'f':
                filename = std::string(optarg);
                read_fasta(filename);
                // read in FASTA file here
                break;
            case 's':
                small_window = std::stoul(std::string(optarg));
                break;
            case 'h':
                if(strcmp(optarg, "canon") == 0) {
                    ht = digest::MinimizedHashType::CANON;
                } else if(strcmp(optarg, "forward") == 0) {
                    ht = digest::MinimizedHashType::FORWARD;
                } else if(strcmp(optarg, "reverse") == 0) {
                    ht = digest::MinimizedHashType::REVERSE;
                }
                break;
            case 'p':
                if(strcmp(optarg, "skipover") == 0) {
                    policy = digest::BadCharPolicy::SKIPOVER;
                } else if(strcmp(optarg, "writeover") == 0) {
                    policy = digest::BadCharPolicy::WRITEOVER;
                }
                break;
            case 'd':
                if(strcmp(optarg, "mod") == 0) {
                    scheme = MINSCHEME::MOD;
                } else if(strcmp(optarg, "window") == 0) {
                    scheme = MINSCHEME::WINDOW;
                } else if(strcmp(optarg, "syncmer") == 0) {
                    scheme = MINSCHEME::SYNCMER;
                }
                break;
            case 'l':
                large_window = std::stoul(std::string(optarg));
                lwind_flag = 1;
                break;
            case 'm':
                mod = std::stoul(std::string(optarg));
                mod_scheme_flags |= 1;
                break;
            case 'c':
                congruence = std::stoul(std::string(optarg));
                mod_scheme_flags |= (1<<1);
                break;    
            default:
                // probably throw an error here
                std::cerr << "Invalid option" << std::endl;
        }
    }
}

void read_fasta(std::string fname){
    // nvm lol, there's an endline at the end of every line in a fasta file
    // so should use the append_seq function
    std::ifstream in(fname, std::ios_base::in);
    std::cout << in.is_open() << std::endl;
    std::string throw_away;
    getline(in, throw_away);
    std::cout << throw_away << std::endl;
    in >> seq;
    std::cout << seq << std::endl;
}