#ifndef QMEM_H_
#define QMEM_H_
#include "../../../../../globals/global_constants.h"
#include "../../../../modules/io/uart/uart.h"
#include "../../../../../misc/utils/utils.h"

class QMEM{
	public:
		QMEM();
		void write(int index, intq dim, intq deg);
		intq read(int index, intq dim);
		void dumpmem(int length);
		intq fetch_register(int address);
		void set_register(int address, intq data);
	private:
		int measure(int amplitude);
		int touch(double probability); 
		Serial serial;
		Utils utils;
		std::vector<std::vector<intq>> mem;

};
#endif