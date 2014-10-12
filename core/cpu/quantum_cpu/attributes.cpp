#include "../../../stdafx.h"
#include "qepu.h"

void QEPU::set_attribute(int attribute_type){
	switch (attribute_type){
		case ATT_FLAG:
			flags.flaglist[qmem.fetch_register(INT_REGDATA_BUFFER)] = qmem.fetch_register(INT_REGDATA_BUFFER+1);
		break;
	}
}

void QEPU::get_attribute(int attribute_type){
	switch (attribute_type){
		case ATT_FLAG:
			qmem.set_register(qmem.fetch_register(INT_REGDATA_BUFFER+1), flags.flaglist[qmem.fetch_register(INT_REGDATA_BUFFER)]);
		break;
	}
}