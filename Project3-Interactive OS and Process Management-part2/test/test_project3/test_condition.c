#include "cond.h"
#include "lock.h"
#include "stdio.h"
#include "syscall.h"
#include "test3.h"
#include "time.h"

lock_id_t mutex_lock_id = 11;
condition_t condition;
static int num_staff = 0;

void producer_task(void)
{
    int i;
    int print_location = 1;
    int production = 3;
    int sum_production = 0;

    for (i = 0; i < 30; i++)
    {
        mutex_lock_acquire(mutex_lock_id);

        num_staff += production;
        sum_production += production;

        mutex_lock_release(mutex_lock_id);

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total produced %d products.", sum_production);

        condition_broadcast(&condition);

        sys_sleep(1);
    }

    sys_move_cursor(0, print_location);
    printf("> [TASK] finish, total produced %d products.", sum_production);
    sys_exit();
}

void consumer_task1(void)
{
    int print_location = 2;
    int consumption = 1;
    int sum_consumption = 0;

    while (1)
    {
        mutex_lock_acquire(mutex_lock_id);

        while (num_staff == 0)
        {
            condition_wait(mutex_lock_id, &condition);
        }

        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products.", sum_consumption);

        mutex_lock_release(mutex_lock_id);
        sys_sleep(1);
    }
}

void consumer_task2(void)
{
    int print_location = 3;
    int consumption = 1;
    int sum_consumption = 0;

    while (1)
    {
        mutex_lock_acquire(mutex_lock_id);

        while (num_staff == 0)
        {
            condition_wait(mutex_lock_id, &condition);
        }

        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products.", sum_consumption);

        mutex_lock_release(mutex_lock_id);
        sys_sleep(1);
    }
}