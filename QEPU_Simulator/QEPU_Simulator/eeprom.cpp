#include "stdafx.h"
#include "eeprom.h"
EEProm::EEProm(){

}

uint8_t EEProm::write(uint16_t address, uint8_t data){
	return 0;
}

uint8_t EEProm::read(uint16_t address){
	return 0;
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
				if (tdb[j] == 0xFF) all_data_read = true;
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