
#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   312
.equ    NESTED_COUNT, 624

 
LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    mfc0    t0, CP0_CONFIG
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG
    
    jr  ra
END(asm_start)

.macro SWITCH_STACK offset
    .set    noat
    dla   k0, current_running
    dmfc0 k1, $15, 1
    andi  k1, k1, 0x3ff
    dsll  k1, k1, 3
    daddu k1, k1, k0
    ld    k0, 0(k1)

    ld    k1, 0(k0)
    daddi k1, k1, -OFFSET_SIZE
    sd    k1, 0(k0)

    sd    sp, OFFSET_REG29(k1)
    #ld    sp, 8(k0)
    .set    at
.endm

.macro LOAD_CURRENT offset
    .set    noat
    dla   k0, current_running
    dmfc0 k1, $15, 1
    andi  k1, k1, 0x3ff
    dsll  k1, k1, 3
    daddu k1, k1, k0
    ld    k0, 0(k1)
    ld    k1, 0(k0)
    .set    at
.endm

.macro SAVE_CONTEXT offset
    .set    noat
    sd    $0, OFFSET_REG0(k1)
    sd    $1, OFFSET_REG1(k1)
    sd    $2, OFFSET_REG2(k1)
    sd    $3, OFFSET_REG3(k1)
    sd    $4, OFFSET_REG4(k1)
    sd    $5, OFFSET_REG5(k1)
    sd    $6, OFFSET_REG6(k1)
    sd    $7, OFFSET_REG7(k1)
    sd    $8, OFFSET_REG8(k1)
    sd    $9, OFFSET_REG9(k1)
    sd    $10, OFFSET_REG10(k1)
    sd    $11, OFFSET_REG11(k1)
    sd    $12, OFFSET_REG12(k1)
    sd    $13, OFFSET_REG13(k1)
    sd    $14, OFFSET_REG14(k1)
    sd    $15, OFFSET_REG15(k1)
    sd    $16, OFFSET_REG16(k1)
    sd    $17, OFFSET_REG17(k1)
    sd    $18, OFFSET_REG18(k1)
    sd    $19, OFFSET_REG19(k1)
    sd    $20, OFFSET_REG20(k1)
    sd    $21, OFFSET_REG21(k1)
    sd    $22, OFFSET_REG22(k1)
    sd    $23, OFFSET_REG23(k1)
    sd    $24, OFFSET_REG24(k1)
    sd    $25, OFFSET_REG25(k1)
    sd    $26, OFFSET_REG26(k1)
    sd    $27, OFFSET_REG27(k1)
    sd    $28, OFFSET_REG28(k1)
    sd    $30, OFFSET_REG30(k1)
    sd    $31, OFFSET_REG31(k1)
    dmfc0 t0, CP0_STATUS
    sd    t0, OFFSET_STATUS(k1)
    dmfc0 t0, CP0_CAUSE
    sd    t0, OFFSET_CAUSE(k1)
    dmfc0 t0, CP0_WATCHHI
    sd    t0, OFFSET_HI(k1)
    dmfc0 t0, CP0_WATCHLO
    sd    t0, OFFSET_LO(k1)
    dmfc0 t0, CP0_BADVADDR
    sd    t0, OFFSET_BADVADDR(k1)
    dmfc0 t0, CP0_EPC
    sd    t0, OFFSET_EPC(k1)
    sd    ra, OFFSET_PC(k1)

    ld    t0, 32(k0)
    sd    t0, OFFSET_PRE_STATE(k1)
    daddi t0, $0, 1
    sd    t0, 32(k0)
    .set    at
.endm

