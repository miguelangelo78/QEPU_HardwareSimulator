#include "entangler.h"

Entangler::Entangler(QMEM*quantum_memory){
	this->quantum_memory = quantum_memory;
}

Entangler::~Entangler(){ delete quantum_memory; }

bool Entangler::is_gateout_entangled(Complex * vec){ // IS THIS VECTOR POSSIBLE?

	return false;
}

bool Entangler::is_qbit_entangled(Complex * vec){ // IS THIS VECTOR IN THE VECTOR 'ENTANGLED'?
	
	return false;
}

void Entangler::clear_entanglement(int entanglement_index){ // COLLAPSE AN ENTANGLEMENT OF INDEX 'entanglement_index'

}

void Entangler::entangle(Complex * vec){ // ADD THIS VECTOR TO THE ENTANGLEMENT LIST

}

void Entangler::update(){ // UPDATE ALL ENTANGLEMENTS

}