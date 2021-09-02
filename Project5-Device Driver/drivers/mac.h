#ifndef INCLUDE_MAC_H_
#define INCLUDE_MAC_H_

#include "type.h"
#include "queue.h"
//#define TEST_REGS1
//#define TEST_REGS2
#define TEST_REGS3
#define GMAC_BASE_ADDR (0xffffffffc0040000)
#define DMA_BASE_ADDR (0xffffffffc0041000)
#define PSIZE (32)
#define PNUM (10)

#define SIZE_SHM (4096)
/* DMA Normal interrupt */
#define DMA_INTR_ENA_NIE 0x00010000 /* Normal Summary */
#define DMA_INTR_ENA_TIE 0x00000001 /* Transmit Interrupt */
#define DMA_INTR_ENA_TUE 0x00000004 /* Transmit Buffer Unavailable */
#define DMA_INTR_ENA_RIE 0x00000040 /* Receive Interrupt */
#define DMA_INTR_ENA_ERE 0x00004000 /* Early Receive */

/* DMA Abnormal interrupt */
#define DMA_INTR_ENA_AIE 0x00008000 /* Abnormal Summary */
#define DMA_INTR_ENA_FBE 0x00002000 /* Fatal Bus Error */
#define DMA_INTR_ENA_ETE 0x00000400 /* Early Transmit */
#define DMA_INTR_ENA_RWE 0x00000200 /* Receive Watchdog */
#define DMA_INTR_ENA_RSE 0x00000100 /* Receive Stopped */
#define DMA_INTR_ENA_RUE 0x00000080 /* Receive Buffer Unavailable */
#define DMA_INTR_ENA_UNE 0x00000020 /* Tx Underflow */
#define DMA_INTR_ENA_OVE 0x00000010 /* Receive Overflow */
#define DMA_INTR_ENA_TJE 0x00000008 /* Transmit Jabber */
#define DMA_INTR_ENA_TSE 0x00000002 /* Transmit Stopped */

#define DMA_INTR_ABNORMAL (DMA_INTR_ENA_AIE | DMA_INTR_ENA_FBE | \
                           DMA_INTR_ENA_UNE)

#define DMA_INTR_NORMAL (DMA_INTR_ENA_NIE | DMA_INTR_ENA_RIE | \
                         DMA_INTR_ENA_TIE)

/* DMA default interrupt mask */
#define DMA_INTR_DEFAULT_MASK (DMA_INTR_NORMAL | DMA_INTR_ABNORMAL)

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

