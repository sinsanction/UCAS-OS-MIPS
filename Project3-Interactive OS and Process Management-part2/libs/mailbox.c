#include "string.h"
#include "mailbox.h"

#define MAX_NUM_BOX 32

mailbox_t mboxs[MAX_NUM_BOX];
lock_id_t alloc_lock = 31;
condition_t alloc_cond;

static int mbox_is_full(mailbox_id_t boxid, int msg_length)
{
    if(mboxs[boxid].msg_ptr + msg_length > MAX_MBOX_LENGTH)
        return 1;
    else
        return 0;
}

static int mbox_is_empty(mailbox_id_t boxid, int msg_length)
{
    if(mboxs[boxid].msg_ptr - msg_length < 0)
        return 1;
    else
        return 0;
}

void do_mbox_init(mailbox_id_t boxid)
{
    mboxs[boxid].msg_ptr = 0;
    mboxs[boxid].open_num = 0;
}

int mini_hash(uint64_t x)
{
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ul;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebul;
    x = x ^ (x >> 31);
    return x % MAX_NUM_BOX;
}

int do_mbox_open(char *name)
{
    int sum = 0;
    int boxid;
    while(*name != '\0'){
        sum += (int)(*name);
        name++;
    }
    boxid = mini_hash((uint64_t)sum);
    mboxs[boxid].open_num++;
    return boxid;
}

void do_mbox_close(mailbox_id_t boxid)
{
    mboxs[boxid].open_num--;
    if(mboxs[boxid].open_num == 0){
        memset(mboxs[boxid].msg, 0, sizeof(mboxs[boxid].msg));
    }
}

void do_mbox_send(mailbox_id_t boxid, void *msg, int msg_length)
{
    do_mutex_lock_acquire(alloc_lock);

    while(mbox_is_full(boxid, msg_length)){
        do_condition_wait(alloc_lock, &alloc_cond);
    }

    memcpy(&(mboxs[boxid].msg[mboxs[boxid].msg_ptr]), msg, msg_length);
    mboxs[boxid].msg_ptr += msg_length;

    do_mutex_lock_release(alloc_lock);
    do_condition_broadcast(&alloc_cond);
}

void do_mbox_recv(mailbox_id_t boxid, void *msg, int msg_length)
{
    do_mutex_lock_acquire(alloc_lock);

    while(mbox_is_empty(boxid, msg_length)){
        do_condition_wait(alloc_lock, &alloc_cond);
    }

    mboxs[boxid].msg_ptr -= msg_length;
    memcpy(msg, &(mboxs[boxid].msg[mboxs[boxid].msg_ptr]), msg_length);

    do_mutex_lock_release(alloc_lock);
    do_condition_broadcast(&alloc_cond);
}
