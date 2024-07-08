Beginning of an attempt to make a command-line tool that works with the digest library and allows for the digestion of input FASTA files.  The goal is to take a FASTA file as input and output a new FASTA file where the un-selected letters are printed lowercase and the selected letters (i.e. those overlapping a selected minimizer, modimizer or syncmer) are printed uppercase.


## Example Usage:
Assuming the executable is in the build folder and we want to use a window minimizer with the segment tree as the data structure:
./gest -i -g -f ../fasta/small.fasta -s 4 -h canon -p writeover -d window -r segment -t 4 -l 8