#include "cmem.h"

CMEM::CMEM(){
	mem.resize(C_REGISTER_COUNT, 0);
}

void CMEM::write(int address, intq data){
	mem[address] = data;
}

intq CMEM::read(int address){
	return mem[address];
}

void CMEM::dumpmem(int length){
	for (int i = 0; i < C_REGISTER_COUNT; i++) std::cout << "Address: "<<i<<" Data: "<<read(i)<<std::endl;
}