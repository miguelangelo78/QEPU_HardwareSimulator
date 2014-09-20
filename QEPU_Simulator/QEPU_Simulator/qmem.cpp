#include "stdafx.h"
#include "qmem.h"

QMEM::QMEM():entangler(this){
	std::vector<int8_t> sample{0,0};
	mem.resize(QUBIT_COUNT,sample);
}

void QMEM::write(int index, unsigned __int8 dim, unsigned __int8 deg){
	mem[index][dim] = deg;
}

unsigned __int8 QMEM::read(int index, unsigned __int8 dim){
	return mem[index][dim];
}

int QMEM::measure(int amplitude){
	return gates.touch(utils.custom_pow(sin(amplitude*M_PI / 360), 2));
}
int QMEM::fetch_register(int address){
	char data_fetched[BUS_WIDTH + 1] = "";
	int address_offset = address*BUS_WIDTH;
	for (int i = 0; i<BUS_WIDTH; i++)
		sprintf(data_fetched, "%s%d", data_fetched, measure(read(address_offset + i, THE)));
	_strrev(data_fetched);
	return strtol(data_fetched, NULL, 2);
}
void QMEM::set_register(int address, unsigned __int8 data){
	char* data_str = new char[BUS_WIDTH];
	data_str = utils.int2binstr(data, BUS_WIDTH);
	_strrev(data_str);
	int address_offset = address*BUS_WIDTH;
	for (int i = 0; i<BUS_WIDTH; i++)
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