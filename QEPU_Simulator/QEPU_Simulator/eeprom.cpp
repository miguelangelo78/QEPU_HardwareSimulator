#include "stdafx.h"
#include "eeprom.h"
EEProm::EEProm(char*eeprom_filepath) :eeprom_filepath(eeprom_filepath), eeprom_binaryfile_write(eeprom_filepath, std::ofstream::out | std::ofstream::in | std::ofstream::binary),
									  eeprom_binfile_read(eeprom_filepath,std::ofstream::in | std::ofstream::binary)
{
	if (!eeprom_binaryfile_write.is_open()){
		std::cout << "The file '" << eeprom_filepath << "' does not exist!";
		std::cin.get();
		exit(EXIT_FAILURE);
	}
}

EEProm::~EEProm(){
	eeprom_binaryfile_write.close();
	eeprom_binfile_read.close();
}

void EEProm::write(uint16_t address, uint8_t data){
	eeprom_binaryfile_write.clear();
	eeprom_binaryfile_write.seekp(address);
	eeprom_binaryfile_write << data;
}

uint8_t EEProm::read(uint16_t address){
	char c;
	eeprom_binfile_read.clear();
	eeprom_binfile_read.seekg(address, eeprom_binfile_read.beg);
	eeprom_binfile_read.get(c);
	return c;
}

char* EEProm::readall(){
	return "";
}

int EEProm::count_lines(){
	int tdb[INSTR_WIDTH + 1];
	int tdb_i = 0, ctr = 0;
	bool all_data_read = false;
	for (int i = 0; true; i++){
		tdb[tdb_i] = read(i);
		if (tdb_i >= INSTR_WIDTH){
			ctr++;
			for (int j = 0; j<INSTR_WIDTH; j++){
				if (tdb[j] == 204) all_data_read = true;
				else all_data_read = false;
			}
			tdb_i = 0;
			if (all_data_read) return ctr;
		}
		else tdb_i++;
	}
	return ctr;
}

int * EEProm::get_line(int line){
	int line_to_fetch[INSTR_WIDTH];
	int line_width = INSTR_WIDTH;
	for (int cell = 0; cell<INSTR_WIDTH; cell++)	line_to_fetch[cell] = read(cell + (line*line_width));
	return line_to_fetch;
}