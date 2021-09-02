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
#include "mailbox.h"

uint64_t system_call_helper(uint64_t fn, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    return syscall[fn](arg1, arg2, arg3);
}
