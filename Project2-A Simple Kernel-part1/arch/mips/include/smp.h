#ifndef INCLUDE_SMP_H_
#define INCLUDE_SMP_H_

//#include "lock.h"
extern spin_lock_t core_lock;
extern int corelock_is_init ;
void loongson3_boot_secondary();
extern void core_lock_aquire(void);
extern void core_lock_release(void);
#endif