/* Basic descriptor structure for normal and alternate descriptors */
typedef struct dma_desc
{
    union {
        /* Receive descriptor */
        struct
        {
            /* RDES0 */
            uint32_t payload_csum_error : 1;
            uint32_t crc_error : 1;
            uint32_t dribbling : 1;
            uint32_t mii_error : 1;
            uint32_t receive_watchdog : 1;
            uint32_t frame_type : 1;
            uint32_t collision : 1;
            uint32_t ipc_csum_error : 1;
            uint32_t last_descriptor : 1;
            uint32_t first_descriptor : 1;
            uint32_t vlan_tag : 1;
            uint32_t overflow_error : 1;
            uint32_t length_error : 1;
            uint32_t sa_filter_fail : 1;
            uint32_t descriptor_error : 1;
            uint32_t error_summary : 1;
            uint32_t frame_length : 14;
            uint32_t da_filter_fail : 1;
            uint32_t own : 1;
            /* RDES1 */
            uint32_t buffer1_size : 11;
            uint32_t buffer2_size : 11;
            uint32_t reserved1 : 2;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t reserved2 : 5;
            uint32_t disable_ic : 1;

        } rx;
        struct
        {
            /* RDES0 */
            uint32_t rx_mac_addr : 1;
            uint32_t crc_error : 1;
            uint32_t dribbling : 1;
            uint32_t error_gmii : 1;
            uint32_t receive_watchdog : 1;
            uint32_t frame_type : 1;
            uint32_t late_collision : 1;
            uint32_t ipc_csum_error : 1;
            uint32_t last_descriptor : 1;
            uint32_t first_descriptor : 1;
            uint32_t vlan_tag : 1;
            uint32_t overflow_error : 1;
            uint32_t length_error : 1;
            uint32_t sa_filter_fail : 1;
            uint32_t descriptor_error : 1;
            uint32_t error_summary : 1;
            uint32_t frame_length : 14;
            uint32_t da_filter_fail : 1;
            uint32_t own : 1;
            /* RDES1 */
            uint32_t buffer1_size : 13;
            uint32_t reserved1 : 1;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t buffer2_size : 13;
            uint32_t reserved2 : 2;
            uint32_t disable_ic : 1;
        } erx; /* -- enhanced -- */

        /* Transmit descriptor */
        struct
        {
            /* TDES0 */
            uint32_t deferred : 1;
            uint32_t underflow_error : 1;
            uint32_t excessive_deferral : 1;
            uint32_t collision_count : 4;
            uint32_t vlan_frame : 1;
            uint32_t excessive_collisions : 1;
            uint32_t late_collision : 1;
            uint32_t no_carrier : 1;
            uint32_t loss_carrier : 1;
            uint32_t payload_error : 1;
            uint32_t frame_flushed : 1;
            uint32_t jabber_timeout : 1;
            uint32_t error_summary : 1;
            uint32_t ip_header_error : 1;
            uint32_t time_stamp_status : 1;
            uint32_t reserved1 : 13;
            uint32_t own : 1;
            /* TDES1 */
            uint32_t buffer1_size : 11;
            uint32_t buffer2_size : 11;
            uint32_t time_stamp_enable : 1;
            uint32_t disable_padding : 1;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t crc_disable : 1;
            uint32_t checksum_insertion : 2;
            uint32_t first_segment : 1;
            uint32_t last_segment : 1;
            uint32_t interrupt : 1;
        } tx;
        struct
        {
            /* TDES0 */
            uint32_t deferred : 1;
            uint32_t underflow_error : 1;
            uint32_t excessive_deferral : 1;
            uint32_t collision_count : 4;
            uint32_t vlan_frame : 1;
            uint32_t excessive_collisions : 1;
            uint32_t late_collision : 1;
            uint32_t no_carrier : 1;
            uint32_t loss_carrier : 1;
            uint32_t payload_error : 1;
            uint32_t frame_flushed : 1;
            uint32_t jabber_timeout : 1;
            uint32_t error_summary : 1;
            uint32_t ip_header_error : 1;
            uint32_t time_stamp_status : 1;
            uint32_t reserved1 : 2;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t checksum_insertion : 2;
            uint32_t reserved2 : 1;
            uint32_t time_stamp_enable : 1;
            uint32_t disable_padding : 1;
            uint32_t crc_disable : 1;
            uint32_t first_segment : 1;
            uint32_t last_segment : 1;
            uint32_t interrupt : 1;
            uint32_t own : 1;
            /* TDES1 */
            uint32_t buffer1_size : 13;
            uint32_t reserved3 : 3;
            uint32_t buffer2_size : 13;
            uint32_t reserved4 : 3;
        } etx; /* -- enhanced -- */

        struct
        {
            /* TDES0 */
            uint32_t deferred : 1;
            uint32_t underflow_error : 1;
            uint32_t excessive_deferral : 1;
            uint32_t collision_count : 4;
            uint32_t vlan_frame : 1;
            uint32_t excessive_collisions : 1;
            uint32_t late_collision : 1;
            uint32_t no_carrier : 1;
            uint32_t loss_carrier : 1;
            uint32_t payload_error : 1;
            uint32_t frame_flushed : 1;
            uint32_t jabber_timeout : 1;
            uint32_t error_summary : 1;
            uint32_t ip_header_error : 1;
            uint32_t time_stamp_status : 1;
            uint32_t reserved1 : 2;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t checksum_insertion : 2;
            uint32_t reserved2 : 1;
            uint32_t time_stamp_enable : 1;
            uint32_t disable_padding : 1;
            uint32_t crc_disable : 1;
            uint32_t first_segment : 1;
            uint32_t last_segment : 1;
            uint32_t interrupt : 1;
            uint32_t own : 1;
            /* TDES1 */
            uint32_t buffer1_size : 14;
            uint32_t reserved3 : 1;
            uint32_t buffer2_size : 14;
            uint32_t ctrl : 3;
        } etx64; /* -- enhanced -- */
        struct
        {
            /* RDES0 */
            uint32_t rx_mac_addr : 1;
            uint32_t crc_error : 1;
            uint32_t dribbling : 1;
            uint32_t error_gmii : 1;
            uint32_t receive_watchdog : 1;
            uint32_t frame_type : 1;
            uint32_t late_collision : 1;
            uint32_t ipc_csum_error : 1;
            uint32_t last_descriptor : 1;
            uint32_t first_descriptor : 1;
            uint32_t vlan_tag : 1;
            uint32_t overflow_error : 1;
            uint32_t length_error : 1;
            uint32_t sa_filter_fail : 1;
            uint32_t descriptor_error : 1;
            uint32_t error_summary : 1;
            uint32_t frame_length : 14;
            uint32_t da_filter_fail : 1;
            uint32_t own : 1;
            /* RDES1 */
            uint32_t buffer1_size : 14;
            uint32_t second_address_chained : 1;
            uint32_t end_ring : 1;
            uint32_t buffer2_size : 14;
            uint32_t reserved2 : 1;
            uint32_t disable_ic : 1;
        } erx64; /* -- enhanced -- */
    } des01;
    unsigned int des2;
    unsigned int des3;
    unsigned int des4;
    unsigned int des5;
    unsigned int des6;
    unsigned int des7;
} dma_desc_t;

