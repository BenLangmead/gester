#include "gester.hpp"
#include <stdexcept>

/*
Notes:
If you are concatenating strings with Syncmers using the Skipover policy
and your sequence has non-ACTG characters, you will get strange results
In general, Syncmer should always be used with writeover 
(in the case of no non-ACTG characters, bad policy handling doesn't matter)
*/
const struct option longopts[] = {
    {"get_inds",  no_argument, 0, 'i'},
    {"get_concat",  no_argument, 0, 'g'},
    {"file",  required_argument, 0, 'f'},
    {"small",  required_argument, 0, 's'},
    {"hash",   required_argument, 0, 'h'},
    {"policy", optional_argument, 0, 'p'},
    {"digestion_scheme", required_argument, 0, 'd'},
    {"data_structure", optional_argument, 0, 'r'},
    {"thread_count", optional_argument, 0, 't'},
    {"large",  optional_argument, 0, 'l'},
    {"mod",  optional_argument, 0, 'm'},
    {"congruence",  optional_argument, 0, 'c'},
    {0, 0, 0, 0},
};

// output flags, whether or not we return the indices, whether or not we concat the minimizers together
bool get_indices = false;
bool get_concat = false;

unsigned small_window;
unsigned large_window;
std::string filename;
std::string seq;
digest::BadCharPolicy policy = digest::BadCharPolicy::WRITEOVER;
digest::MinimizedHashType ht =digest::MinimizedHashType::CANON;
MINSCHEME scheme;
unsigned mod;
unsigned congruence;
DATA_STRUCTURE ds = DATA_STRUCTURE::ADAPTIVE; // TODO: add to user input
unsigned thread_count = 1;

// used to check that the arguments needed for the minimizer scheme have been provided
bool lwind_flag = 0;
uint8_t mod_scheme_flags = 0;

std::vector<uint32_t> vec;

//**** PREBUILT DIGESTERS ****//

//// code to select:
// if (policy == digest::BadCharPolicy::SKIPOVER)
// 	get_prebuilt<digest::BadCharPolicy::SKIPOVER>();
// else
// 	get_prebuilt<digest::BadCharPolicy::WRITEOVER>();

// only supportes 4 <= large <= 32
template <digest::BadCharPolicy P, int large>
digest::Digester<P>* get_prebuilt() {
	if (large < large_window) {
		return get_prebuilt<P,std::min(32,large+1)>();
	}

	if (scheme == MINSCHEME::WINDOW) {
		switch (ds) {
			case DATA_STRUCTURE::NAIVE:
				return new digest::WindowMin<P, digest::ds::Naive<large>>(seq, small_window, large_window, 0, ht);
			case DATA_STRUCTURE::SEGMENT:
				return new digest::WindowMin<P, digest::ds::SegmentTree<large>>(seq, small_window, large_window, 0, ht);
			case DATA_STRUCTURE::NAIVE2:
				return new digest::WindowMin<P, digest::ds::Naive2<large>>(seq, small_window, large_window, 0, ht);
			default:
				assert(0);
		}
	} else {
		switch (ds) {
			case DATA_STRUCTURE::NAIVE:
				return new digest::Syncmer<P, digest::ds::Naive<large>>(seq, small_window, large_window, 0, ht);
			case DATA_STRUCTURE::SEGMENT:
				return new digest::Syncmer<P, digest::ds::SegmentTree<large>>(seq, small_window, large_window, 0, ht);
			case DATA_STRUCTURE::NAIVE2:
				return new digest::Syncmer<P, digest::ds::Naive2<large>>(seq, small_window, large_window, 0, ht);
			default:
				assert(0);
		}
	}

	assert(0);
}

template <digest::BadCharPolicy P>
digest::Digester<P>* get_prebuilt() {
	if (scheme == MINSCHEME::MOD) {
		return new digest::ModMin<P>(seq, small_window, mod, congruence, 0, ht);
	}
	if (ds == DATA_STRUCTURE::ADAPTIVE) {
		if (scheme == MINSCHEME::WINDOW) {
			return new digest::WindowMin<P, digest::ds::Adaptive>(seq, small_window, large_window, 0, ht);
		} else {
			return new digest::Syncmer<P, digest::ds::Adaptive>(seq, small_window, large_window, 0, ht);
		}
	}
	return get_prebuilt<P,4>();
}

int main(int argc, char* argv[]) {
    
    parse_default_options(argc, argv);

    get_minimizers();
    get_output();
    return 0;
}

