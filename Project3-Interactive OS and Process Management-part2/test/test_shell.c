/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                  The shell acts as a task running in user mode. 
 *       The main function is to make system calls through the user's output.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
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

#include "screen.h"
#include "stdio.h"
#include "syscall.h"
#include "test.h"
#include "sched.h"

#ifdef P3_TEST

struct task_info task1 = {"task1", (uint64_t)&ready_to_exit_task, USER_PROCESS, 1};
struct task_info task2 = {"task2", (uint64_t)&wait_lock_task, USER_PROCESS, 1};
struct task_info task3 = {"task3", (uint64_t)&wait_exit_task, USER_PROCESS, 1};

struct task_info task4 = {"task4", (uint64_t)&semaphore_add_task1, USER_PROCESS, 1};
struct task_info task5 = {"task5", (uint64_t)&semaphore_add_task2, USER_PROCESS, 1};
struct task_info task6 = {"task6", (uint64_t)&semaphore_add_task3, USER_PROCESS, 1};

struct task_info task7 = {"task7", (uint64_t)&producer_task, USER_PROCESS, 1};
struct task_info task8 = {"task8", (uint64_t)&consumer_task1, USER_PROCESS, 1};
struct task_info task9 = {"task9", (uint64_t)&consumer_task2, USER_PROCESS, 1};

struct task_info task10 = {"task10", (uint64_t)&barrier_task1, USER_PROCESS, 1};
struct task_info task11 = {"task11", (uint64_t)&barrier_task2, USER_PROCESS, 1};
struct task_info task12 = {"task12", (uint64_t)&barrier_task3, USER_PROCESS, 1};

struct task_info task13 = {"SunQuan", (uint64_t)&SunQuan, USER_PROCESS, 1};
struct task_info task14 = {"LiuBei", (uint64_t)&LiuBei, USER_PROCESS, 1};
struct task_info task15 = {"CaoCao", (uint64_t)&CaoCao, USER_PROCESS, 1};

#ifdef P4_TEST
struct task_info task16 = {"mem_test1", (uint64_t)&rw_task1, USER_PROCESS};
struct task_info task17 = {"plan", (uint64_t)&drawing_task1, USER_PROCESS};
#endif

#ifdef P5_TEST
struct task_info task18 = {"mac_send", (uint64_t)&mac_send_task, USER_PROCESS};
struct task_info task19 = {"mac_recv", (uint64_t)&mac_recv_task, USER_PROCESS};
#endif

#ifdef P6_TEST

struct task_info task19 = {"fs_test", (uint64_t)&test_fs, USER_PROCESS};
#endif
struct task_info task16 = {"multcore", (uint64_t)&test_multicore, USER_PROCESS, 1};
struct task_info task17 = {"affinity", (uint64_t)&test_affinity, USER_PROCESS, 1};
static struct task_info *test_tasks[NUM_MAX_TASK] = {
    &task1,
    &task2,
    &task3,
    &task4,
    &task5,
    &task6,
    &task7,
    &task8,
    &task9,
    &task10,
    &task11,
    &task12,
    &task13,
    &task14,
    &task15,
    &task16,
    &task17,
};

#endif

#define SHELL_BEGIN 15

char getchar(void)
{
    char *tx_fifo_write = (char *)0xffffffffbfe00000;
	char *tx_fifo_state = (char *)0xffffffffbfe00005;

    while(1){
        if( (*tx_fifo_state) & 0x01 )
        return (*tx_fifo_write);
    }
}

