#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  // time that is left for a process to run
  u32 time_left;
  // save the time when the process first ran
  u32 init_time;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

// helper function to compare and sort processes
int compare_times(const void *a, const void *b) {
    const struct process *proc1 = (const struct process *)a;
    const struct process *proc2 = (const struct process *)b;

    if (proc1->arrival_time < proc2->arrival_time) {
      return -1;
    }
    else if (proc1->arrival_time > proc2->arrival_time) {
      return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  // use for process that havent run yet
  const u32 UNINIT_TIME = (u32)-1;

  // base case
  if (quantum_length <= 0 || size == 0) {
    return EINVAL;
  }

  // create the ready queue
  struct process_list r_queue;
  TAILQ_INIT(&r_queue);

  // use quick sort to sort processes
  qsort(data, size, sizeof(struct process), compare_times);

  // fill the list
  for (u32 i = 0; i < size; i++) {
      data[i].init_time = UNINIT_TIME;
      data[i].time_left = data[i].burst_time;  
      TAILQ_INSERT_TAIL(&list, &data[i], pointers);
  }

  // find first arrival time
  struct process *first_proc = TAILQ_FIRST(&list);  
  u32 first_arrival = first_proc->arrival_time;  
  u32 cur_time = first_arrival;

  // adds first processes to ready queue, uses linked list approach to iterate over processes
  struct process *proc = TAILQ_FIRST(&list);
  struct process *temp_proc;
  while (proc != NULL) {
    temp_proc = TAILQ_NEXT(proc, pointers);
    if (proc->arrival_time == first_arrival) {
      TAILQ_REMOVE(&list, proc, pointers);
      TAILQ_INSERT_TAIL(&r_queue, proc, pointers);
    }
    proc = temp_proc;
  }

  // begin processing list and queue
  while (!TAILQ_EMPTY(&list) || !TAILQ_EMPTY(&r_queue)) {
    // queue will be empty if a process has finished and a new one isnt queued yet
    if (TAILQ_EMPTY(&r_queue)) {
      // get that next process ready
      struct process *next_proc = TAILQ_FIRST(&list), *temp_proc;
      if (next_proc) {
        // set the time to arrival time and add to ready queue
          cur_time = next_proc->arrival_time;
          temp_proc = TAILQ_NEXT(next_proc, pointers); 
          TAILQ_REMOVE(&list, next_proc, pointers);
          TAILQ_INSERT_TAIL(&r_queue, next_proc, pointers);
          next_proc = temp_proc;  
      }
    }
    // get the most ready process
    struct process *cur_proc = TAILQ_FIRST(&r_queue);

    // get response time, casting -1 to an unsigned 32 bit int for comparison
    if (cur_proc->init_time == UNINIT_TIME) {
        cur_proc->init_time = cur_time;
        int resp_time = cur_time - cur_proc->arrival_time;
        total_response_time += resp_time;
    }
    // check if current time left is less than quantum time bc we either keep the time left or just use the quantom length
    u32 run_time = (cur_proc->time_left < quantum_length) ? cur_proc->time_left : quantum_length;
    // combine the current time and the runtime to get the time of finishing
    u32 fin_time = cur_time + run_time;

    // queue processes as current process is running, uses linked list approach to iterate over processes
    struct process *next_proc = TAILQ_FIRST(&list);
    struct process *temp_proc;
    while (next_proc != NULL) {
      temp_proc = TAILQ_NEXT(next_proc, pointers);
      if (next_proc->arrival_time > cur_time && 
          next_proc->arrival_time <= fin_time && 
          next_proc->init_time == UNINIT_TIME) {
            TAILQ_REMOVE(&list, next_proc, pointers);
            TAILQ_INSERT_TAIL(&r_queue, next_proc, pointers);
        }
      next_proc = temp_proc;
    }

    // alter clock
    cur_time += run_time;
    cur_proc->time_left -= run_time;
    TAILQ_REMOVE(&r_queue, cur_proc, pointers);

    // halt process, if still needs to run it goes to the end of the queue
    if (cur_proc->time_left > 0) {
      TAILQ_INSERT_TAIL(&r_queue, cur_proc, pointers);
    } 
    else {
        // calc waiting time for complete process
        total_waiting_time += cur_time - cur_proc->arrival_time - cur_proc->burst_time;
    }
  }
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
