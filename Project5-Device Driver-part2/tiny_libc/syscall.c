#include "syscall.h"

pid_t sys_spawn(uint64_t task_id, uint64_t argv1, uint64_t argv2)
{
    return invoke_syscall(SYSCALL_SPAWN, task_id, argv1, argv2);
}

void sys_exit(void)
{
    invoke_syscall(SYSCALL_EXIT, 0, 0, 0);
}

void sys_sleep(uint32_t time)
{
    invoke_syscall(SYSCALL_SLEEP, time, 0, 0);
}

int sys_kill(pid_t pid)
{
    return invoke_syscall(SYSCALL_KILL, pid, 0, 0);
}

int sys_waitpid(pid_t pid)
{
    return invoke_syscall(SYSCALL_WAITPID, pid, 0, 0);
}

/* used to test */
/*
void sys_getpriority(priority_info_t *prio)
{
    invoke_syscall(SYSCALL_GETPRIORITY, (uint64_t)prio, 0, 0);
}*/

uint32_t sys_get_timer(void)
{
    return invoke_syscall(SYSCALL_GETTIMER, 0, 0, 0);
}

void sys_do_scheduler(void)
{
    invoke_syscall(SYSCALL_YIELD, 0, 0, 0);
}

void sys_block(void)
{
    invoke_syscall(SYSCALL_BLOCK, 0, 0, 0);
}

void sys_update_pf(void)
{
    invoke_syscall(SYSCALL_UPDATE_PF, 0, 0, 0);
}

void sys_sdwrite(uint64_t buf, int disk_base, int n)
{
    invoke_syscall(SYSCALL_SDWRITE, buf, disk_base, n);
}

void sys_sdread(uint64_t buf, int disk_base, int n)
{
    invoke_syscall(SYSCALL_SDREAD, buf, disk_base, n);
}

void sys_get_pfnum(int *pfnum)
{
    invoke_syscall(SYSCALL_GETPFNUM, (uint64_t)pfnum, 0, 0);
}

void sys_get_swapreg(swap_t *swap_reg)
{
    invoke_syscall(SYSCALL_GETSWAPREG, (uint64_t)swap_reg, 0, 0);
}

uint64_t shmget(uint64_t key)
{
    return invoke_syscall(SYSCALL_SHMGET, key, 0, 0);
}

uint64_t shmat(uint64_t shmid)
{
    return invoke_syscall(SYSCALL_SHMAT, shmid, 0, 0);
}

uint64_t shmdt(uint64_t addr)
{
    return invoke_syscall(SYSCALL_SHMDT, addr, 0, 0);
}


int binsemget(int key)
{
    return invoke_syscall(SYSCALL_BINSEM_GET, key, 0, 0);
}

void binsemop(int bin_id, int bin_op)
{
    if(bin_op == 1)
        invoke_syscall(SYSCALL_BINSEM_ACQUIRE, bin_id, 0, 0);
    else if(bin_op == 0)
        invoke_syscall(SYSCALL_BINSEM_RELEASE, bin_id, 0, 0);
}

void sys_write(char *buff)
{
    invoke_syscall(SYSCALL_WRITE, (uint64_t)buff, 0, 0);
}

void sys_reflush(void)
{
    invoke_syscall(SYSCALL_REFLUSH, 0, 0, 0);
}

void sys_move_cursor(int x, int y)
{
    invoke_syscall(SYSCALL_CURSOR, x, y, 0);
}
/*
void mutex_lock_init(lock_id_t lock_id)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_INIT, (uint64_t)lock_id, 0, 0);
}

void mutex_lock_acquire(lock_id_t lock_id)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_ACQUIRE, (uint64_t)lock_id, 0, 0);
}

void mutex_lock_release(lock_id_t lock_id)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_RELEASE, (uint64_t)lock_id, 0, 0);
}

void condition_init(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_INIT, (uint64_t)condition, 0, 0);
}

void condition_wait(lock_id_t lock_id, condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_WAIT, (uint64_t)lock_id, (uint64_t)condition, 0);
}

void condition_signal(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_SIGNAL, (uint64_t)condition, 0, 0);
}

void condition_broadcast(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_BROADCAST, (uint64_t)condition, 0, 0);
}

void semaphore_init(semaphore_t *s, int val)
{
    invoke_syscall(SYSCALL_SEMAPHORE_INIT, (uint64_t)s, val, 0);
}

void semaphore_up(semaphore_t *s)
{
    invoke_syscall(SYSCALL_SEMAPHORE_UP, (uint64_t)s, 0, 0);
}

void semaphore_down(semaphore_t *s)
{
    invoke_syscall(SYSCALL_SEMAPHORE_DOWN, (uint64_t)s, 0, 0);
}

void barrier_init(barrier_t *barrier, int goal)
{
    invoke_syscall(SYSCALL_BARRIER_INIT, (uint64_t)barrier, goal, 0);
}

void barrier_wait(barrier_t *barrier)
{
    invoke_syscall(SYSCALL_BARRIER_WAIT, (uint64_t)barrier, 0, 0);
}

int mbox_open(char *name)
{
    return invoke_syscall(SYSCALL_MBOX_OPEN, (uint64_t)name, 0, 0);
}

void mbox_close(mailbox_id_t boxid)
{
    invoke_syscall(SYSCALL_MBOX_CLOSE, boxid, 0, 0);
}

void mbox_send(mailbox_id_t boxid, void *msg, int msg_length)
{
    invoke_syscall(SYSCALL_MBOX_SEND, boxid, (uint64_t)msg, msg_length);
}

void mbox_recv(mailbox_id_t boxid, void *msg, int msg_length)
{
    invoke_syscall(SYSCALL_MBOX_RECV, boxid, (uint64_t)msg, msg_length);
}*/

int sys_read_shell_buff(char *buff)
{
    return invoke_syscall(SYSCALL_READ_SHELL_BUFF, (uint64_t)buff, 0, 0);
}

int sys_process_show(int *ps_base)
{
    return invoke_syscall(SYSCALL_PS, (uint64_t)ps_base, 0, 0);
}

void sys_screen_clear(int line1, int line2)
{
    invoke_syscall(SYSCALL_SCREEN_CLEAR, line1, line2, 0);
}

void sys_screen_scroll(int line1, int line2)
{
    invoke_syscall(SYSCALL_SCREEN_SCROLL, line1, line2, 0);
}

pid_t sys_getpid(void)
{
    return invoke_syscall(SYSCALL_GETPID, 0, 0, 0);
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

uint32_t sys_net_recv(uint64_t recv_reg_addr)
{
    return invoke_syscall(SYSCALL_NET_RECV, recv_reg_addr, 0, 0);
}

void sys_net_send(uint64_t buf_addr, uint64_t size, uint64_t num)
{
    invoke_syscall(SYSCALL_NET_SEND, buf_addr, size, num);
}

void sys_init_mac(void)
{
    invoke_syscall(SYSCALL_INIT_MAC, 0, 0, 0);
}
