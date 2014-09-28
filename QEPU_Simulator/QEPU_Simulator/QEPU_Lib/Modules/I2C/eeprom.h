#ifndef _eeprom_H_
#define _eeprom_H_
#include "../../Globals/qepu_constants.h"
#include <inttypes.h>
class EEProm{
	public:
		EEProm(char*eeprom_filepath);
		~EEProm();
		void write(uint16_t address, uint8_t data);
		uint8_t read(uint16_t address);
		char* readall();
		int count_lines();
		int * get_line(int line);
		char*eeprom_filepath;
		std::ofstream eeprom_binaryfile_write;
		std::ifstream eeprom_binfile_read;
};
#endif