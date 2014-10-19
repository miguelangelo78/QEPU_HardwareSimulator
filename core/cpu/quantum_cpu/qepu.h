#ifndef QEPU_H_
#define QEPU_H_
#include "../../../globals/global_constants.h"
#include "../../../globals/instruction_set.h"
#include "../registers/reg_interface.h"
#include "../../modules/io/uart/uart.h"
#include "../../modules/io/i2c/eeprom/eeprom.h"
#include "../../modules/memory/sram/sram.h"
#include "../../../math/gates/lib/quantum_multiplier.h"
#include "../../../misc/utils/utils.h"

class QEPU{
	public:
		QEPU(char*eeprom_filepath);
		class Flag{
			public:
				Flag();
				static void compare(int data1, int data2);
				static void clear();
				static int8_t flaglist[FLAG_COUNT];
		};
		class Jumpstack{
			public:
				Jumpstack();
				void push(int newaddress);
				int pop();
				void dumpjumpstack(); // TEMPORARY
			private:
				int newest_address_index;
				int addresses_toreturn[JUMP_STACK_SIZE];
		};
		void run();
		Flag flags;
	private:
		void set_attribute(int attribute_type);
		void get_attribute(int attribute_type);
		void interrupt_cpu(int interrupt_type);
		void interrupt_done();
		void setup_seed();
		void set_programcounter(int newaddress);
		void execute(int func, intq op1, byte op1meta, intq op2, byte op2meta, intq op3, byte op3meta);
		RegInterface reg_in;
		Gates gates;
		EEProm eeprom;
		SRAM sram;
		Serial serial;
		Jumpstack jumpstack;
		Utils utils;
		int program_counter;
		int program_counter_maximum;
		bool interrupt_signal;
};
#endif