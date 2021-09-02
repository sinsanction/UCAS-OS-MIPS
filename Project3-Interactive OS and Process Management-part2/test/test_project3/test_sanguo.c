#include "lock.h"
#include "mailbox.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "test3.h"
#include "time.h"

struct task_info sq_task;

struct task_info lb_task;

struct task_info cc_task;

void SunQuan(void)
{
    mailbox_id_t pub = mbox_open("SunQuan-Publish-PID");
    pid_t myPid = sys_getpid();
    strcpy(lb_task.name, "LiuBei");
    lb_task.entry_point = (uint64_t)&LiuBei;
    lb_task.type = USER_PROCESS;
    lb_task.priority = 1;
    /* Send PID twice, once for LiuBei,
     * and once for the CaoCao */

    // sys_move_cursor(0, 0);
    // printf("SunQuan(%d): Hello, I am SunQuan          ", myPid);

    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    /* Find LiuBei's PID */
    mailbox_id_t sub = mbox_open("LiuBei-Publish-PID");

    for (;;)
    {
        pid_t liubei;

        sys_move_cursor(0, 1);
        printf("[SunQuan](%d): Where are you Liubei?                ", myPid);
        mbox_recv(sub, &liubei, sizeof(pid_t));

        sys_move_cursor(0, 1);
        printf("[SunQuan](%d): I'm waiting for Liubei (%d)          ", myPid, liubei);
        sys_waitpid(liubei);

        sys_move_cursor(0, 1);
        printf("[SunQuan](%d): I'm coming to save you, LiuBei!", myPid);

        sys_sleep(1);
        sys_spawn(&lb_task, 2);
        mbox_send(pub, &myPid, sizeof(pid_t));
    }
}

void LiuBei(void)
{
    mailbox_id_t pub = mbox_open("LiuBei-Publish-PID");
    pid_t myPid = sys_getpid();

    strcpy(sq_task.name, "SunQuan");
    sq_task.entry_point = (uint64_t)&SunQuan;
    sq_task.type = USER_PROCESS;
    sq_task.priority = 1;
    /* Send PID twice, once for sunquan Hood,
     * and once for the CaoCao */
    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    /* Find sunquan's PID */
    mailbox_id_t sub = mbox_open("SunQuan-Publish-PID");

    // sys_move_cursor(0, 1);
    // printf("LiuBei(%d): Hello, I am Liubei          ", myPid);

    for (;;)
    {
        pid_t aramis;

        sys_move_cursor(0, 2);
        printf("[LiuBei](%d): Where are you SunQuan?          ", myPid);
        mbox_recv(sub, &aramis, sizeof(pid_t));

        sys_move_cursor(0, 2);
        printf("[LiuBei](%d): I'm waiting for SunQuan (%d)    ", myPid, aramis);
        sys_waitpid(aramis);

        sys_move_cursor(0, 2);
        printf("[LiuBei](%d): I'm coming to save you, SunQuan!", myPid);

        sys_sleep(1);
        sys_spawn(&sq_task, 2);
        mbox_send(pub, &myPid, sizeof(pid_t));
    }
}

void CaoCao(void)
{
    uint32_t myRand;
    pid_t myPid = sys_getpid();

    mailbox_id_t subSunQuan = mbox_open("SunQuan-Publish-PID");
    mailbox_id_t subLiuBei = mbox_open("LiuBei-Publish-PID");

    int i;
    pid_t sunquan, liubei;
    mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));
    mbox_recv(subLiuBei, &liubei, sizeof(pid_t));

    for (i = 0;; i++)
    {
        sys_move_cursor(0, 3);
        printf("[CaoCao](%d): I am working... muahaha ", myPid);

        sys_sleep(5);

        sys_move_cursor(0, 4);
        printf("[CaoCao](%d): I have my decision! ", myPid);

        switch (i % 2)
        {
        case 0:
            sys_move_cursor(0, 5);
            printf("[CaoCao](%d): I will kill SunQuan (%d)!  ", myPid, sunquan);
            sys_sleep(1);

            sys_move_cursor(0, 6);
            printf("[CaoCao]biu biu biu ~~~~~~ AAAAAAAA SunQuan (%d) is dead QAQ.", sunquan);
            sys_kill(sunquan);
            mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));

            sys_move_cursor(0, 7);
            printf("[CaoCao](%d): Oops! SunQuan(%d) lives!                 ", myPid, sunquan);
            break;
        case 1:
            sys_move_cursor(0, 5);
            printf("[CaoCao](%d): I will kill LiuBei(%d)! ", myPid, liubei);
            sys_sleep(1);

            sys_move_cursor(0, 6);
            printf("[CaoCao]biu biu biu ~~~~~~ AAAAAAAA Liubei (%d) is dead QAQ.", liubei);
            sys_kill(liubei);

            sys_move_cursor(0, 7);
            mbox_recv(subLiuBei, &liubei, sizeof(pid_t));
            printf("[CaoCao](%d): Oops! LiuBei(%d) is alive again! ", myPid, liubei);
            break;
        }

        sys_sleep(2);
    }
}
