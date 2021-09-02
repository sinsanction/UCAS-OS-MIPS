#include "irq.h"
#include "mac.h"
#include "sched.h"
#include "screen.h"
#include "syscall.h"
#include "test5.h"
#include "type.h"
uint32_t buffer[PSIZE] = {0xffffffff, 0x5500ffff, 0xf77db57d, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0004e914, 0x0000, 0x005e0001, 0x2300fb00, 0x84b7f28b, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0801e914, 0x0000};

static void init_data(uint32_t *addr)
{
}
static void mac_send_desc_init(mac_t *mac)
{
}

static void mac_recv_desc_init(mac_t *mac)
{
}

static void mii_dul_force(mac_t *mac)
{
    reg_write_32(mac->dma_addr, 0x80);

    uint32_t conf = 0xc800;

    // loopback, 100M
    reg_write_32(mac->mac_addr, reg_read_32(mac->mac_addr) | (conf) | (1 << 8));
    //enable recieve all

    reg_write_32(mac->mac_addr + 0x4, 0x80000001);
}

void dma_control_init(mac_t *mac, uint32_t init_value)
{
    reg_write_32(mac->dma_addr + DmaControl, init_value);
    return;
}

void mac_send_task()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t print_location = 1;

    test_mac.mac_addr = GMAC_BASE_ADDR;
    test_mac.dma_addr = DMA_BASE_ADDR;

    test_mac.psize = PSIZE * 4; // 1024bytes
    test_mac.pnum = PNUM;       // pnum

    mac_send_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);
#ifdef MAC_INT
    register_irq_handler(12, mac_irq_handle);
    irq_enable(12);
    set_mac_int();
#endif
    sys_move_cursor(1, print_location);
    printf("> [MAC SEND TASK] start send package.               \n");

    uint32_t cnt = 0;
    i = 4;
    uint32_t *send_desc;
    while (i > 0)
    {
        sys_net_send(test_mac.td, test_mac.td_phy);
        cnt += PNUM;
        sys_move_cursor(1, print_location);
        printf("> [MAC SEND TASK] totally send package %d !        \n", cnt);
        i--;
    }
    send_desc = (uint32_t *)test_mac.td;
    if (((*send_desc) & 0x80000000) == 0x80000000)
    {
        printf("\nsend error\n");
    }

    sys_exit();
}

void mac_recv_task()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t ret;
    uint32_t print_location = 10;

    test_mac.mac_addr = GMAC_BASE_ADDR;
    test_mac.dma_addr = DMA_BASE_ADDR;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum
    mac_recv_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);
#ifdef MAC_INT
    register_irq_handler(12, mac_irq_handle);
    irq_enable(12);
    set_mac_int();
    ch_flag = 0;
    for (i = 0; i < PNUM; i++)
    {
        recv_flag[i] = 0;
    }
#endif
    queue_init(&recv_block_queue);
    sys_move_cursor(1, print_location);
    printf("[MAC RECV TASK] start recv:                    ");
    ret = sys_net_recv(test_mac.rd, test_mac.rd_phy, test_mac.daddr);
#ifdef MAC_INT

    uint32_t cnt = 0;
    uint32_t *Recv_desc;
    Recv_desc = (uint32_t *)(test_mac.rd + (PNUM - 1) * 32);
    if (((*Recv_desc) & 0x80000000) == 0x80000000)
    {
        sys_move_cursor(1, print_location);
        printf("> [MAC RECV TASK] waiting receive package.\n");
        sys_wait_recv_package();
    }
    mac_recv_handle(&test_mac);
#endif
#ifdef MAC_POLLING
    if (ret == 0)
    {
        sys_move_cursor(1, print_location);
        printf("[MAC RECV TASK]     net recv is ok!                          ");
    }
    else
    {
        sys_move_cursor(1, print_location);
        printf("[MAC RECV TASK]     net recv is fault!                       ");
    }

#endif
    sys_exit();
}
