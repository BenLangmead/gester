#include "gester.hpp"

/*
TODO:
More Robust error handling
Add an option to allow user to pick how many threads they want to run it on
Allow the user to pick the data structure used, globally instantiate an array
with a bunch of them with pre-determined window sizes, you can use defines to get this done I think
look at the examples in bench_ds.cpp

Notes:
If you do are asking for the concatenation of strings with Syncmers using the Skipover policy
and your sequence has non-ACTG characters, you will get strange results
In general, Syncmer should always be used with writeover 
(in the case of no non-ACTG characters, bad policy handling doesn't matter)
*/

// I think I want to make this global
const struct option longopts[] = {
    {"get_inds",  no_argument, 0, 'i'},
    {"get_concat",  no_argument, 0, 'g'},
    {"file",  required_argument, 0, 'f'},
    {"small",  required_argument, 0, 's'},
    {"hash",   required_argument, 0, 'h'},
    {"policy", optional_argument, 0, 'p'},
    {"digestion_scheme", required_argument, 0, 'd'},
    {"large",  optional_argument, 0, 'l'},
    {"mod",  optional_argument, 0, 'm'},
    {"congruence",  optional_argument, 0, 'c'},
    {0, 0, 0, 0},
};

// output flags
bool get_indices = false;
bool get_concat = false;

unsigned small_window;
unsigned large_window;
std::string filename;
std::string seq;
digest::BadCharPolicy policy = digest::BadCharPolicy::WRITEOVER;
digest::MinimizedHashType ht;
MINSCHEME scheme;
unsigned mod;
unsigned congruence;

bool lwind_flag = 0;
uint8_t mod_scheme_flags = 0;

std::vector<uint32_t> vec;

int main(int argc, char* argv[]) {
    
    parse_default_options(argc, argv);

    get_minimizers();
    get_output();
    return 0;
}

void parse_default_options(int argc, char* argv[]){
    // Parse command line arguments
    int option = 0, index = 0;
    while ((option = getopt_long(argc, argv, "igf:s:h:p:d:l:m:c:", longopts, &index)) != -1) {
        switch (option) {
            case 'i':
                get_indices = true;
                break;
            case 'g':
                get_concat = true;
                break;
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
                }else{
                    // throw an error
                }
                break;
            case 'd':
                if(strcmp(optarg, "mod") == 0) {
                    scheme = MINSCHEME::MOD;
                } else if(strcmp(optarg, "window") == 0) {
                    scheme = MINSCHEME::WINDOW;
                } else if(strcmp(optarg, "syncmer") == 0) {
                    scheme = MINSCHEME::SYNCMER;
                } else{
                    // throw error
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
    std::ifstream in(fname, std::ios_base::in);
    std::string throw_away;
    getline(in, throw_away);
    std::string temp;
    while(in >> temp){
        seq += temp;
    }
    //std::cout << seq << std::endl;
}

void get_minimizers(){
    if(policy == digest::BadCharPolicy::SKIPOVER){
        if(scheme == MINSCHEME::MOD){
            assert(mod_scheme_flags == 3);
            digest::ModMin<digest::BadCharPolicy::SKIPOVER> dig(seq, small_window, mod, congruence, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }else if(scheme == MINSCHEME::WINDOW){
            assert(lwind_flag);
            digest::WindowMin<digest::BadCharPolicy::SKIPOVER, digest::ds::Adaptive> dig(seq, small_window, large_window, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }else{
            assert(lwind_flag);
            digest::Syncmer<digest::BadCharPolicy::SKIPOVER, digest::ds::Adaptive> dig(seq, small_window, large_window, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }
    }else{
        if(scheme == MINSCHEME::MOD){
            assert(mod_scheme_flags == 3);
            digest::ModMin<digest::BadCharPolicy::WRITEOVER> dig(seq, small_window, mod, congruence, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }else if(scheme == MINSCHEME::WINDOW){
            assert(lwind_flag);
            digest::WindowMin<digest::BadCharPolicy::WRITEOVER, digest::ds::Adaptive> dig(seq, small_window, large_window, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }else{
            assert(lwind_flag);
            digest::Syncmer<digest::BadCharPolicy::WRITEOVER, digest::ds::Adaptive> dig(seq, small_window, large_window, 0, ht);
            dig.roll_minimizer(seq.size(), vec);
        }
    }
}

void get_output(){
    std::ofstream ofs ("minimized.out", std::ofstream::out);
    if(get_indices){
        for(auto a : vec){
            // I think we put this in a file
            ofs << a << " ";
        }
        ofs << "\n";
    }
    if(get_concat){
        std::string out_string;
        if(scheme == MINSCHEME::MOD || scheme == MINSCHEME::WINDOW){
            for(auto a : vec){
                out_string += seq.substr(a, small_window);
            }
        }else{
            assert(policy == digest::BadCharPolicy::WRITEOVER);
            for(auto a : vec){
                out_string += seq.substr(a, large_window + small_window-1);
            }
        }
        ofs << out_string << "\n";
    }
}