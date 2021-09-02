#include "asm.h"

LEAF(invoke_syscall)
    syscall
    jr  ra
END(invoke_syscall)