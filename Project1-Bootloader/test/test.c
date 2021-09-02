#include "test.h"

// use shell to test kill a process
struct task_info shell_task = {"shell", (uint64_t)&test_shell, USER_PROCESS};
struct task_info *shell_tasks[16] = {&shell_task};
int num_shell_tasks = 1;
