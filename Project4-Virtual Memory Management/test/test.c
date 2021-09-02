#include "test.h"

// use shell to test kill a process
struct task_info shell_task = {"shell", (uint64_t)&test_shell, USER_PROCESS, 1};
struct task_info *shell_tasks[16] = {&shell_task};
int num_shell_tasks = 1;

struct task_info proc_swap = {"proc_swap", (uint64_t)&process_swap, USER_PROCESS, 1};
struct task_info *proc_swap_tasks[16] = {&proc_swap};
int num_proc_swap_tasks = 1;