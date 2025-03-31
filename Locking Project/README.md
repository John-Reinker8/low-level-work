# Hash Hash Hash
This project introduces the usage of mutex locks to an implementation of a hash table in order to make the implementation thread-safe. There are two versions: v1 uses one mutex lock for thread-safe access to the entire hash table. v2 is far more efficient without sacrificing accuracy; it uses multiple mutex locks, one for accessing each entry/bucket in the hash table.

## Building
To build, cd into the project directory and run the "make" command.
```shell
make
```

## Running
The built executable "hash-table-tester" has two possible command line arguments: -t is used to change the number of threads, with the default being 4; -s is used to change the number of hash table entries that are added per thread. Below is an example run with 6 threads and 100000 hash table entries added per thread:
```shell
./hash-table-tester -t 6 -s 100000
Generation: 124,199 usec
Hash table base: 4,718,563 usec
  - 0 missing
Hash table v1: 6,911,844 usec
  - 0 missing
Hash table v2: 881,491 usec
  - 0 missing
```
The output gives organized results based on the v1 and v2 runtime performances and missing values if they occur. A base implementation's results is also clocked for comparison.



## First Implementation
In the `hash_table_v1_add_entry` function, I added a sole mutex lock member titled "lock" to the "hash_table_v1" struct. This ensures that access to the hash table is mutually exclusive across any number of threads (only one thread can access the entire table at any given time). The mutex lock is initialized in the "hash_table_v1_create()" function, which basically means the lock is created upon the creation of the hash table itself. In the "hash_table_v1_add_entry()" function, the lock is locked at the beginning of the function's work routine. Thus, only one thread is permitted to complete the addition of an entry to the hash table at a time. Once the work is performed, the lock is unlocked before returning from the function, allowing another thread to potentially hold it and add an entry to the table. When it comes time for the hash table to be destroyed in "hash_table_v1_destroy()", the lock is destroyed before the table is freed.

### Performance
We can use the output of the example above to compare the implementations.
```shell
./hash-table-tester -t 6 -s 100000
Generation: 124,199 usec
Hash table base: 4,718,563 usec
  - 0 missing
Hash table v1: 6,911,844 usec
  - 0 missing
Hash table v2: 881,491 usec
  - 0 missing
```
Version 1 is significantly slower than the base version. This is the case in all test runs I performed. No runs had any missing values, indicating program correctness for v1.

The gap in performance speed is due to v1 incurring much overhead in the creation and handling of the locks. The Base Implementation is not set up to work concurrently, therefore it does not have such overheads. In v1, every time the table is accessed, mutex operations are performed which slow down the program despite ensuring thread safety. It's a trade-off!

## Second Implementation
In the `hash_table_v2_add_entry` function, I increased the concurrency power of the v2 program by using multiple, finer-grained mutex locks. Here, each entry/bucket in the hash table has its own mutex lock, allowing threads to work on different parts of the table at the same time. This is much more efficient than v1, where a thread would have to wait to access the table even if it was going to work in an entirely different bucket. Now, threads are only going to be blocked if their targeted entry is already being edited.

Instead of declaring a mutex lock as a member of the "hash_table_v2" struct, the mutex lock "lock" was added as a member to the "hash_table_entry" struct, to allow each entry to have its own lock. Each lock is now initialized in a loop within the "hash_table_v2_create()" function, initializing one lock for each entry. In the "hash_table_v2_add_entry()" function, the lock for a particular entry is locked once the hash has been calculated and that entry has been targeted. The entry is then edited, and the lock is unlocked before the function returns. 
When the table is destroyed using "hash_table_v2_destroy()", a loop goes through each entry, locking and then unlocking once the entry has been properly cleaned up. Then, that particular lock gets destroyed. Finally, the table is freed.

### Performance
Again, we can use the output of the example above to compare the implementations.
```shell
./hash-table-tester -t 6 -s 100000
Generation: 124,199 usec
Hash table base: 4,718,563 usec
  - 0 missing
Hash table v1: 6,911,844 usec
  - 0 missing
Hash table v2: 881,491 usec
  - 0 missing
```

In all the test runs, v2's performance time was significantly faster than both v1 and the Base implementation. No runs had any missing values, indicating program correctness for v2.

In most of the runs, v2's time was ≤ base_time / (num_cores - 1), however in some runs it was slightly more. v2's time was never > base_time / (num_cores - 2). These differences may be attributable to server usage or system power. The reason for the increase in speed is because v2 allows threads to work concurrently WITHIN the hash table. So, arbitrary protection is avoided (for example, why would we need to lock the table if the incoming thread is not working in the same entry as the current thread?). There is still overhead due to the mutex operations, but it is heavily outweighed by the efficiency introduced by multiple workers within the table.

## Cleaning up
To clean up the binary files, run "make clean"
```shell
make clean
```