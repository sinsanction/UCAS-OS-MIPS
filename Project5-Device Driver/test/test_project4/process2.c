#include "sched.h"
#include "screen.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"

#include "test4.h"

#define RW_TIMES 2
uint64_t atoi(char *input)
{
	uint64_t sum = 0;

	while((*input)!='\0'){
		if((*input)>='0'&&(*input)<='9'){
			sum = sum * 16 + (*input) - '0';
		}
		else if((*input)>='a'&&(*input)<='f'){
			sum = sum * 16 + (*input) - 'a' + 10;
		}
		input++;
	}
	return sum;
}
void rw_task1(char *argv[])
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
		mem1 = atoi(argv[i]);

		//memory[i] = mem2 = rand();
		memory[i] = mem2 = 12345678;
		*(int *)mem1 = mem2;
		printf("Write: 0x%x,%d", mem1, mem2);
	}
	curs += RW_TIMES;
	for (i = 0; i < RW_TIMES; i++)
	{
		sys_move_cursor(1, curs + i);
		mem1 = atoi(argv[RW_TIMES + i]);

		memory[i + RW_TIMES] = *(int *)mem1;
		if (memory[i + RW_TIMES] == memory[i])
			printf("Read succeed: 0x%x,%d", mem1, memory[i + RW_TIMES]);
		else
			printf("Read error: 0x%x,%d", mem1, memory[i + RW_TIMES]);
	}
	sys_exit();
}
