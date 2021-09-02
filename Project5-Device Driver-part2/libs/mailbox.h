#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include "type.h"
#include "sync.h"

#define MAX_MBOX_LENGTH (64)

typedef struct mailbox
{
    char msg[MAX_MBOX_LENGTH];
    int msg_ptr;
    int open_num;

} mailbox_t;

typedef int mailbox_id_t;

void do_mbox_init(mailbox_id_t);
int do_mbox_open(char *);
void do_mbox_close(mailbox_id_t);
void do_mbox_send(mailbox_id_t, void *, int);
void do_mbox_recv(mailbox_id_t, void *, int);

#endif