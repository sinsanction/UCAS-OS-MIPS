#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "test.h"

void priority_tasks1(void)
{
    int print_location = 1;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 01: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks2(void)
{
    int print_location = 2;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 02: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks3(void)
{
    int print_location = 3;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 03: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks4(void)
{
    int print_location = 4;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 04: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks5(void)
{
    int print_location = 5;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 05: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks6(void)
{
    int print_location = 6;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 06: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks7(void)
{
    int print_location = 7;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 07: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks8(void)
{
    int print_location = 8;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 08: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks9(void)
{
    int print_location = 9;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 09: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}

void priority_tasks10(void)
{
    int print_location = 10;
    priority_info_t now_priority;

    while(1)
    {
        sys_getpriority(&now_priority);
        sys_move_cursor(1, print_location);
        printf("> [TASK] priority tasks 10: priority: (%d) runtime_priority: (%d) runtimes: (%d)\n", now_priority.priority, now_priority.runtime_priority, now_priority.runtimes);
    }
}
