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
#include "sched.h"

#define IGNORE 0
#define NUM_SYSCALLS 70

/* define */
#define SYSCALL_SPAWN 0
#define SYSCALL_EXIT 1
#define SYSCALL_SLEEP 2
#define SYSCALL_KILL 3
#define SYSCALL_WAITPID 4
#define SYSCALL_PS 5
#define SYSCALL_GETPID 6

#define SYSCALL_GETTIMER 8
#define SYSCALL_YIELD 9
#define SYSCALL_GETPRIORITY 10

#define SYSCALL_BINSEM_GET 11
#define SYSCALL_BINSEM_ACQUIRE 12
#define SYSCALL_BINSEM_RELEASE 13

#define SYSCALL_BLOCK 15
#define SYSCALL_UPDATE_PF 16
#define SYSCALL_SDWRITE 17
#define SYSCALL_SDREAD 18

#define SYSCALL_WRITE 20
#define SYSCALL_READ 21
#define SYSCALL_CURSOR 22
#define SYSCALL_REFLUSH 23
#define SYSCALL_SERIAL_READ 24
#define SYSCALL_SERIAL_WRITE 25
#define SYSCALL_READ_SHELL_BUFF 26
#define SYSCALL_SCREEN_CLEAR 27
#define SYSCALL_SCREEN_SCROLL 28

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

#define SYSCALL_MBOX_OPEN  46
#define SYSCALL_MBOX_CLOSE 47
#define SYSCALL_MBOX_SEND  48
#define SYSCALL_MBOX_RECV  49

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

typedef uint64_t (*syscall_func)();

/* syscall function pointer */
uint64_t (*syscall[NUM_SYSCALLS])();

uint64_t system_call_helper(uint64_t,uint64_t,uint64_t,uint64_t);

#endif