.macro RESTORE_CONTEXT offset
    .set    noat
    dla   k0, current_running
    dmfc0 k1, $15, 1
    andi  k1, k1, 0x3ff
    dsll  k1, k1, 3
    daddu k1, k1, k0
    ld    k0, 0(k1)

    ld    k1, 0(k0)
    daddi k1, k1, OFFSET_SIZE
    sd    k1, 0(k0)
    daddi k1, k1, -OFFSET_SIZE

    dmfc0 t1, CP0_ENTRYHI
    dli   t0, 0xffffffffffffff00
    and   t1, t1, t0
    lw    t0, 380(k0)
    or    t0, t1, t0
    dmtc0 t0, CP0_ENTRYHI

    ld    t0, OFFSET_PRE_STATE(k1)
    sd    t0, 32(k0)
    ld    $0, OFFSET_REG0(k1)
    ld    $1, OFFSET_REG1(k1)
    ld    $2, OFFSET_REG2(k1)
    ld    $3, OFFSET_REG3(k1)
    ld    $4, OFFSET_REG4(k1)
    ld    $5, OFFSET_REG5(k1)
    ld    $6, OFFSET_REG6(k1)
    ld    $7, OFFSET_REG7(k1)
    ld    $8, OFFSET_REG8(k1)
    ld    $9, OFFSET_REG9(k1)
    ld    $10, OFFSET_REG10(k1)
    ld    $11, OFFSET_REG11(k1)
    ld    $12, OFFSET_REG12(k1)
    ld    $13, OFFSET_REG13(k1)
    ld    $14, OFFSET_REG14(k1)
    ld    $15, OFFSET_REG15(k1)
    ld    $16, OFFSET_REG16(k1)
    ld    $17, OFFSET_REG17(k1)
    ld    $18, OFFSET_REG18(k1)
    ld    $19, OFFSET_REG19(k1)
    ld    $20, OFFSET_REG20(k1)
    ld    $21, OFFSET_REG21(k1)
    ld    $22, OFFSET_REG22(k1)
    ld    $23, OFFSET_REG23(k1)
    ld    $24, OFFSET_REG24(k1)
    ld    $25, OFFSET_REG25(k1)

    ld    $28, OFFSET_REG28(k1)
    ld    $29, OFFSET_REG29(k1)
    ld    $30, OFFSET_REG30(k1)
    ld    $31, OFFSET_REG31(k1)
    ld    k0, OFFSET_CAUSE(k1)
    dmtc0 k0, CP0_CAUSE
    ld    k0, OFFSET_HI(k1)
    dmtc0 k0, CP0_WATCHHI
    ld    k0, OFFSET_LO(k1)
    dmtc0 k0, CP0_WATCHLO
    ld    k0, OFFSET_BADVADDR(k1)
    dmtc0 k0, CP0_BADVADDR
    ld    k0, OFFSET_EPC(k1)
    dmtc0 k0, CP0_EPC
    ld    k0, OFFSET_STATUS(k1)
    dmtc0 k0, CP0_STATUS
    .set    at
.endm

# function do_scheduler
NESTED(do_scheduler, 0, ra)
    SWITCH_STACK
    SAVE_CONTEXT
    sd    ra, OFFSET_EPC(k1)
    jal   scheduler

    RESTORE_CONTEXT
    eret
END(do_scheduler)

NESTED(do_scheduler2, 0, ra)
    jal   scheduler
    RESTORE_CONTEXT
    eret
END(do_scheduler2)

LEAF(reset_timer)
    
END(reset_timer)

LEAF(set_cp0_status)
    dmtc0 a0, CP0_STATUS
    jr ra
END(set_cp0_status)

LEAF(set_cp0_cause)
    dmtc0 a0, CP0_CAUSE
    jr ra
END(set_cp0_cause)

LEAF(get_cp0_status)
    dmfc0 v0, CP0_STATUS
    jr ra
END(get_cp0_status)

LEAF(get_cp0_cause)
    dmfc0 v0, CP0_CAUSE
    jr ra
END(get_cp0_cause)

LEAF(enable_interrupt)
    dmfc0 t0, CP0_STATUS
    dli   t1, 0xffbf7ff8
    and   t0, t0, t1
    ori   t0, t0, 0x8001
    dmtc0 t0, CP0_STATUS
    jr ra
END(enable_interrupt)

LEAF(disable_interrupt)
    dmtc0 $0, CP0_STATUS
    jr ra
END(disable_interrupt)

LEAF(get_cp0_count)
    dmfc0 v0, CP0_COUNT
    jr ra 
END(get_cp0_count)

LEAF(get_cp0_compare)
	dmfc0 v0, CP0_COMPARE
    jr ra
END(get_cp0_compare)

LEAF(set_cp0_count)
    dmtc0 a0, CP0_COUNT
    jr ra
END(set_cp0_count)

LEAF(set_cp0_compare)
	dmtc0 a0, CP0_COMPARE
    jr ra
END(set_cp0_compare)

 
.global exception_handler_begin
.global exception_handler_end
.global TLBexception_handler_begin
.global TLBexception_handler_end

NESTED(TLBexception_handler_entry, 0, sp)
TLBexception_handler_begin:
    SWITCH_STACK
    ld    sp, 8(k0)
    SAVE_CONTEXT
    dla   k0, handle_refill
    jr    k0
