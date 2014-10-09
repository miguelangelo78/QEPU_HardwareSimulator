#ifndef SRAM_H_
#define SRAM_H_
#include "../../../../globals/qepu_constants.h"
#include "../../../../misc/utils/utils.h"
#include "../../io/uart/uart.h"
#include <math.h>
#include <stdio.h>
class SRAM{
	public:
		SRAM();
		unsigned __int8 read(int address);
		void write(int address,unsigned __int8 data);
		unsigned __int8 pop();
		void push(unsigned __int8 register_data);
		void permissions(bool allowed);
		void dumpmem(int length,bool norestritions);
		int stack_head_offset;
	private:
		Utils utils;
		Serial serial;
		void init();
		void memory_management();
		int* read_sram();
		int stack_tail_offset;
		bool memory_restrictedaccess_allowed;
		std::vector<unsigned __int8> sram_memory;
};
#endif