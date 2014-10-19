#include "reg_interface.h"

RegInterface::RegInterface() : device_id(0){}

void RegInterface::setid(byte id){
	/* classical registers = 0
	   quantum registers   = 1 */
	device_id = id;
}

void RegInterface::write(int address, intq data){
	switch (device_id){
		case ID_REG_CMEM:
			cmem.write(address,data);
			break;
		case ID_REG_QMEM: 
			qmem.set_register(address,data);
			break;
	}
}

intq RegInterface::read(int address){
	intq data_read = 0;
	switch (device_id){
		case ID_REG_CMEM:
			data_read=cmem.read(address);
			break;
		case ID_REG_QMEM:
			data_read = qmem.fetch_register(address);
			break;
		}
	return data_read;
}

void RegInterface::writec(int address, intq data){
	cmem.write(address, data);
}
intq RegInterface::readc(int address){
	return cmem.read(address);
}

void RegInterface::dumpmem(int length){
	switch (device_id){
		case ID_REG_CMEM: cmem.dumpmem(length); break;
		case ID_REG_QMEM: qmem.dumpmem(length); break;
	}
}

void RegInterface::write(int index, int dim, intq deg){
	if (device_id == ID_REG_QMEM) qmem.write(index, dim, deg);
}

intq RegInterface::read(int index, intq dim){
	if (device_id == ID_REG_QMEM) return qmem.read(index,dim);
	else return 0;
}

QMEM RegInterface::get_quantummemory(){
	return qmem;
}