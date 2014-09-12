#ifndef QMEM_H_
#define QMEM_H_
#include "qepu_constants.h"
#include "gates.h"
#include "uart.h"
#include "utils.h"
class QMEM{
	public:
		QMEM();
		void write(int index, unsigned __int8 dim, unsigned __int8 deg);
		unsigned __int8 read(int index, unsigned __int8 dim);
		void dumpmem(int length);
		int fetch_register(int address);
		void set_register(int address, unsigned __int8 data);
	private:
		void interrupt_cpu(int interrupt_type);
		void interrupt_done();
		void setup_seed();
		int measure(int amplitude);
		Gates gates;
		Serial serial;
		Utils utils;
		std::vector<std::vector<int8_t>> mem;
};
#endif