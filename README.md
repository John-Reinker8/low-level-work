This repo contains contains 4 low-level projects written in C:
- Piping Project:
  - A program that can mimic the pipe operator in Linux when the program is fed >1 program arguments.
- Round Robin Project:
  - A program that implements a Round Robin Scheduling algorithm for a given workload and quantum length (these are given as command-line inputs, the first being a file path and the second being an integer).
  - The algorithm calculates the total_waiting_time and total_response_time for the processes and outputs the averages for each of them.
- Locking Project:
  - This project introduces the usage of mutex locks to an implementation of a hash table in order to make the implementation thread-safe. There are two versions: v1 uses one mutex lock for thread-safe access to the entire hash table. v2 is far more efficient without sacrificing accuracy; it uses multiple mutex locks, one for accessing each entry/bucket in the hash table.
- File System Project:
  - This project implements a 1 MiB ext2 file system, which has 2 directories (root and lost+found), 1 regular file (hello-world), and 1 symbolic link (hello -> hello_world)
