
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
    sd    t0, 0(sp)
    sd    t1, 8(sp)
    daddi t1, sp, 16
    ld    t0, current_running
    ld    sp, 0(t0)
    daddi sp, sp, -OFFSET_SIZE
    sd    sp, 0(t0)
    sd    t1, OFFSET_REG29(sp)
    daddi t1, t1, -16
    ld    t0, 0(t1)
    ld    t1, 8(t1)
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
    dmfc0 t0, CP0_STATUS
    sd    t0, OFFSET_STATUS(sp)
    dmfc0 t0, CP0_CAUSE
    sd    t0, OFFSET_CAUSE(sp)
    dmfc0 t0, CP0_WATCHHI
    sd    t0, OFFSET_HI(sp)
    dmfc0 t0, CP0_WATCHLO
    sd    t0, OFFSET_LO(sp)
    dmfc0 t0, CP0_BADVADDR
    sd    t0, OFFSET_BADVADDR(sp)
    dmfc0 t0, CP0_EPC
    sd    t0, OFFSET_EPC(sp)
    sd    ra, OFFSET_PC(sp)
    .set    at
.endm

.macro RESTORE_CONTEXT offset
    .set    noat
    ld    t0, current_running
    ld    t1, 0(t0)
    daddi t1, t1, OFFSET_SIZE
    sd    t1, 0(t0)
    daddi t1, t1, -OFFSET_SIZE
    ld    $0, OFFSET_REG0(t1)
    ld    $1, OFFSET_REG1(t1)
    ld    $2, OFFSET_REG2(t1)
    ld    $3, OFFSET_REG3(t1)
    ld    $4, OFFSET_REG4(t1)
    ld    $5, OFFSET_REG5(t1)
    ld    $6, OFFSET_REG6(t1)
    ld    $7, OFFSET_REG7(t1)
    ld    $10, OFFSET_REG10(t1)
    ld    $11, OFFSET_REG11(t1)
    ld    $12, OFFSET_REG12(t1)
    ld    $13, OFFSET_REG13(t1)
    ld    $14, OFFSET_REG14(t1)
    ld    $15, OFFSET_REG15(t1)
    ld    $16, OFFSET_REG16(t1)
    ld    $17, OFFSET_REG17(t1)
    ld    $18, OFFSET_REG18(t1)
    ld    $19, OFFSET_REG19(t1)
    ld    $20, OFFSET_REG20(t1)
    ld    $21, OFFSET_REG21(t1)
    ld    $22, OFFSET_REG22(t1)
    ld    $23, OFFSET_REG23(t1)
    ld    $24, OFFSET_REG24(t1)
    ld    $25, OFFSET_REG25(t1)
    ld    $26, OFFSET_REG26(t1)
    ld    $27, OFFSET_REG27(t1)
    ld    $28, OFFSET_REG28(t1)
    ld    $29, OFFSET_REG29(t1)
    ld    $30, OFFSET_REG30(t1)
    ld    $31, OFFSET_REG31(t1)
    ld    t0, OFFSET_STATUS(t1)
    dmtc0 t0, CP0_STATUS
    ld    t0, OFFSET_CAUSE(t1)
    dmtc0 t0, CP0_CAUSE
    ld    t0, OFFSET_HI(t1)
    dmtc0 t0, CP0_WATCHHI
    ld    t0, OFFSET_LO(t1)
    dmtc0 t0, CP0_WATCHLO
    ld    t0, OFFSET_BADVADDR(t1)
    dmtc0 t0, CP0_BADVADDR
    ld    t0, OFFSET_EPC(t1)
    dmtc0 t0, CP0_EPC
    ld    t0, OFFSET_REG8(t1)
    ld    t1, OFFSET_REG9(t1)
    .set    at
.endm

# function do_scheduler
NESTED(do_scheduler, 0, ra)
    SWITCH_STACK
    SAVE_CONTEXT
    jal scheduler
    RESTORE_CONTEXT
    jr ra
END(do_scheduler)

LEAF(reset_timer)
    
END(reset_timer)

LEAF(set_cp0_status)
    
END(set_cp0_status)

LEAF(set_cp0_cause)
   
END(set_cp0_cause)

LEAF(get_cp0_status)
    
END(get_cp0_status)

LEAF(get_cp0_cause)
    
END(get_cp0_cause)

LEAF(get_cp0_count)
	 
END(get_cp0_count)

LEAF(get_cp0_compare)
	 
END(get_cp0_compare)

 
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
    
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    
    
END(handle_int)

NESTED(handle_syscall, 0, sp)
   
    
END(handle_syscall)

NESTED(handle_tlb, 0, sp)
   
END(handle_tlb)

NESTED(handle_other, 0, sp)
    
   
END(handle_other)

LEAF(exception_handler_exit)
  
END(exception_handler_exit)

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

