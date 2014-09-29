#ifndef QEPU_H_
#define QEPU_H_
#include "../../Globals/qepu_constants.h"
#include "../QMEM/qmem.h"
#include "../UART/uart.h"
#include "../I2C/eeprom.h"
#include "../SRAM/sram.h"
#include "../../Math/Quantum/gates.h"
#include "../../Misc/Utils/utils.h"

class QEPU{
	public:
		QEPU(char*eeprom_filepath);
		class Flag{
			public:
				Flag();
				static void compare(int data1, int data2);
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
		void execute(int func, int32_t op1, int32_t op2, int32_t op3);
		QMEM qmem;
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