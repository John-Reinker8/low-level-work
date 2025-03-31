# You Spin Me Round Robin
A program written in C that implements a Round Robin Scheduling algorithm for a given workload and 
quantum length (these are given as command-line inputs, the first being a file path and the second 
being an integer).

The algorithm calculates the total_waiting_time and total_response_time for the processes and outputs
the averages for each of them.

## Building

```shell
TODO: Have a directory that holds the rr.c file, the Makefile, and the file path argument file
be in a linux environment and run the 'make' command.
```

## Running

cmd for running TODO: ./rr {file path} {quantum length}
```shell
TODO: Make sure to replace {file path} and {quantum length} with the proper values.
```

results
```shell
TODO: The program will run and output in the following format:
Average waiting time: {value}
Average response time: {value}

For example, running the following with the given file processes.txt:
./rr processes.txt 3

This will output:
Average waiting time: 7.00
Average response time: 2.75

```

## Cleaning up

```shell
TODO: To clean up the binary files and allow for a new built (in case you made some edits to the .c file), run 'make clean'
```
