
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

.macro SAVE_CONTEXT offset
    .set    noat
  
    .set    at
.endm

.macro RESTORE_CONTEXT offset
    .set    noat
   
   .set    at
.endm

# function do_scheduler
NESTED(do_scheduler, 0, ra)
     

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
    
END(get_cpu_id)

LEAF(get_cpu_id)
    
END(get_cpu_id)

