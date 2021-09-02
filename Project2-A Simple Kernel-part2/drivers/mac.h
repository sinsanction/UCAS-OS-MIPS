#ifndef INCLUDE_MAC_H_
#define INCLUDE_MAC_H_

#include "type.h"
#include "queue.h"
//#define TEST_REGS1
//#define TEST_REGS2
//#define TEST_REGS3
#define GMAC_BASE_ADDR (0xffffffffc0040000)
#define DMA_BASE_ADDR (0xffffffffc0041000)
#define PSIZE (64)
#define PNUM (64)

extern queue_t recv_block_queue;
extern uint32_t recv_flag[PNUM];
extern uint32_t ch_flag;
/*
enum GmacRegisters
{
    GmacAddr0Low = 0x0044,  /* Mac frame filtering controls */
//GmacAddr0High = 0x0040, /* Mac address0 high Register  */
//};
enum GmacRegisters
{
    GmacConfig = 0x0000,      /* Mac config Register                       */
    GmacFrameFilter = 0x0004, /* Mac frame filtering controls              */
    GmacHashHigh = 0x0008,    /* Multi-cast hash table high                */
    GmacHashLow = 0x000C,     /* Multi-cast hash table low                 */
    GmacGmiiAddr = 0x0010,    /* GMII address Register(ext. Phy)           */
    GmacGmiiData = 0x0014,    /* GMII data Register(ext. Phy)              */
    GmacFlowControl = 0x0018, /* Flow control Register                     */
    GmacVlan = 0x001C,        /* VLAN tag Register (IEEE 802.1Q)           */

    GmacVersion = 0x0020,       /* GMAC Core Version Register                */
    GmacWakeupAddr = 0x0028,    /* GMAC wake-up frame filter adrress reg     */
    GmacPmtCtrlStatus = 0x002C, /* PMT control and status register           */

    GmacInterruptStatus = 0x0038, /* Mac Interrupt ststus register	       */
    GmacInterruptMask = 0x003C,   /* Mac Interrupt Mask register	       */

    GmacAddr0High = 0x0040,  /* Mac address0 high Register                */
    GmacAddr0Low = 0x0044,   /* Mac address0 low Register                 */
    GmacAddr1High = 0x0048,  /* Mac address1 high Register                */
    GmacAddr1Low = 0x004C,   /* Mac address1 low Register                 */
    GmacAddr2High = 0x0050,  /* Mac address2 high Register                */
    GmacAddr2Low = 0x0054,   /* Mac address2 low Register                 */
    GmacAddr3High = 0x0058,  /* Mac address3 high Register                */
    GmacAddr3Low = 0x005C,   /* Mac address3 low Register                 */
    GmacAddr4High = 0x0060,  /* Mac address4 high Register                */
    GmacAddr4Low = 0x0064,   /* Mac address4 low Register                 */
    GmacAddr5High = 0x0068,  /* Mac address5 high Register                */
    GmacAddr5Low = 0x006C,   /* Mac address5 low Register                 */
    GmacAddr6High = 0x0070,  /* Mac address6 high Register                */
    GmacAddr6Low = 0x0074,   /* Mac address6 low Register                 */
    GmacAddr7High = 0x0078,  /* Mac address7 high Register                */
    GmacAddr7Low = 0x007C,   /* Mac address7 low Register                 */
    GmacAddr8High = 0x0080,  /* Mac address8 high Register                */
    GmacAddr8Low = 0x0084,   /* Mac address8 low Register                 */
    GmacAddr9High = 0x0088,  /* Mac address9 high Register                */
    GmacAddr9Low = 0x008C,   /* Mac address9 low Register                 */
    GmacAddr10High = 0x0090, /* Mac address10 high Register               */
    GmacAddr10Low = 0x0094,  /* Mac address10 low Register                */
    GmacAddr11High = 0x0098, /* Mac address11 high Register               */
    GmacAddr11Low = 0x009C,  /* Mac address11 low Register                */
    GmacAddr12High = 0x00A0, /* Mac address12 high Register               */
    GmacAddr12Low = 0x00A4,  /* Mac address12 low Register                */
    GmacAddr13High = 0x00A8, /* Mac address13 high Register               */
    GmacAddr13Low = 0x00AC,  /* Mac address13 low Register                */
    GmacAddr14High = 0x00B0, /* Mac address14 high Register               */
    GmacAddr14Low = 0x00B4,  /* Mac address14 low Register                */
    GmacAddr15High = 0x00B8, /* Mac address15 high Register               */
    GmacAddr15Low = 0x00BC,  /* Mac address15 low Register                */

    GmacMiiStatus = 0x00D8,
    /*Time Stamp Register Map*/
    GmacTSControl = 0x0700, /* Controls the Timestamp update logic                         : only when IEEE 1588 time stamping is enabled in corekit            */

    GmacTSSubSecIncr = 0x0704, /* 8 bit value by which sub second register is incremented     : only when IEEE 1588 time stamping without external timestamp input */

