#ifndef INCLUDE_TEST4_H_
#define INCLUDE_TEST4_H_
#include "queue.h"
#define MAC_POLLING

#define EPT_ARP 0x0608 /* type: ARP */

static void init_data(uint32_t *addr);
//void phy_regs_task(void);
#if 1
void mac_send_task(void);
void mac_recv_task(void);
void mac_init_task(void);
#endif
static void init_mac(void);
//extern uint32_t recv_flag[PNUM];
//extern uint32_t ch_flag ;
#endif