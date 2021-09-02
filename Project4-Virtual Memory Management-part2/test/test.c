#include "test.h"

// use shell to test kill a process
struct task_info shell_task = {"shell", (uint64_t)0x1000000, 0, USER_PROCESS, 1, 0};
struct task_info proc_swap = {"proc_swap", (uint64_t)0x2000000, 0, USER_PROCESS, 1, 0};
struct task_info task1 = {"mem_test1", (uint64_t)0x3000000, 0, USER_PROCESS, 1, 0};
struct task_info task2 = {"plan", (uint64_t)0x4000000, 0, USER_PROCESS, 1, 0};
struct task_info task3 = {"swap_task", (uint64_t)0x5000000, 0, USER_PROCESS, 1, 0};
struct task_info task4 = {"shm_task0", (uint64_t)0x6000000, 0, USER_PROCESS, 1, 0};
struct task_info task5 = {"shm_task1", (uint64_t)0x7000000, 0, USER_PROCESS, 1, 0};

struct task_info *user_tasks[16] = {
    &shell_task,
    &proc_swap,
    &task1,
    &task2,
    &task3,
    &task4,
    &task5,
};
int num_user_tasks = 7;
