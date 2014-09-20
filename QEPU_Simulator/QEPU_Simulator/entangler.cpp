#include "entangler.h"

Entangler::Entangler(QMEM*quantum_memory){
	this->quantum_memory = quantum_memory;
}

Entangler::~Entangler(){ delete quantum_memory; }

bool Entangler::is_qbit_entangled(){
	
	return false;
}
bool Entangler::is_gateout_entangled(){

	return false;
}
void Entangler::clear_entanglement(){

}
void Entangler::entangle(){

}
void Entangler::update(){

}