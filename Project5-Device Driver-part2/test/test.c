#include "test.h"

// use shell to test kill a process
struct task_info shell_task = {"shell", (uint64_t)USER_BASE_0, 0, USER_PROCESS, 1, 0};
struct task_info task1 = {"recv_test", (uint64_t)USER_BASE_1, 0, USER_PROCESS, 1, 0};
struct task_info task2 = {"send_test", (uint64_t)USER_BASE_2, 0, USER_PROCESS, 1, 0};

struct task_info *user_tasks[16] = {
    &shell_task,
    &task1,
    &task2,
};
int num_user_tasks = 3;
