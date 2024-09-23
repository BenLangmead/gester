A command-line tool that uses the [Digest library](https://github.com/VeryAmazed/digest) and allows for the digestion of an input FASTA file and provides either the indices of minimzers and/or a resulting sequence which is the minimizers concatenated together in an output file called "minimized.out".

## Setup:

## Arguments:
1. -i optional argument, include to indicate you want the index of every minimizer outputed.
2. -g optional argument, include to indicate you want an output sequence consisting of all the minimizers concatenated.
3. -f {string} path to fasta file containing the input sequence you want to minimize.
4. -s {unsigned int} size of the small window a.k.a.the k-mer length.
5. -h {string} type of hash to be uses, "canon" for canonical, "forward" for forward hash, "reverse" for reverse hash. Canonical is chosen by default. 
6. -p {string} non-ACTG character handling policy, "skipover" to ignore any k-mer with a non-ACTG character in it, "writeover" to replace any non-ACTG character with an A. Writeover is chosen by default.
7. -d {string} digestion policy to use, "mod" for mod-minimizers, "window" for window minimizers, and "syncmer" for syncmers.
8. -r {string} the range minimum query data structure to use in the case of window minimizers or syncmers. More details can be found at the Digest Library documentation. "naive" for naive, "segment" for segment tree, "naive2" for naive2. Adaptive is chosen by default.
9. -t {unsigned int} number of threads to use.
10. -l {unsigned int} large window size if using window minimizers or syncmers. Can be left out if not using them.
11. -m {unsigned int} number to mod by if mod-minimizer is chosen. Can be left out if not using them.
12. -c {unsigned int} number the modulus should be congruent to if using mod-minimizers. Can be left out if not using them.

### Note:

If you are concatenating strings with Syncmers using the Skipover policy and your sequence has non-ACTG characters, you will get strange/incorrect results. This is because in such a case one cannot be certain of the size of a Syncmer since the entire large window is the syncmer but the large window van be of variable size if there are k-mers with non-ACTG characters in them.

## Example Usage:
Assuming the executable is in the build folder and we want to use a window minimizer with the segment tree as the data structure:
./gest -i -g -f ../fasta/small.fasta -s 4 -h canon -p writeover -d window -r segment -t 4 -l 8