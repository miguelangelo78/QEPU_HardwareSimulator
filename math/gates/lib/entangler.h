#ifndef ENTANGLER_H_
#define ENTANGLER_H_
#include "../../../core/cpu/registers/quantum/qmem/qmem.h"
#include "../../complex/complex.h"

class Entangler{
	public:
		Entangler(QMEM* quantum_memory);
		~Entangler();
		bool is_qbit_entangled(Complex * vec); // IS THIS VECTOR IN THE VECTOR 'ENTANGLED'?
		bool is_gateout_entangled(Complex * vec); // IS THIS VECTOR POSSIBLE?
		void clear_entanglement(int entanglement_index); // COLLAPSE AN ENTANGLEMENT OF INDEX 'entanglement_index'
		void entangle(Complex * vec); // ADD THIS VECTOR TO THE ENTANGLEMENT LIST
		void update(); // UPDATE ALL ENTANGLEMENTS
	private:
		std::vector<int32_t> entangled;
		QMEM* quantum_memory;
};
#endif