Instruction:
For server side : 
load gcc 6.2 use ”module load gcc-6.2.0" on CIMS
1. cd to lab4Both on terminal
2. type "make" to call Makefile
3. type “./lab2 -n <N>“ 

For client side : 
1. cd to lab4_workload.
2. use code:
	httperf --wsesslog=1,0,zipfian.workload --server=192.168.1.125 --port=4060

a. Please be sure to change inet address, use the address your server is running on.

b. Since the server and thread pool will run and wait for request, 
   so the program won’t stop automatically, type ctrl + c to quit server.

c. Please check Result for test results.
   I used 4 threads as my computer has 4 cores.

d. Please check pdf document of lab4 for conclusions from this lab.


This lab has two versions: on disk version and on disk + cache version.
