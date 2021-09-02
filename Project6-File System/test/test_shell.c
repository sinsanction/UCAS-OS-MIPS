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

#include "type.h"
#include "stdio.h"
#include "syscall.h"
#include "string.h"

#define SHELL_BEGIN 15
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 30
#define NUM_MAX_TASK 32

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

int check_copy(char *dest, char *buff, int begin, int end){
    int i;
    if(buff[begin]!='0' || buff[begin+1]!='x')
        return -1;
    for(i=begin+2; i<end; i++){
        if(!( (buff[i]>='0'&&buff[i]<='9') || (buff[i]>='a'&&buff[i]<='f') ))
            return -1;
    }

    strcpy(dest, &(buff[begin+2]));
    return 1;
}

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

char arg[2][24];
uint64_t argv[2];
int ps_table[NUM_MAX_TASK*3];

void __attribute__((section(".entry_function"))) _start(void)
{
    char remind_str[128] = "> Joker@UCAS_OS:$ \0";
    char buff[256];
    int buff_ptr;
    char input;
    int cursor_x;
    int cursor_y;
    int x_begin = strlen(remind_str);
    int i, j, flag;
    int op_begin[8];
    int op_end[8];
    int op_num;
    int number;

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
                            if(op_num >= 8){
                                goto OUT;
                            }
                            break;
                        }
                        if(buff[j] == ' '){
                            op_end[op_num] = j;
                            op_num++;
                            i = j;
                            if(op_num >= 8){
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
        int op_ps      = (strcmp(&buff[op_begin[0]], "ps") == 0);
        int op_clear   = (strcmp(&buff[op_begin[0]], "clear") == 0);
        int op_kill    = (strcmp(&buff[op_begin[0]], "kill") == 0);
        int op_exec    = (strcmp(&buff[op_begin[0]], "exec") == 0);
        int op_mkfs    = (strcmp(&buff[op_begin[0]], "mkfs") == 0);
        int op_statfs  = (strcmp(&buff[op_begin[0]], "statfs") == 0);
        int op_clrfs   = (strcmp(&buff[op_begin[0]], "clrfs") == 0);
        int op_ls      = (strcmp(&buff[op_begin[0]], "ls") == 0);
        int op_cd      = (strcmp(&buff[op_begin[0]], "cd") == 0);
        int op_mkdir   = (strcmp(&buff[op_begin[0]], "mkdir") == 0);
        int op_rmdir   = (strcmp(&buff[op_begin[0]], "rmdir") == 0);
        int op_touch   = (strcmp(&buff[op_begin[0]], "touch") == 0);
        int op_cat     = (strcmp(&buff[op_begin[0]], "cat") == 0);
        int op_ln      = (strcmp(&buff[op_begin[0]], "ln") == 0);

        sys_move_cursor(cursor_x, cursor_y);
        //printf("op_num: %d ps: %d clear: %d kill: %d exec: %d", op_num, op_ps, op_clear, op_kill, op_exec);
        //check_add_cursor_y(&cursor_y);
        //sys_move_cursor(cursor_x, cursor_y);

        /* rocessing command */
        if(op_num == 1){
            if(op_ps){
                int ps_num;
                printf("[PROCESS TABLE]");
                ps_num = sys_process_show(ps_table);
                for(i=0; i<ps_num; i++){
                    check_add_cursor_y(&cursor_y);
                    sys_move_cursor(cursor_x, cursor_y);
                    switch(ps_table[i*3+1]){
                    case 0:
                        printf("[%d] PID: %d  STATUS: BLOCKED  PRIORITY: %d", i+1, ps_table[i*3], ps_table[i*3+2]);
                        break;
                    case 1:
                        printf("[%d] PID: %d  STATUS: RUNNING  PRIORITY: %d", i+1, ps_table[i*3], ps_table[i*3+2]);
                        break;
                    case 2:
                        printf("[%d] PID: %d  STATUS: READY    PRIORITY: %d", i+1, ps_table[i*3], ps_table[i*3+2]);
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
            else if(op_mkfs){
                int ret = sys_mkfs();
                if(ret == 0){
                    cursor_y = 29;
                }
                else{
                    cursor_y = 17;
                }
                continue;
            }
            else if(op_statfs){
                int ret = sys_print_fs();
                if(ret == 0){
                    cursor_y = 24;
                }
                else{
                    cursor_y = 17;
                }
                continue;
            }
            else if(op_clrfs){
                sys_clrfs();
                cursor_y = 17;
                continue;
            }
            else if(op_ls){
                cursor_y = sys_readdir(NULL);
                continue;
            }
            else if(op_exec || op_kill){
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
                if(flag && number>=0 && number<=1){
                    int pid;
                    pid = (int)sys_spawn(number, 0, 0);
                    if(pid > 0)
                        printf("exec process[%d], pid=%d", number, pid);
                    else
                        printf("fail, memory is not enough");
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
            else if(op_ls){
                cursor_y = sys_readdir(&buff[op_begin[1]]);
                continue;
            }
            else if(op_cd){
                int ret = sys_enterdir(&buff[op_begin[1]], &remind_str[16]);
                if(ret == 0){
                    printf("cd: succeed.");
                    x_begin = strlen(remind_str);
                }
                else if(ret == 1){
                    printf("cd: fail, the path has two consecutive slashes.");
                }
                else if(ret == 2){
                    printf("cd: fail, the path is too long.");
                }
                else if(ret == -1){
                    printf("cd: fail, filesystem has not set up.");
                }
                else{
                    printf("cd: fail, the path does not exist.(%d)", ret);
                }
                
            }
            else if(op_mkdir){
                int ret = sys_mkdir(&buff[op_begin[1]]);
                if(ret == 0){
                    printf("mkdir: succeed.");
                }
                else if(ret == 1){
                    printf("mkdir: fail, the path has two consecutive slashes.");
                }
                else if(ret == 2){
                    printf("mkdir: fail, the path is too long.");
                }
                else if(ret == -1){
                    printf("mkdir: fail, filesystem has not set up.");
                }
                else if(ret == 7){
                    printf("mkdir: fail, already have the same dir.");
                }
                else{
                    printf("mkdir: fail, the path does not exist.(%d)", ret);
                }
            }
            else if(op_rmdir){
                int ret = sys_rmdir(&buff[op_begin[1]]);
                if(ret == 0){
                    printf("rmdir: succeed.");
                }
                else if(ret == 1){
                    printf("rmdir: fail, the path has two consecutive slashes.");
                }
                else if(ret == 2){
                    printf("rmdir: fail, the path is too long.");
                }
                else if(ret == 9){
                    printf("rmdir: fail, the dir is not a leaf dir.");
                }
                else if(ret == -1){
                    printf("rmdir: fail, filesystem has not set up.");
                }
                else{
                    printf("rmdir: fail, the path does not exist.(%d)", ret);
                }
            }
            else if(op_touch){
                int ret = sys_mknod(&buff[op_begin[1]]);
                if(ret == 0){
                    printf("touch: succeed.");
                }
                else if(ret == 1){
                    printf("touch: fail, the path has two consecutive slashes.");
                }
                else if(ret == 2){
                    printf("touch: fail, the path is too long.");
                }
                else if(ret == -1){
                    printf("touch: fail, filesystem has not set up.");
                }
                else if(ret == 7){
                    printf("touch: fail, already have the same file.");
                }
                else{
                    printf("touch: fail, the path does not exist.(%d)", ret);
                }
            }
            else if(op_cat){
                int ret = sys_cat(&buff[op_begin[1]]);
                if(ret == 0){
                    printf("cat: succeed.");
                }
                else if(ret == -1){
                    printf("cat: fail, filesystem has not set up.");
                }
                else if(ret < 10){
                    printf("cat: fail, the file does not exist.(%d)", ret);
                }
                else{
                    printf("cat: fail, the dest is a link, but the link file does not exist.(%d)", ret);
                }
            }
            else if(op_ps || op_clear){
                printf("%s: too many arguments", &buff[op_begin[0]]);
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else if(op_num == 3){
            if(op_ln){
                buff[op_end[1]] = '\0';
                buff[op_end[2]] = '\0';
                int ret = sys_hard_link(&buff[op_begin[1]], &buff[op_begin[2]]);
                if(ret == 0){
                    printf("ln: succeed.");
                }
                else if(ret == -1){
                    printf("ln: fail, filesystem has not set up.");
                }
                else if(ret < 10){
                    printf("ln: fail, the source file does not exist.(%d)", ret);
                }
                else{
                    printf("ln: fail, the dest path does not exist.(%d)", ret);
                }
            }
            else{
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else if(op_num == 4){
            buff[op_end[1]] = '\0';
            int op_s = (strcmp(&buff[op_begin[1]], "-s") == 0);
            if(op_ln && op_s){
                buff[op_end[2]] = '\0';
                buff[op_end[3]] = '\0';
                int ret = sys_soft_link(&buff[op_begin[2]], &buff[op_begin[3]]);
                if(ret == 0){
                    printf("ln -s: succeed.");
                }
                else if(ret == -1){
                    printf("ln -s: fail, filesystem has not set up.");
                }
                else{
                    printf("ln -s: fail, the dest path does not exist.(%d)", ret);
                }
            }
            else{
                buff[op_end[1]] = ' ';
                printf("unknown command: '%s'", &buff[op_begin[0]]);
            }
        }
        else{
            printf("unknown command");
        }
        check_add_cursor_y(&cursor_y);
    }
}
