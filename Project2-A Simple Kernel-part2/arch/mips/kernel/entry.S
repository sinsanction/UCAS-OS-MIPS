
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
    daddi sp, sp, -16
    sd    k0, 0(sp)
    sd    k1, 8(sp)
    daddi k1, sp, 16
    ld    k0, current_running
    ld    sp, 0(k0)
    daddi sp, sp, -OFFSET_SIZE
    sd    k1, OFFSET_REG29(sp)
    daddi k1, k1, -16
    ld    k0, 0(k1)
    ld    k1, 8(k1)
    .set    at
.endm

.macro SAVE_CONTEXT offset
    .set    noat
    sd    $0, OFFSET_REG0(sp)
    sd    $1, OFFSET_REG1(sp)
    sd    $2, OFFSET_REG2(sp)
    sd    $3, OFFSET_REG3(sp)
    sd    $4, OFFSET_REG4(sp)
    sd    $5, OFFSET_REG5(sp)
    sd    $6, OFFSET_REG6(sp)
    sd    $7, OFFSET_REG7(sp)
    sd    $8, OFFSET_REG8(sp)
    sd    $9, OFFSET_REG9(sp)
    sd    $10, OFFSET_REG10(sp)
    sd    $11, OFFSET_REG11(sp)
    sd    $12, OFFSET_REG12(sp)
    sd    $13, OFFSET_REG13(sp)
    sd    $14, OFFSET_REG14(sp)
    sd    $15, OFFSET_REG15(sp)
    sd    $16, OFFSET_REG16(sp)
    sd    $17, OFFSET_REG17(sp)
    sd    $18, OFFSET_REG18(sp)
    sd    $19, OFFSET_REG19(sp)
    sd    $20, OFFSET_REG20(sp)
    sd    $21, OFFSET_REG21(sp)
    sd    $22, OFFSET_REG22(sp)
    sd    $23, OFFSET_REG23(sp)
    sd    $24, OFFSET_REG24(sp)
    sd    $25, OFFSET_REG25(sp)
    sd    $26, OFFSET_REG26(sp)
    sd    $27, OFFSET_REG27(sp)
    sd    $28, OFFSET_REG28(sp)
    sd    $30, OFFSET_REG30(sp)
    sd    $31, OFFSET_REG31(sp)
    dmfc0 k0, CP0_STATUS
    sd    k0, OFFSET_STATUS(sp)
    dmfc0 k0, CP0_CAUSE
    sd    k0, OFFSET_CAUSE(sp)
    dmfc0 k0, CP0_WATCHHI
    sd    k0, OFFSET_HI(sp)
    dmfc0 k0, CP0_WATCHLO
    sd    k0, OFFSET_LO(sp)
    dmfc0 k0, CP0_BADVADDR
    sd    k0, OFFSET_BADVADDR(sp)
    dmfc0 k0, CP0_EPC
    sd    k0, OFFSET_EPC(sp)
    sd    ra, OFFSET_PC(sp)
    ld    k0, current_running
    ld    k1, 248(k0)
    sd    k1, OFFSET_PRE_STATE(sp)
    daddi k1, $0, 1
    sd    k1, 248(k0)
    sd    sp, 0(k0)
    .set    at
.endm

.macro RESTORE_CONTEXT offset
    .set    noat
    daddi sp, sp, OFFSET_SIZE
    sd    sp, 0(k0)
    daddi sp, sp, -OFFSET_SIZE
    ld    k1, OFFSET_PRE_STATE(sp)
    sd    k1, 248(k0)
    ld    $0, OFFSET_REG0(sp)
    ld    $1, OFFSET_REG1(sp)
    ld    $2, OFFSET_REG2(sp)
    ld    $3, OFFSET_REG3(sp)
    ld    $4, OFFSET_REG4(sp)
    ld    $5, OFFSET_REG5(sp)
    ld    $6, OFFSET_REG6(sp)
    ld    $7, OFFSET_REG7(sp)
    ld    $8, OFFSET_REG8(sp)
    ld    $9, OFFSET_REG9(sp)
    ld    $10, OFFSET_REG10(sp)
    ld    $11, OFFSET_REG11(sp)
    ld    $12, OFFSET_REG12(sp)
    ld    $13, OFFSET_REG13(sp)
    ld    $14, OFFSET_REG14(sp)
    ld    $15, OFFSET_REG15(sp)
    ld    $16, OFFSET_REG16(sp)
    ld    $17, OFFSET_REG17(sp)
    ld    $18, OFFSET_REG18(sp)
    ld    $19, OFFSET_REG19(sp)
    ld    $20, OFFSET_REG20(sp)
    ld    $21, OFFSET_REG21(sp)
    ld    $22, OFFSET_REG22(sp)
    ld    $23, OFFSET_REG23(sp)
    ld    $24, OFFSET_REG24(sp)
    ld    $25, OFFSET_REG25(sp)
    ld    $27, OFFSET_REG27(sp)
    ld    $28, OFFSET_REG28(sp)
    ld    $30, OFFSET_REG30(sp)
    ld    $31, OFFSET_REG31(sp)
    ld    k0, OFFSET_STATUS(sp)
    dmtc0 k0, CP0_STATUS
    ld    k0, OFFSET_CAUSE(sp)
    dmtc0 k0, CP0_CAUSE
    ld    k0, OFFSET_HI(sp)
    dmtc0 k0, CP0_WATCHHI
    ld    k0, OFFSET_LO(sp)
    dmtc0 k0, CP0_WATCHLO
    ld    k0, OFFSET_BADVADDR(sp)
    dmtc0 k0, CP0_BADVADDR
    ld    k0, OFFSET_EPC(sp)
    dmtc0 k0, CP0_EPC
    ld    k0, OFFSET_REG26(sp)
    ld    sp, OFFSET_REG29(sp)
    .set    at
.endm

# function do_scheduler
NESTED(do_scheduler, 0, ra)
    daddi sp, sp, -OFFSET_SIZE
    sd    k0, 0(sp)
    daddi k0, sp, OFFSET_SIZE
    sd    k0, OFFSET_REG29(sp)
    ld    k0, 0(sp)
    SAVE_CONTEXT
    sd    ra, OFFSET_EPC(sp)
    jal   scheduler
    ld    k0, current_running
    ld    sp, 0(k0)
    RESTORE_CONTEXT
    eret
END(do_scheduler)

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
	 
 
TLBexception_handler_end:
END(TLBexception_handler_entry)

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    SWITCH_STACK
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
    ld    k0, current_running
    ld    sp, 0(k0)
    RESTORE_CONTEXT
    eret
END(handle_int)

NESTED(handle_syscall, 0, sp)
    ld    k0, OFFSET_EPC(sp)
    daddi k0, k0, 4
    sd    k0, OFFSET_EPC(sp)
    jal   system_call_helper
    ld    k0, current_running
    sd    v0, OFFSET_REG2(sp)
    RESTORE_CONTEXT
    eret
END(handle_syscall)

NESTED(handle_tlb, 0, sp)
    dmfc0 a0, CP0_CAUSE
    dmfc0 a1, CP0_EPC
    dmfc0 a2, CP0_BADVADDR
    jal   other_exception_handler
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


LEAF(set_cp0_entryhi)
   
END(set_cp0_entryhi)


LEAF(get_cp0_index)
    
END(get_cp0_index)

LEAF(set_cp0_index)
    
END(set_cp0_index)

LEAF(get_cp0_badvaddr)
    
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
    
END(get_cpu_id)

