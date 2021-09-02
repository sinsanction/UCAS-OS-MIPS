#include "type.h"
#include "stdio.h"
#include "syscall.h"

#define RW_TIMES 2

int rand()
{
    int current_time = sys_get_timer();
    return current_time % 100000;
}

void __attribute__((section(".entry_function"))) _start(uint64_t addr1, uint64_t addr2)
{
	uint64_t mem1 = 0;
	int mem2 = 0;
	int curs = 1;
	int memory[RW_TIMES * 2];

	int i = 0;
	sys_move_cursor(1, 0);

	//srand((uint32_t)get_ticks());
	for (i = 0; i < RW_TIMES; i++)
	{
		sys_move_cursor(1, curs + i);
		mem1 = (i == 0) ? addr1 : addr2;

		memory[i] = mem2 = rand();
		//memory[i] = mem2 = 12345678;
		*(int *)mem1 = mem2;
		printf("Write: 0x%x,%d", mem1, mem2);
	}
	curs += RW_TIMES;
	for (i = 0; i < RW_TIMES; i++)
	{
		sys_move_cursor(1, curs + i);
		mem1 = (i == 0) ? addr1 : addr2;

		memory[i + RW_TIMES] = *(int *)mem1;
		if (memory[i + RW_TIMES] == memory[i])
			printf("Read succeed: 0x%x,%d", mem1, memory[i + RW_TIMES]);
		else
			printf("Read error: 0x%x,%d", mem1, memory[i + RW_TIMES]);
	}
	sys_exit();
}
