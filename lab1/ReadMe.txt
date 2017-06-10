Instruction:
load gcc 6.2 use ”module load gcc-6.2.0" on CIMS
There are 3 versions of output. 
a. A concise version with only completion time as required by the assignment.
b. A verbose version with all the runtime information.
c. A test version with test result.

Please be aware that the verbose one is much slower as it takes time to output 
information.

The running time for concise version is much shorter before adding code for testing.
If a short running time is needed, please delete all the code for the test part. 


concise version:
1. cd to lab1 on terminal
2. type "make" to call Makefile
3. type “./lab1 -n <N>“ 
4. check output.txt

verbose version:
1. cd to lab1 on terminal
2. type "make" to call Makefile
3. type “./lab1 -n <N> -v“ 
4. check output.txt

test version:

1. cd to lab1 on terminal
2. type "make" to call Makefile
3. type “./lab1 -n <N> -t“ 
4. check output.txt

5. type "make clean" to clean all *.o and output

or 
type “./lab1 -n <N> -v -t“  for all the information needed.