void parse_default_options(int argc, char* argv[]){
    // Parse command line arguments
    int option = 0, index = 0;
    while ((option = getopt_long(argc, argv, "igf:s:h:p:d:r:t:l:m:c:", longopts, &index)) != -1) {
        switch (option) {
            case 'i':
                get_indices = true;
                break;
            case 'g':
                get_concat = true;
                break;
            case 'f':
                try{
                    filename = std::string(optarg);
                    read_fasta(filename);
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse and open the argument for input file");
                }
                break;
            case 's':
                try{
                    // make sure its bigger or equal to 4
                    if(optarg[0] == '-') throw std::invalid_argument("");
                    small_window = std::stoul(std::string(optarg));
                    if(small_window < 4) throw std::invalid_argument("");
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse the small window value. Note that the small window value must be greater or equal to 4");
                }
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
                } else{
                    throw std::invalid_argument("Received an invalid minimization scheme");
                }
                break;
            case 'r':
                if(strcmp(optarg, "naive") == 0) {
                    ds = DATA_STRUCTURE::NAIVE;
                } else if(strcmp(optarg, "segment") == 0) {
                    ds= DATA_STRUCTURE::SEGMENT;
                } else if(strcmp(optarg, "naive2") == 0) {
                    ds = DATA_STRUCTURE::NAIVE2;
                }
                break;
            case 't':
                try{
                    if(optarg[0] == '-') throw std::invalid_argument("");
                    thread_count = std::stoul(std::string(optarg));
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse the number of threads");
                }
                break;
            case 'l':
                try{
                    if(optarg[0] == '-') throw std::invalid_argument("");
                    large_window = std::stoul(std::string(optarg));
                    lwind_flag = 1;
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse the large window size");
                }
                break;
            case 'm':
                try{
                    if(optarg[0] == '-') throw std::invalid_argument("");
                    mod = std::stoul(std::string(optarg));
                    mod_scheme_flags |= 1;
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse the mod value");
                }
                break;
            case 'c':
                try{
                    if(optarg[0] == '-') throw std::invalid_argument("");
                    congruence = std::stoul(std::string(optarg));
                    mod_scheme_flags |= (1<<1);
                }catch(...){
                    throw std::invalid_argument("Error occured while trying to parse the argument for congruence");
                }
                break;    
            default:
                // probably throw an error here
                std::cerr << "Invalid option at position " << index << std::endl;
        }
    }
}

void read_fasta(std::string fname){
    std::ifstream in(fname, std::ios_base::in);
    if(!in.is_open()){
        throw std::invalid_argument("");
    }
    std::string throw_away;
    getline(in, throw_away);
    std::string temp;
    while(in >> temp){
        seq += temp;
    }
}

void get_minimizers(){
	if (thread_count == 1) {
		if (policy == digest::BadCharPolicy::SKIPOVER){
			auto dig = get_prebuilt<digest::BadCharPolicy::SKIPOVER>();
			dig->roll_minimizer(seq.size(), vec);
		} else {
			auto dig = get_prebuilt<digest::BadCharPolicy::WRITEOVER>();
			dig->roll_minimizer(seq.size(), vec);
		}
		return;
	}

	std::vector<std::vector<uint32_t>> out;
    if (policy == digest::BadCharPolicy::SKIPOVER){
		if (scheme == MINSCHEME::MOD) {
            if(mod_scheme_flags != 3){
                throw std::invalid_argument("Missing arguments for mod-minimizer");
            }
            digest::thread_out::thread_mod<digest::BadCharPolicy::SKIPOVER>(thread_count, out, seq, small_window, mod, congruence, 0, ht);
        }else if(scheme == MINSCHEME::WINDOW){
            if(!lwind_flag){
                throw std::invalid_argument("Missing arguments for window-minimizer");
            }
			digest::thread_out::thread_wind<digest::BadCharPolicy::SKIPOVER,digest::ds::Adaptive>(thread_count, out, seq, small_window, large_window, 0, ht);
        }else{
            if(!lwind_flag){
                throw std::invalid_argument("Missing arguments for syncmer");
            }
            digest::thread_out::thread_sync<digest::BadCharPolicy::SKIPOVER, digest::ds::Adaptive>(thread_count, out, seq, small_window, large_window, 0, ht);
        }
    } else {
        if(scheme == MINSCHEME::MOD){
            if(mod_scheme_flags != 3){
                throw std::invalid_argument("Missing arguments for mod-minimizer");
            }
            digest::thread_out::thread_mod<digest::BadCharPolicy::WRITEOVER>(thread_count, out, seq, small_window, mod, congruence, 0, ht);
        }else if(scheme == MINSCHEME::WINDOW){
            if(!lwind_flag){
                throw std::invalid_argument("Missing arguments for window-minimizer");
            }
			digest::thread_out::thread_wind<digest::BadCharPolicy::WRITEOVER,digest::ds::Adaptive>(thread_count, out, seq, small_window, large_window, 0, ht);
        }else{
            if(!lwind_flag){
                throw std::invalid_argument("Missing arguments for syncmer");
            }
            digest::thread_out::thread_sync<digest::BadCharPolicy::WRITEOVER,digest::ds::Adaptive>(thread_count, out, seq, small_window, large_window, 0, ht);
        }
    }

	for (auto &v : out)	for (auto i : v) {
		if (vec.size() and i < vec.back()) continue;
		vec.emplace_back(i);
	}
}

void get_output(){
    std::ofstream ofs ("minimized.out", std::ofstream::out);
    if(get_indices){
        for(auto a : vec){
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