TLBexception_handler_end:
END(TLBexception_handler_entry)

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    SWITCH_STACK
    ld    sp, 8(k0)
    SAVE_CONTEXT
    dmfc0 k0, CP0_CAUSE
    dsrl  k0, k0, 2
    andi  k0, k0, 0x1f
    dla   k1, exception_handler
    dsll  k0, k0, 3
    daddu k1, k1, k0
    ld    k0, 0(k1)
    jr    k0
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    dmfc0 a0, CP0_STATUS
    dmfc0 a1, CP0_CAUSE
    dmfc0 a2, CP0_EPC
    jal   interrupt_helper
    RESTORE_CONTEXT
    eret
END(handle_int)

NESTED(handle_syscall, 0, sp)
    LOAD_CURRENT
    ld    k0, OFFSET_EPC(k1)
    daddi k0, k0, 4
    sd    k0, OFFSET_EPC(k1)
    jal   system_call_helper

    LOAD_CURRENT
    sd    v0, OFFSET_REG2(k1)
    RESTORE_CONTEXT
    eret
END(handle_syscall)

NESTED(handle_refill, 0, sp)
    dmfc0 a0, CP0_ENTRYHI
    dmfc0 a1, CP0_CONTEXT
    jal   do_TLB_Refill
    RESTORE_CONTEXT
    eret
END(handle_refill)

NESTED(handle_tlb, 0, sp)
    dmfc0 a0, CP0_CAUSE
    dmfc0 a1, CP0_ENTRYHI
    dmfc0 a2, CP0_CONTEXT
    jal   tlb_exception_handler
    RESTORE_CONTEXT
    eret
END(handle_tlb)

NESTED(handle_other, 0, sp)
    dmfc0 a0, CP0_CAUSE
    dmfc0 a1, CP0_EPC
    dmfc0 a2, CP0_BADVADDR
    jal   other_exception_handler
END(handle_other)

LEAF(exception_handler_exit)
  
END(exception_handler_exit)

LEAF(print_error)
    dli t0, 0x123456
    jr  t0
END(print_error)

LEAF(get_reg_gp)
    daddi v0, gp, 0
    jr    ra
END(get_reg_gp)

LEAF(get_reg_sp)
    daddi v0, sp, 0
    jr    ra
END(get_reg_sp)

LEAF(set_a_tlb_entry)
    dmtc0 a0, CP0_ENTRYHI
    dmtc0 a1, CP0_ENTRYLO0
    dmtc0 a2, CP0_ENTRYLO1
    dli   k0, 0
    dmtc0 k0, CP0_PAGEMASK
    mtc0  a3, CP0_INDEX
    tlbwi
    jr    ra
END(set_a_tlb_entry)

LEAF(set_cp0_entryhi)
    dmtc0 a0, CP0_ENTRYHI
    jr    ra
END(set_cp0_entryhi)

LEAF(check_tlb)
    dmtc0 a0, CP0_ENTRYHI
    tlbp
    mfc0  v0, CP0_INDEX
    dsrl  v0, v0, 31
    jr    ra
END(check_tlb)

LEAF(find_index)
    dmtc0 a0, CP0_ENTRYHI
    tlbp
    dmfc0 v0, CP0_INDEX
    andi  v0, v0, 0x3f
    jr    ra
END(find_index)

LEAF(get_cp0_index)
    dmfc0 v0, CP0_INDEX
    jr    ra
END(get_cp0_index)

LEAF(set_cp0_index)
    
END(set_cp0_index)

LEAF(get_cp0_badvaddr)
    dmfc0 v0, CP0_BADVADDR
    jr    ra
END(get_cp0_badvaddr)

LEAF(get_cp0_entrylo0)
    
END(get_cp0_entrylo0)

LEAF(set_cp0_entrylo0)
    
END(set_cp0_entrylo0)

LEAF(get_cp0_entrylo1)
    
END(get_cp0_entrylo1)

LEAF(set_cp0_entrylo1)
   
END(set_cp0_entrylo1)

LEAF(set_cp0_pagemask)
    
END(set_cp0_pagemask)

LEAF(tlbwr_operation)
   
END(tlbwr_operation)

LEAF(tlbwi_operation)
    
END(tlbwi_operation)

LEAF(tlbp_operation)
    
END(tlbp_operation)



LEAF(get_cp0_config)
    
END(get_cp0_config)

LEAF(get_cpu_id)
    dmfc0 v0, $15, 1
    andi  v0, v0, 0x3ff
    jr    ra
END(get_cpu_id)