typedef struct desc
{
#ifdef COMPLEX_DESC
    struct dma_desc basic;
    union {
        struct
        {
            uint32_t ip_payload_type : 3;
            uint32_t ip_hdr_err : 1;
            uint32_t ip_payload_err : 1;
            uint32_t ip_csum_bypassed : 1;
            uint32_t ipv4_pkt_rcvd : 1;
            uint32_t ipv6_pkt_rcvd : 1;
            uint32_t msg_type : 4;
            uint32_t ptp_frame_type : 1;
            uint32_t ptp_ver : 1;
            uint32_t timestamp_dropped : 1;
            uint32_t reserved : 1;
            uint32_t av_pkt_rcvd : 1;
            uint32_t av_tagged_pkt_rcvd : 1;
            uint32_t vlan_tag_priority_val : 3;
            uint32_t reserved3 : 3;
            uint32_t l3_filter_match : 1;
            uint32_t l4_filter_match : 1;
            uint32_t l3_l4_filter_no_match : 2;
            uint32_t reserved4 : 4;
        } erx;
        struct
        {
            uint32_t reserved;
        } etx;
    } des4;
    unsigned int des5; /* Reserved */
    unsigned int des6; /* Tx/Rx Timestamp Low */
    unsigned int des7; /* Tx/Rx Timestamp High */

#else
    uint32_t tdes0;
    uint32_t tdes1;
    uint32_t tdes2;
    uint32_t tdes3;
    uint32_t tdes4;
    uint32_t tdes5;
    uint32_t tdes6;
    uint32_t tdes7;
#endif
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

typedef struct recv_arg
{
    uint64_t buf_addr;
    uint64_t size;
    uint64_t num;
    uint64_t length;

} recv_arg_t;

extern dma_desc_t *recv_desc;
extern dma_desc_t *send_desc;

void set_mac_int();
uint32_t read_register(uint64_t base, uint64_t offset);
void reg_write_32(uint64_t addr, uint32_t data);
void irq_enable(int IRQn);
void print_rx_dscrb(mac_t *mac);
void print_tx_dscrb(mac_t *mac);
uint32_t do_net_recv(recv_arg_t *recv_reg);
void do_net_send(uint64_t buf_addr, uint64_t size, uint64_t num);
void do_init_mac(void);
void do_wait_recv_package(void);
void mac_irq_handle(void);
void mac_recv_handle(mac_t *test_mac);
void clear_interrupt();
uint32_t printf_recv_buffer(uint64_t);
#endif
