## UID: 205698257

## Pipe Up

pipe.c – A program that can mimic the pipe operator in Linux when the program is fed >1 program arguments.

## Building

Upload a directory file containing both "pipe.c" and "Makefile" to a virtual machine in order to simulate a Linux environment.
Then, run the "make" command after navigating inside the uploaded directory. This will create the "pipe.o" and executable "pipe" files.

## Running

To run, use the following format: ./pipe arg1 (arg2) (argn)
Parentheses indicate optional arguments, and "argn" represents the fact that you can add as many arguments you want.
Please note that arguments need to be valid programs for the call to work.

Example: ./pipe ls sort uniq wc
This operation is expected to print out the number of unique files in the current directory, the number of words (where each filename is treated as a word), and the total number of characters including spaces and newlines.

On the virtual machine, this output 7 7 63 for me.
Then, one can verify the validity of this by running the standard version: ls | sort | uniq | wc
And this also output 7 7 63.

## Cleaning up

The following can be run to clean up the binary files (such as pipe.o and the executable pipe): make clean
This displays the following in the terminal: rm -f pipe.o pipe
So, this is basically a shorthand way of running rm on our target files.
