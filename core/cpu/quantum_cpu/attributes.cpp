#include "../../../stdafx.h"
#include "qepu.h"

void QEPU::set_attribute(int attribute_type){
	switch (attribute_type){
		case ATT_FLAG:{
			int flag_to_write = qmem.fetch_register(INT_REGDATA_BUFFER + 1);
			if (flag_to_write > 1) flag_to_write = 1; else if (flag_to_write < 0) flag_to_write = 0;
			flags.flaglist[qmem.fetch_register(INT_REGDATA_BUFFER)] = flag_to_write;
		}
		break;
		case ATT_SB: // SET STACK BASE. PARAMETERS: AX -> MEMORY ADDRESS
			sram.set_stack_base(qmem.fetch_register(INT_RESERVED_ADDRESSPOINTER));
			break;
		case ATT_SH: // SET STACK HEAD. PARAMETERS: AX -> MEMORY ADDRESS
			sram.set_stack_head(qmem.fetch_register(INT_RESERVED_ADDRESSPOINTER));
			break;
	}
}

void QEPU::get_attribute(int attribute_type){
	switch (attribute_type){
		case ATT_FLAG:
			qmem.set_register(INT_REGDATA_BUFFER+1, flags.flaglist[qmem.fetch_register(INT_REGDATA_BUFFER)]);
			break;
		case ATT_SB: // GET STACK BASE. RETURNS TO REGISTER 'AX'
			qmem.set_register(INT_RESERVED_ADDRESSPOINTER, sram.stack_tail_offset);
			break;
		case ATT_SH: // GET STACK HEAD. RETURNS TO REGISTER 'AX'
			qmem.set_register(INT_RESERVED_ADDRESSPOINTER, sram.stack_head_offset);
			break;
	}
}