#include "cond.h"
#include "lock.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "test3.h"
#include "time.h"
#define MAX_RANGE 30000
#define MOD 10007
#define NUM_CPUS 2

struct TestMultiCoreArg
{
  int print_location;
  int from;
  int to;
  int *result;
};
void add_task1();
void add_task2();
void add_task3();
void test_multicore(void)
{
  sys_move_cursor(0, 0);
  printf("start test multi-core performance\n\r");
  int single_core_result = 0;
  struct task_info task_add1 = {"add1", &add_task1, USER_PROCESS};
  struct task_info task_add2 = {"add2", &add_task2, USER_PROCESS};
  struct task_info task_add3 = {"add3", &add_task3, USER_PROCESS};
  // single core performance
  int i;
  uint64_t singleCoreBegin = get_ticks();
  pid_t single_pid = sys_spawn(&task_add1);
  sys_waitpid(single_pid);
  uint64_t singleCoreEnd = get_ticks();
  sys_move_cursor(0, 3);
  printf("single_pid %d,single core: %d ticks     \n\r", single_pid, singleCoreEnd - singleCoreBegin);

  struct TestMultiCoreArg multiCoreArgs[NUM_CPUS];
  pid_t pids[NUM_CPUS];

  uint64_t multiCoreBegin = get_ticks();
  pids[0] = sys_spawn(&task_add2);

  pids[1] = sys_spawn(&task_add3);
  for (i = 0; i < NUM_CPUS; ++i)
  {
    sys_waitpid(pids[i]);
  }

  uint64_t multiCoreEnd = get_ticks();
  sys_move_cursor(0, 9);
  printf("pids[0] %d,pids[1] %d,multi core: %d ticks             \n\r", pids[0], pids[1], multiCoreEnd - multiCoreBegin);

  sys_exit();
}

void add_task1()
{
  int print_location = 2;
  int from = 0;
  int to = MAX_RANGE;
  int result = 0;
  int i;
  // sys_move_cursor(0, print_location);
  //  printf("start compute, from = %d, to = %d  ", from, to);

  for (i = from; i < to; ++i)
  {
    result = (result + i) % MOD;
  }
  //  printf("Done \n\r");
  sys_exit();
}

void add_task2()
{
  int print_location = 4;
  int from = 0;
  int to = MAX_RANGE / NUM_CPUS;
  int result = 0;
  int i;
  //  sys_move_cursor(0, print_location);
  //  printf("start compute, from = %d, to = %d  ", from, to);

  for (i = from; i < to; ++i)
  {
    result = (result + i) % MOD;
  }
  //  printf("Done \n\r");
  sys_exit();
}
void add_task3()
{
  int print_location = 6;
  int from = MAX_RANGE / NUM_CPUS;
  int to = MAX_RANGE;
  int result = 0;
  int i;
  //  sys_move_cursor(0, print_location);
  // printf("start compute, from = %d, to = %d  ", from, to);

  for (i = from; i < to; ++i)
  {
    result = (result + i) % MOD;
  }
  //  printf("Done \n\r");
  sys_exit();
}
