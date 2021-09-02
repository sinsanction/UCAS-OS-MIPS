#include "test.h"

/* task group to test do_scheduler() */
struct task_info task2_1 = {"task1", (uint64_t)&printk_task1, USER_PROCESS, 1};
struct task_info task2_2 = {"task1", (uint64_t)&printk_task2, USER_PROCESS, 1};
struct task_info task2_3 = {"task1", (uint64_t)&drawing_task1, USER_PROCESS, 1};
struct task_info *sched1_tasks[16] = {&task2_1, &task2_2, &task2_3};
int num_sched1_tasks = 3;

/* task group to test lock */
struct task_info task2_4 = {"task4", (uint64_t)&lock_task1, USER_PROCESS, 1};
struct task_info task2_5 = {"task5", (uint64_t)&lock_task2, USER_PROCESS, 1};
struct task_info *lock_tasks[16] = {&task2_4, &task2_5};
int num_lock_tasks = 2;

/* task group to test clock scheduler */
struct task_info task2_6 = {"task6", (uint64_t)&sleep_task, USER_PROCESS, 1};
struct task_info task2_7 = {"task7", (uint64_t)&timer_task, USER_PROCESS, 1};
struct task_info *timer_tasks[16] = {&task2_6, &task2_7};
int num_timer_tasks = 2;

/* task group to test clock scheduler */
struct task_info task2_8 = {"task8", (uint64_t)&printf_task1, USER_PROCESS, 1};
struct task_info task2_9 = {"task9", (uint64_t)&printf_task2, USER_PROCESS, 1};
struct task_info task2_10 = {"task10", (uint64_t)&drawing_task2, USER_PROCESS, 1};
struct task_info *sched2_tasks[16] = {&task2_8, &task2_9, &task2_10};
int num_sched2_tasks = 3;

/* task group to test priority scheduler */
struct task_info taskp_1 = {"taskp1", (uint64_t)&priority_tasks1, USER_PROCESS, 3};
struct task_info taskp_2 = {"taskp2", (uint64_t)&priority_tasks2, USER_PROCESS, 2};
struct task_info taskp_3 = {"taskp3", (uint64_t)&priority_tasks3, USER_PROCESS, 2};
struct task_info taskp_4 = {"taskp4", (uint64_t)&priority_tasks4, USER_PROCESS, 2};
struct task_info taskp_5 = {"taskp5", (uint64_t)&priority_tasks5, USER_PROCESS, 1};
struct task_info taskp_6 = {"taskp6", (uint64_t)&priority_tasks6, USER_PROCESS, 1};
struct task_info taskp_7 = {"taskp7", (uint64_t)&priority_tasks7, USER_PROCESS, 1};
struct task_info taskp_8 = {"taskp8", (uint64_t)&priority_tasks8, USER_PROCESS, 1};
struct task_info taskp_9 = {"taskp9", (uint64_t)&priority_tasks9, USER_PROCESS, 1};
struct task_info taskp_10 = {"taskp10", (uint64_t)&priority_tasks10, USER_PROCESS, 1};
struct task_info *priority_tasks[16] = {&taskp_1, &taskp_2, &taskp_3, &taskp_4, &taskp_5, &taskp_6, &taskp_7, &taskp_8, &taskp_9, &taskp_10};
int num_priority_tasks = 10;

/* task group to measure the time of do_scheduler */
struct task_info taskm_1 = {"taskm1", (uint64_t)&measure_task1, KERNEL_PROCESS, 1};
struct task_info *measure_tasks[16] = {&taskm_1};
int num_measure_tasks = 1;
