#ifndef GESTER_HPP
#define GESTER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cassert>
#include <getopt.h>
#include "digest/digester.hpp"
#include "digest/mod_minimizer.hpp"
#include "digest/window_minimizer.hpp"

enum class MINSCHEME{
    MOD, WINDOW, SYNCMER
};

void parse_default_options(int argc, char* argv[]);
void read_fasta(std::string fname);
void get_minimizers();

#endif