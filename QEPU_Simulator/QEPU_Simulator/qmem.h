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

		class Entangler{
		public:
			Entangler(QMEM* quantum_memory);
			bool is_qbit_entangled(); // MAY OR MAY NOT NEED
			bool is_gateout_entangles();
			void clear_entanglement();
			void entangle();
			void update();
		private:
			std::vector<int32_t> entangled;
			QMEM* quantum_memory;
		};
	private:
		int measure(int amplitude);
		Gates gates;
		Serial serial;
		Utils utils;
		Entangler entangler;
		std::vector<std::vector<int8_t>> mem;
};
#endif