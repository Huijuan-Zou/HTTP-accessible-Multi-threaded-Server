Instruction:
For server side : 
load gcc 6.2 use ”module load gcc-6.2.0" on CIMS
1. cd to lab3 on terminal
2. type "make" to call Makefile
3. type “./lab2 -n <N>“ 

For client side : 
1. cd to lab2_workload.
2. use code:
	httperf --wsesslog=940,0,sample1.workload --server=192.168.1.125 --port=4060

a. Please be sure to change inet address, use the address your server is running on.

b. Since the server and thread pool will run and wait for request, 
   so the program won’t stop automatically, type ctrl + c to quit server.

c. Please check Results_Client, Results_Server for testing results.
   I outputted both client and server side statistics using 1 to 11 threads.

d. Please check pdf document of Lab3Ans for conclusions from this lab.

e. The Lab3_singleGlobalLock is a version using single global lock instead of multiple locks. In practice I used two locks because unique lock cannot be replaced by exclusive mutex lock.