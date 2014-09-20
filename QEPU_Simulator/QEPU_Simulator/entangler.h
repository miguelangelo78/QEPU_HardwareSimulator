#ifndef ENTANGLER_H_
#define ENTANGLER_H_
#include "qmem.h"

class Entangler{
	public:
		Entangler(QMEM* quantum_memory);
		~Entangler();
		bool is_qbit_entangled(); // MAY OR MAY NOT NEED
		bool is_gateout_entangled();
		void clear_entanglement();
		void entangle();
		void update();
	private:
		std::vector<int32_t> entangled;
		QMEM* quantum_memory;
};
#endif