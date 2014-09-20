#include "qmem.h"

QMEM::Entangler::Entangler(QMEM*quantum_memory){
	this->quantum_memory = quantum_memory;
}

bool QMEM::Entangler::is_qbit_entangled(){
	
	return false;
}
bool QMEM::Entangler::is_gateout_entangles(){

	return false;
}
void QMEM::Entangler::clear_entanglement(){

}
void QMEM::Entangler::entangle(){

}
void QMEM::Entangler::update(){

}