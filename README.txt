#READ.ME

Project: Virtual Address Manager
by Tyler Edwards 
Added November 2023

Description: Reads logical addresses from file and translates them to into physical addresses. 

Language: C 

Reason: For CPSC 380 (Operating Systems)

Successes: By implementing them in C, I gained a better understanding of how address management functions

Setbacks: Figuring out how to account for case situations liek page faults provided difficult at times.

Learned: how to use address management functions


/*
Commands:
gcc vmmgr.c -o vmmgr
./vmmgr

Logical Address: 16916, Physical Address: -236, Value: 0 
...
*/

- Utilizes in-class provided code for BACKING_STORE.bin and addresses.txt