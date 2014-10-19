#include "../../../../../globals/global_constants.h"

class CMEM{
	public: 
		CMEM();
		void write(int address, intq data);
		intq read(int address);
		void dumpmem(int length);
	private:
		std::vector<intq> mem;
};