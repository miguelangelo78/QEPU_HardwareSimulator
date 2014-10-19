#include "classical\cmem\cmem.h"
#include "quantum\qmem\qmem.h"
#include "../../../globals/global_constants.h"

class RegInterface{
public:
	RegInterface();
	void setid(byte id);
	void write(int address,intq data);
	intq read(int address);
	void writec(int address, intq data);
	intq readc(int address);
	void dumpmem(int length);

	//overrides for the quantum memory only:
	void write(int index, int dim, intq deg);
	intq read(int index, intq dim);
	QMEM get_quantummemory();
private:
	int device_id;
	QMEM qmem;
	CMEM cmem;
};