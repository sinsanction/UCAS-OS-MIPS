#include "fs.h"
#include "sem.h"
#include "irq.h"
#include "cond.h"
#include "lock.h"
#include "sched.h"
#include "common.h"
#include "screen.h"
#include "barrier.h"
#include "syscall.h"

void system_call_helper(uint64_t fn, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
}

void sys_spawn(task_info_t *info)
{
}

void sys_exit(void)
{
}

void sys_sleep(uint32_t time)
{
}

int sys_kill(pid_t pid)
{
}

int sys_waitpid(pid_t pid)
{
}

void sys_write(char *buff)
{
}

void sys_reflush()
{
}

void sys_move_cursor(int x, int y)
{
}

void mutex_lock_init(mutex_lock_t *lock)
{
}

void mutex_lock_acquire(mutex_lock_t *lock)
{
}

void mutex_lock_release(mutex_lock_t *lock)
{
}

void condition_init(condition_t *condition)
{
}

void condition_wait(mutex_lock_t *lock, condition_t *condition)
{
}
void condition_signal(condition_t *condition)
{
}

void condition_broadcast(condition_t *condition)
{
}

void semaphore_init(semaphore_t *s, int val)
{
}
void semaphore_up(semaphore_t *s)
{
}

void semaphore_down(semaphore_t *s)
{
}

void barrier_init(barrier_t *barrier, int goal)
{
}

void barrier_wait(barrier_t *barrier)
{
}

int sys_read_shell_buff(char *buff)
{
}

void sys_process_show(void)
{
}
void sys_screen_clear(int line1, int line2)
{
}

pid_t sys_getpid()
{
}

void sys_mkfs()
{
}

int sys_mkdir(char *name)
{
}

int sys_readdir(char *name)
{
}

int sys_enterdir(char *name)
{
}

int sys_rmdir(char *name)
{
}

int sys_print_fs(char *name)
{
}

int sys_mknod(char *name)
{
}

int sys_fopen(char *name, uint32_t access)
{
}

int sys_fwrite(uint32_t fd, char *buff, uint32_t size)
{
}

int sys_cat(char *name)
{
}

int sys_fread(uint32_t fd, char *buff, uint32_t size)
{
}

int sys_close(uint32_t fd)
{
}

uint32_t sys_net_recv(uint64_t rd, uint64_t rd_phy, uint64_t daddr)
{
}

void sys_net_send(uint64_t td, uint64_t td_phy)
{
}

void sys_init_mac()
{
}