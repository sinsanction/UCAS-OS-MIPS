/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                       System call related processing
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#ifndef INCLUDE_SYSCALL_H_
#define INCLUDE_SYSCALL_H_

#include "type.h"
#include "sync.h"
#include "queue.h"

#define IGNORE 0
#define NUM_SYSCALLS 64

/* define */
#define SYSCALL_SPAWN 0
#define SYSCALL_EXIT 1
#define SYSCALL_SLEEP 2
#define SYSCALL_KILL 3
#define SYSCALL_WAITPID 4
#define SYSCALL_PS 5
#define SYSCALL_GETPID 6

#define SYSCALL_WRITE 20
#define SYSCALL_READ 21
#define SYSCALL_CURSOR 22
#define SYSCALL_REFLUSH 23
#define SYSCALL_SERIAL_READ 24
#define SYSCALL_SERIAL_WRITE 25
#define SYSCALL_READ_SHELL_BUFF 26
#define SYSCALL_SCREEN_CLEAR 27

#define SYSCALL_MUTEX_LOCK_INIT 30
#define SYSCALL_MUTEX_LOCK_ACQUIRE 31
#define SYSCALL_MUTEX_LOCK_RELEASE 32

#define SYSCALL_CONDITION_INIT 33
#define SYSCALL_CONDITION_WAIT 34
#define SYSCALL_CONDITION_SIGNAL 35
#define SYSCALL_CONDITION_BROADCAST 36

#define SYSCALL_SEMAPHORE_INIT 37
#define SYSCALL_SEMAPHORE_UP 38
#define SYSCALL_SEMAPHORE_DOWN 39

#define SYSCALL_BARRIER_INIT 40
#define SYSCALL_BARRIER_WAIT 41

#define SYSCALL_FS_INIT 50
#define SYSCALL_FS_MKDIR 51
#define SYSCALL_FS_RMDIR 52
#define SYSCALL_FS_READDIR 53
#define SYSCALL_FS_ENTERDIR 54
#define SYSCALL_FS_PRINT 55
#define SYSCALL_FS_MKNOD 56
#define SYSCALL_FS_OPEN 57
#define SYSCALL_FS_WRITE 58
#define SYSCALL_FS_CAT 59
#define SYSCALL_FS_READ 60
#define SYSCALL_FS_CLOSE 61

#define SYSCALL_WAIT_RECV_PACKAGE 42
#define SYSCALL_NET_RECV 43
#define SYSCALL_NET_SEND 44
#define SYSCALL_INIT_MAC 45

/* syscall function pointer */
uint64_t (*syscall[NUM_SYSCALLS])();

void system_call_helper(uint64_t,uint64_t,uint64_t,uint64_t);
extern uint64_t invoke_syscall(uint64_t,uint64_t,uint64_t,uint64_t);

void sys_exit(void);
void sys_sleep(uint32_t);
int sys_kill(pid_t);
int sys_waitpid(pid_t);
void sys_process_show(void);
pid_t sys_getpid();

void sys_write(char *);
void sys_move_cursor(int, int);
void sys_reflush();
char sys_serial_read();
void sys_serial_write(char);
int sys_read_shell_buff(char *);
void sys_screen_clean(int, int);

void mutex_lock_init(mutex_lock_t *);
void mutex_lock_acquire(mutex_lock_t *);
void mutex_lock_release(mutex_lock_t *);

void condition_init(condition_t *);
void condition_wait(mutex_lock_t *, condition_t *);
void condition_signal(condition_t *);
void condition_broadcast(condition_t *);

void semaphore_init(semaphore_t *, int);
void semaphore_up(semaphore_t *);
void semaphore_down(semaphore_t *);

void barrier_init(barrier_t *, int);
void barrier_wait(barrier_t *);

void sys_mkfs();
int sys_mkdir(char *);
int sys_readdir(char *);
int sys_enterdir(char *);
int sys_rmdir(char *);
int sys_print_fs();
int sys_mknod(char *);

int sys_fopen(char *, uint32_t);
int sys_fwrite(uint32_t, char *, uint32_t);
int sys_fread(uint32_t, char *, uint32_t);
int sys_close(uint32_t);
int sys_cat(char *);

#endif