void check_add_cursor_y(int *cursor_y)
{
    (*cursor_y)++;
    if(*cursor_y >= SCREEN_HEIGHT){
        sys_screen_scroll(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
        (*cursor_y)--;
    }
}

void test_shell(void)
{
    char *tx_fifo_write = (char *)0xffffffffbfe00000;
	char *tx_fifo_state = (char *)0xffffffffbfe00005;
    char remind_str[19] = "> Joker@UCAS_OS:$ \0";
    char buff[256];
    int buff_ptr;
    char input;
    int cursor_x;
    int cursor_y;
    int x_begin = strlen(remind_str);
    int i, j, flag;
    int op_begin[6];
    int op_end[6];
    int op_num;
    int number;
    int number2;

    sys_move_cursor(1, SHELL_BEGIN);
    printf("------------------- COMMAND -------------------\n");
    cursor_y = SHELL_BEGIN + 1;

    while(1){
        sys_move_cursor(0, cursor_y);
        printf("%s", remind_str);
        cursor_x = x_begin;
        buff_ptr = 0;
        /* get serial input */
        while(1){
            input = getchar();
            
            if(input == '\b' || input == 127){
                if(buff_ptr > 0){
                    cursor_x--;
                    buff_ptr--;
                    sys_move_cursor(cursor_x, cursor_y);
                    printf(" ");
                }
            }
            else if(input == 13 || input == '\n'){
                buff[buff_ptr] = '\0';
                break;
            }
            else{
                buff[buff_ptr++] = input;
                sys_move_cursor(cursor_x, cursor_y);
                printf("%c", input);
                cursor_x++;
            }
        }

        /* parse argument and command */
        check_add_cursor_y(&cursor_y);
        cursor_x = 0;
        op_num = 0;
        for(i=0; i<=buff_ptr; i++){
            if(buff[i] != ' '){
                if(buff[i] == '\0'){
                    break;
                }
                else{
                    op_begin[op_num] = i;
                    for(j=i+1; j<=buff_ptr; j++){
                        if(buff[j] == '\0'){
                            op_end[op_num] = j;
                            op_num++;
                            i = buff_ptr;
                            if(op_num >= 5){
                                goto OUT;
                            }
                            break;
                        }
                        if(buff[j] == ' '){
                            op_end[op_num] = j;
                            op_num++;
                            i = j;
                            if(op_num >= 5){
                                goto OUT;
                            }
                            break;
                        }
                    }
                }
            }
        }
        OUT:
        if(op_num == 0){
            continue;
        }
        buff[op_end[0]] = '\0';
        int op_ps    = (strcmp(&buff[op_begin[0]], "ps") == 0);
        int op_clear = (strcmp(&buff[op_begin[0]], "clear") == 0);
        int op_kill  = (strcmp(&buff[op_begin[0]], "kill") == 0);
        int op_exec  = (strcmp(&buff[op_begin[0]], "exec") == 0);
        int op_taskset = (strcmp(&buff[op_begin[0]], "taskset") == 0);

        sys_move_cursor(cursor_x, cursor_y);
        //printf("op_num: %d ps: %d clear: %d kill: %d exec: %d", op_num, op_ps, op_clear, op_kill, op_exec);
        //check_add_cursor_y(&cursor_y);
        //sys_move_cursor(cursor_x, cursor_y);

        /* rocessing command */
        if(op_num == 1){
            if(op_ps){
                int ps_table[NUM_MAX_TASK*4];
                int ps_num;
                printf("[PROCESS TABLE]");
                ps_num = sys_process_show(ps_table);
                for(i=0; i<ps_num; i++){
                    check_add_cursor_y(&cursor_y);
                    sys_move_cursor(cursor_x, cursor_y);
                    switch(ps_table[i*4+1]){
                    case 0:
                        printf("[%d] PID: %d  STATUS: BLOCKED  PRIORITY: %d  INCORE: %d", i+1, ps_table[i*4], ps_table[i*4+2], ps_table[i*4+3]);
                        break;
                    case 1:
                        printf("[%d] PID: %d  STATUS: RUNNING  PRIORITY: %d  INCORE: %d", i+1, ps_table[i*4], ps_table[i*4+2], ps_table[i*4+3]);
                        break;
                    case 2:
                        printf("[%d] PID: %d  STATUS: READY    PRIORITY: %d  INCORE: %d", i+1, ps_table[i*4], ps_table[i*4+2], ps_table[i*4+3]);
                        break;
                    }
                }
            }
            else if(op_clear){
                printf("do clear");
                sys_screen_clear(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
                sys_screen_clear(0, SHELL_BEGIN - 1);
                cursor_y = SHELL_BEGIN + 1;
                continue;
            }
            else if(op_exec || op_kill || op_taskset){
                printf("%s: too few arguments", &buff[op_begin[0]]);
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else if(op_num == 2){
            number = 0;
            flag = 1;
            buff[op_end[1]] = '\0';
            for(i=op_begin[1]; i<op_end[1]; i++){
                if(buff[i]>='0' && buff[i]<='9'){
                    number = number*10 + (buff[i] - '0');
                }
                else{
                    flag = 0;
                    break;
                }
            }

            if(op_exec){
                if(flag && number>=0 && number<=16){
                    int pid;
                    pid = (int)sys_spawn(test_tasks[number], 2);
                    printf("exec process[%d], pid=%d", number, pid);
                }
                else{
                    printf("exec: invalid argument: '%s'", &buff[op_begin[1]]);
                }
            }
            else if(op_kill){
                if(flag && number>=0 && number<=1024){
                    int kill_res;
                    printf("kill process pid=%d", number);
                    check_add_cursor_y(&cursor_y);
                    sys_move_cursor(cursor_x, cursor_y);
                    kill_res = sys_kill(number);
                    if(kill_res == 0){
                        printf("sorry, you don't have permission to kill the process pid=%d", number);
                    }
                    else if(kill_res == -1){
                        printf("fail, the process pid=%d is not running", number);
                    }
                    else{
                        printf("succeed, the process pid=%d has been killed", number);
                    }
                }
                else{
                    printf("kill: invalid argument: '%s'", &buff[op_begin[1]]);
                }
            }
            else if(op_ps || op_clear){
                printf("%s: too many arguments", &buff[op_begin[0]]);
            }
            else if(op_taskset){
                printf("%s: too few arguments", &buff[op_begin[0]]);
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else if(op_num == 3){
            number = 0;
            flag = 1;
            buff[op_end[1]] = buff[op_end[2]] = '\0';
            for(i=op_begin[1]; i<op_end[1]; i++){
                if(buff[i]>='0' && buff[i]<='9'){
                    number = number*10 + (buff[i] - '0');
                }
                else{
                    flag = 0;
                    break;
                }
            }
            int op_p = (strcmp(&buff[op_begin[1]], "-p") == 0);

            if(op_taskset){
                if(flag && number>=1 && number<=3){
                    number2 = 0;
                    flag = 1;
                    for(i=op_begin[2]; i<op_end[2]; i++){
                        if(buff[i]>='0' && buff[i]<='9'){
                            number2 = number2*10 + (buff[i] - '0');
                        }
                        else{
                            flag = 0;
                            break;
                        }
                    }

                    if(flag && number2>=0 && number2<=16){
                        int pid;
                        pid = (int)sys_spawn(test_tasks[number2], number-1);
                        printf("taskset process[%d], pid=%d, cpu=%d", number2, pid, number-1);
                    }
                    else{
                        printf("taskset: invalid argument: '%s'", &buff[op_begin[2]]);
                    }
                }
                else if(op_p){
                    printf("%s %s: too few arguments", &buff[op_begin[0]], &buff[op_begin[1]]);
                }
                else{
                    printf("taskset: invalid argument: '%s'", &buff[op_begin[1]]);
                }
            }
            else if(op_ps || op_clear || op_kill || op_exec){
                printf("%s: too many arguments", &buff[op_begin[0]]);
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else if(op_num == 4){
            buff[op_end[1]] = buff[op_end[2]] = buff[op_end[3]] = '\0';
            int op_p = (strcmp(&buff[op_begin[1]], "-p") == 0);

            if(op_taskset && op_p){
                number = 0;
                flag = 1;
                for(i=op_begin[2]; i<op_end[2]; i++){
                    if(buff[i]>='0' && buff[i]<='9'){
                        number = number*10 + (buff[i] - '0');
                    }
                    else{
                        flag = 0;
                        break;
                    }
                }
                if(flag && number>=1 && number<=3){
                    number2 = 0;
                    flag = 1;
                    for(i=op_begin[3]; i<op_end[3]; i++){
                        if(buff[i]>='0' && buff[i]<='9'){
                            number2 = number2*10 + (buff[i] - '0');
                        }
                        else{
                            flag = 0;
                            break;
                        }
                    }
                    if(flag && number2>=0 && number2<=1024){
                        int ret;
                        ret = sys_band(number2, number-1);
                        if(ret == 0){
                            printf("sorry, you don't have permission to change the process pid=%d", number2);
                        }
                        else if(ret == -1){
                            printf("fail, the process pid=%d is not running", number2);
                        }
                        else{
                            printf("succeed, the process pid=%d is banded to cpu %d", number2, number-1);
                        }
                    }
                    else{
                        printf("taskset -p: invalid argument: '%s'", &buff[op_begin[3]]);
                    }
                }
                else{
                    printf("taskset -p: invalid argument: '%s'", &buff[op_begin[2]]);
                }
            }
            else if(op_ps || op_clear || op_kill || op_exec || op_taskset){
                printf("%s: too many arguments", &buff[op_begin[0]]);
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else{
            printf("too many arguments");
        }
        check_add_cursor_y(&cursor_y);
    }
}
