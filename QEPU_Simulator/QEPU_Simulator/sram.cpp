#include "stdafx.h"
#include "sram.h"
SRAM::SRAM(){
	init();
}
void SRAM::init(){
	//CTRLMEM_DIR = OUTPUT;
	stack_head_offset = MEMORY_HEAD_PERMISSION_OFFSET + HEAD_PROGRAMMER_OFFSET;
	stack_tail_offset = ADDRESS_MAX - MEMORY_TAIL_PERMISSION_OFFSET - TAIL_PROGRAMMER_OFFSET;
	memory_restrictedaccess_allowed = false;
}
void SRAM::set_address(int address){
	/*MEM_ADDRESS_DIR = OUTPUT;
	if (!memory_restrictedaccess_allowed)
	if (address + stack_head_offset>stack_tail_offset)MEM_ADDRESS = stack_tail_offset;
	else MEM_ADDRESS = address + stack_head_offset;
	else MEM_ADDRESS = address;*/
}
int8_t SRAM::read(int address){
	memory_management();
	set_address(address);
	/*BUS_DIR = INPUT;
	setctrlpin(MRE, HIGH);
	int bus_data = BUS_IN;
	setctrlpin(MRE, LOW);
	BUS_DIR = OUTPUT;
	MEM_ADDRESS = LOW;
	return bus_data;*/
	return 0;
}
void SRAM::write(int address, int data){
	memory_management();
	set_address(address);
	bus_write(data);
	setctrlpin(MWR, HIGH);
	setctrlpin(MWR, LOW);
	//MEM_ADDRESS = LOW;
	bus_write(LOW);
}
void SRAM::bus_write(int data){
	//BUS_DIR = OUTPUT;
	//BUS_OUT = data;
}
void SRAM::setctrlpin(int pin, int state){
	//CTRLMEM &= ~(1 << pin); // CLEAR PIN
	//CTRLMEM |= (state << pin); // SET PIN
}
int SRAM::pop(){
	memory_management();
	int popped_number = read(0);
	stack_head_offset++;
	if (stack_head_offset>stack_tail_offset) return 0;
	else return popped_number;
}
void SRAM::push(int register_data){
	memory_management();
	if (stack_head_offset <= MEMORY_HEAD_PERMISSION_OFFSET){ //PUSH EVERY OTHER ELEMENT FORWARD ****
		int *ram_memory = utils.arrint_shiftright(read_sram(), register_data, stack_tail_offset - stack_head_offset);
		for (int i = stack_head_offset; i<stack_tail_offset; i++) write(i, ram_memory[i]);
	}
	else{
		stack_head_offset--;
		write(0, register_data);
	}
}
int* SRAM::read_sram(){
	int ram_memory[ADDRESS_MAX];
	for (int i = stack_head_offset; i<stack_tail_offset; i++) ram_memory[i] = read(i);
	return ram_memory;
}
void SRAM::dumpmem(int length){
	memory_management();
	int head = 0, tail = length;
	if (length <= 0){ head = stack_head_offset; tail = stack_tail_offset; }
	for (int i = 0; i<tail + head; i++){
		char mem_debug_str[40];
		sprintf(mem_debug_str, "Address: %d (Off: %d) Data: %d", i, i + stack_head_offset, read(i));
		serial.writestrln(mem_debug_str);
	}
}
void SRAM::memory_management(){
	if (stack_head_offset>stack_tail_offset || stack_head_offset<0 || stack_head_offset>ADDRESS_MAX || stack_tail_offset<0 || stack_tail_offset>ADDRESS_MAX){
		init(); //RESTORE STACK POINTERS TO DEFAULT
	}
}
void SRAM::permissions(bool allowed){
	memory_restrictedaccess_allowed = allowed;
}