#ifndef QMEM_H_
#define QMEM_H_
#include "../../../../globals/qepu_constants.h"
#include "../../io/uart/uart.h"
#include "../../../../misc/utils/utils.h"

class QMEM{
	public:
		QMEM();
		void write(int index, unsigned __int8 dim, unsigned __int8 deg);
		unsigned __int8 read(int index, unsigned __int8 dim);
		void dumpmem(int length);
		int fetch_register(int address);
		void set_register(int address, unsigned __int8 data);
	private:
		int measure(int amplitude);
		int touch(double probability); 
		Serial serial;
		Utils utils;
		std::vector<std::vector<int8_t>> mem;
};
#endif