    GmacTSHigh = 0x0708, /* 32 bit seconds(MS)                                          : only when IEEE 1588 time stamping without external timestamp input */
    GmacTSLow = 0x070C,  /* 32 bit nano seconds(MS)                                     : only when IEEE 1588 time stamping without external timestamp input */

    GmacTSHighUpdate = 0x0710, /* 32 bit seconds(MS) to be written/added/subtracted           : only when IEEE 1588 time stamping without external timestamp input */
    GmacTSLowUpdate = 0x0714,  /* 32 bit nano seconds(MS) to be writeen/added/subtracted      : only when IEEE 1588 time stamping without external timestamp input */

    GmacTSAddend = 0x0718, /* Used by Software to readjust the clock frequency linearly   : only when IEEE 1588 time stamping without external timestamp input */

    GmacTSTargetTimeHigh = 0x071C, /* 32 bit seconds(MS) to be compared with system time          : only when IEEE 1588 time stamping without external timestamp input */
    GmacTSTargetTimeLow = 0x0720,  /* 32 bit nano seconds(MS) to be compared with system time     : only when IEEE 1588 time stamping without external timestamp input */

    GmacTSHighWord = 0x0724, /* Time Stamp Higher Word Register (Version 2 only); only lower 16 bits are valid                                                   */
    //GmacTSHighWordUpdate    = 0x072C,  /* Time Stamp Higher Word Update Register (Version 2 only); only lower 16 bits are valid                                            */

    GmacTSStatus = 0x0728, /* Time Stamp Status Register                                                                                                       */
};

enum DmaRegisters
{
    DmaBusMode = 0x0000,      /* CSR0 - Bus Mode Register                          */
    DmaTxPollDemand = 0x0004, /* CSR1 - Transmit Poll Demand Register              */
    DmaRxPollDemand = 0x0008, /* CSR2 - Receive Poll Demand Register               */
    DmaRxBaseAddr = 0x000C,   /* CSR3 - Receive Descriptor list base address       */
    DmaTxBaseAddr = 0x0010,   /* CSR4 - Transmit Descriptor list base address      */
    DmaStatus = 0x0014,       /* CSR5 - Dma status Register                        */
    DmaControl = 0x0018,      /* CSR6 - Dma Operation Mode Register                */
    DmaInterrupt = 0x001C,    /* CSR7 - Interrupt enable                           */
    DmaMissedFr = 0x0020,     /* CSR8 - Missed Frame & Buffer overflow Counter     */
    DmaTxCurrDesc = 0x0048,   /* CSR18 - Current host Tx Desc Register              */
    DmaRxCurrDesc = 0x004C,   /* CSR19 - Current host Rx Desc Register              */
    DmaTxCurrAddr = 0x0050,   /* CSR20 - Current host transmit buffer address      */
    DmaRxCurrAddr = 0x0054,   /* CSR21 - Current host receive buffer address       */
};

enum DmaControlReg
{

    DmaStoreAndForward = 0x00200000, /* (SF)Store and forward                            21      RW        0       */
    DmaRxThreshCtrl128 = 0x00000018, /* (RTC)Controls thre Threh of MTL tx Fifo 128      4:3   RW                */

    DmaTxStart = 0x00002000, /* (ST)Start/Stop transmission                      13      RW        0       */

    DmaTxSecondFrame = 0x00000004, /* (OSF)Operate on second frame                     4       RW        0       */
};
enum InitialRegisters
{
    DmaIntDisable = 0,
};
typedef struct desc
{
    uint32_t tdes0;
    uint32_t tdes1;
    uint32_t tdes2;
    uint32_t tdes3;
    uint32_t tdes4;
    uint32_t tdes5;
    uint32_t tdes6;
    uint32_t tdes7;
} desc_t;
typedef struct mac
{
    uint32_t psize; // backpack size
    uint32_t pnum;
    uint64_t mac_addr; // MAC base address
    uint64_t dma_addr; // DMA base address

    uint64_t saddr; // send address
    uint64_t daddr; // receive address

    uint64_t saddr_phy; // send phy address
    uint64_t daddr_phy; // receive phy address

    uint64_t td; // DMA send desc
    uint64_t rd; // DMA receive desc

    uint64_t td_phy;
    uint64_t rd_phy;

} mac_t;
void set_mac_int();
uint32_t read_register(uint64_t base, uint64_t offset);
void reg_write_32(uint64_t addr, uint32_t data);
void irq_enable(int IRQn);
void print_rx_dscrb(mac_t *mac);
void print_tx_dscrb(mac_t *mac);
uint32_t do_net_recv(uint64_t rd, uint64_t rd_phy, uint64_t daddr);
void do_net_send(uint64_t td, uint64_t td_phy);
void do_init_mac(void);
void do_wait_recv_package(void);
void mac_irq_handle(void);
void mac_recv_handle(mac_t *test_mac);
#endif
