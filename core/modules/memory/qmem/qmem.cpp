#include "../../../../stdafx.h"
#include "qmem.h"

QMEM::QMEM(){
	std::vector<intq> sample{ 0, 0 };
	mem.resize(QUBIT_COUNT,sample);
}

void QMEM::write(int index, intq dim, intq deg){
	mem[index][dim] = deg;
}

intq QMEM::read(int index, intq dim){
	return mem[index][dim];
}

int QMEM::touch(double probability){
	return RAND_MAX*probability >= rand();
}

int QMEM::measure(int amplitude){
	return touch(utils.custom_pow(sin(amplitude*M_PI / 360), 2));
}

intq QMEM::fetch_register(int address){
	char data_fetched[REGISTER_SIZE + 1] = "";
	int address_offset = address*REGISTER_SIZE;
	for (int i = 0; i<REGISTER_SIZE; i++)
		sprintf(data_fetched, "%s%i", data_fetched, measure(read(address_offset + i, THE)));
	_strrev(data_fetched);
	return strtoul(data_fetched, NULL, 2);
}

void QMEM::set_register(int address, intq data){
	char* data_str = new char[REGISTER_SIZE];
	data_str = utils.long2binstr(data, REGISTER_SIZE);
	_strrev(data_str);
	int address_offset = address*REGISTER_SIZE;
	for (int i = 0; i<REGISTER_SIZE; i++)
		if (data_str[i] == '1') write(address_offset + i, THE, 180);
		else write(address_offset + i, THE, 0);
}
void QMEM::dumpmem(int length){
	serial.writestrln("");
	serial.writestrln("** QUANTUM MEMORY DUMP **");
	for (int i = 0; i<length; i++){
		char * thedim = (char*) malloc(sizeof(char) * 3); sprintf(thedim, "%d", read(i, THE));
		char * phidim = (char*) malloc(sizeof(char) * 3); sprintf(phidim, "%d", read(i, PHI));
		char * qindex = (char*) malloc(sizeof(char));	sprintf(qindex, "%d", i);
		serial.writestr("Q"); serial.writestr(qindex); serial.writestr(": Theta - ");
		serial.writestr(thedim);
		serial.writestr(" , Phi - ");
		serial.writestrln(phidim);
	}
}