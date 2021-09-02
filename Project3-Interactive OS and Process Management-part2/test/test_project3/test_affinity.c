#include <time.h>
#include <stdio.h>

#include <syscall.h>
#include "test.h"
#define MAX_ITERATION 1000000
#define INTEGER_TEST_CHUNK 100
#define INTEGER_TEST_NUM 5

void integer_test_task();
int print_location = 2;
void test_affinity(void)
{
    int i;
    // srand(42);
    sys_move_cursor(1, 1);
    printf("start test cpu affinity, pids = {");
    int single_core_result = 0;
    struct task_info task_test = {"interger_task", (uint64_t)&integer_test_task, USER_PROCESS, 1};
    pid_t pids[INTEGER_TEST_NUM] = {0};
    for (i = 0; i < INTEGER_TEST_NUM; ++i)
    {
        pids[i] = sys_spawn(&task_test, 0);
        printf("%d, ", pids[i]);
    }
    printf("}\n\r");
    for (i = 0; i < INTEGER_TEST_NUM; ++i)
    {
        sys_waitpid(pids[i]);
    }

    sys_exit();
}

void integer_test_task()
{
    pid_t pid = sys_getpid();
    uint64_t ans = 0;
    int i, j;
    int location = print_location++;

    for (i = 0; i < MAX_ITERATION; ++i)
    {
        for (j = 0; j < INTEGER_TEST_CHUNK; ++j)
        {
            ans += j;
        }
        sys_move_cursor(1, location);
        printf("[%d] integer test (%d/%d)\n\r", pid, i, MAX_ITERATION);
    }
    sys_exit();
}
