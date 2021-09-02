#include "type.h"
#include "stdio.h"
#include "syscall.h"
#include "string.h"
#include "struct.h"

#define SHM_KEY 1
#define P5_C_CORE
#define FIFO_BUF_MAX (2048 * 4)
#define PSIZE (32)
#define PNUM (10)

#define MAX_RECV_CNT 64
char recv_buffer[MAX_RECV_CNT * PSIZE * 4];
uint64_t recv_length[MAX_RECV_CNT] = {0};

recv_arg_t recv_reg;

/* this should not exceed a page */
typedef struct echo_shm_vars
{
    uint64_t magic_number;
    uint64_t available;
    char fifo_buffer[FIFO_BUF_MAX];
} echo_shm_vars_t;

void shm_read(char *shmbuf, uint64_t *_available, char *buf, uint64_t size)
{
    while (size > 0)
    {
        while (*_available != 1)
            ;

        int sz = size > FIFO_BUF_MAX ? FIFO_BUF_MAX : size;
        memcpy(buf, shmbuf, sz);
        size -= sz;

        *_available = 0;
    }
}

void shm_write(char *shmbuf, uint64_t *_available, char *buf, uint64_t size)
{
    while (size > 0)
    {
        while (*_available != 0)
            ;

        int sz = size > FIFO_BUF_MAX ? FIFO_BUF_MAX : size;
        memcpy(shmbuf, buf, sz);
        size -= sz;

        *_available = 1;
    }
}

void __attribute__((section(".entry_function"))) _start(void)
{
    int mode = 0;
    int size = 64;
#ifdef P5_C_CORE
    uint64_t shmid;
    shmid = shmget(SHM_KEY);
    echo_shm_vars_t *vars = (echo_shm_vars_t *)shmat(shmid);
    if (vars == NULL)
    {
        sys_move_cursor(1, 1);
        printf("shmpageget failed!\n");
        sys_exit();
    }

    vars->available = 0;

    sys_move_cursor(1, 1);
    printf("[ECHO TASK] start recv(%d):                    \n", size);

    recv_reg.buf_addr = (uint64_t)recv_buffer;
    recv_reg.size = size * PSIZE;
    recv_reg.num = size;
    recv_reg.length = (uint64_t)recv_length;

    int ret = sys_net_recv((uint64_t)&recv_reg);
    shm_write(vars->fifo_buffer, &vars->available, recv_buffer, size * PSIZE * 4);
    shm_write(vars->fifo_buffer, &vars->available, (char *)recv_length, size * sizeof(uint64_t));

    shmdt((uint64_t)vars);
#else
    sys_move_cursor(1, 1);
    printf("[ECHO TASK] start recv(%d):                    \n", size);

    recv_reg.buf_addr = (uint64_t)recv_buffer;
    recv_reg.size = size * PSIZE;
    recv_reg.num = size;
    recv_reg.length = (uint64_t)recv_length;

    int ret = sys_net_recv((uint64_t)&recv_reg);
    int i;
    /*for(i=0; i<size; i++){
        sys_move_cursor(1, 3);
        printf("recv buffer [%d], r_desc: 0x%x", i, *(uint32_t*)&recv_desc[i]);
        sys_move_cursor(1, 4);
        printf_recv_buffer((uint64_t)&recv_buffer[i * PSIZE]);
    }*/

#endif
    sys_exit();
}
