#include "../../../stdafx.h"
#include "qepu.h"

void QEPU::interrupt_cpu(int interrupt_type){
	interrupt_signal = false;
	bool interrupt_init = false;
	//NEEDS RESERVED REGISTERS USED FOR ADDRESS POINTING -> ADDRESS SHOULD POINT TO A REGISTER ADDRESS,NOT SRAM
	//NEEDS RESERVED REGISTERS USED FOR DATA SAVING -> SHOULD SAVE ON A REGISTER,NOT SRAM
	int address_pointer_offset = 0;
	//SPECIAL ADDRESSESS:
	int data_buffer_pointer = reg_in.readc(INT_RESERVED_ADDRESSPOINTER);
	int data_in_registerpointer = reg_in.readc(INT_RESERVED_ADDRESSDATASTORE);
	while (!interrupt_signal){
		switch (interrupt_type){
			case INT_UART_OUT_CHAR:
				serial.write(reg_in.readc(INT_RESERVED_ADDRESSPOINTER));
				interrupt_done();
				break;
			case INT_UART_OUT_BUFFER:
				{
					int data_read = sram.read(data_buffer_pointer + address_pointer_offset);
					if (data_read == STRING_TERMINATOR) interrupt_done();
					else{
						serial.write(data_read);
						address_pointer_offset++;
					}
				}
				break;
			case INT_UART_IN_CHAR_NO_ECHO:
				sram.write(data_in_registerpointer + address_pointer_offset, serial.read());
				interrupt_done();
				break;
			case INT_UART_IN_CHAR_ECHO:
				{
					char char_echo = serial.read();
					serial.write(char_echo);
					sram.write(data_in_registerpointer + address_pointer_offset, char_echo);
					interrupt_done();
				}
				break;
			case INT_UART_IN_BUFF_NO_ECHO:
				{
					int char_noecho = serial.read();
					if (char_noecho == STRING_CARRETURN){
						interrupt_done();
						std::cout << std::endl;
						sram.write(data_in_registerpointer + address_pointer_offset, STRING_TERMINATOR);
					}
					else
						if (char_noecho == 0x8 && address_pointer_offset>0) address_pointer_offset--;
						else{
							sram.write(data_in_registerpointer + address_pointer_offset, char_noecho);
							address_pointer_offset++;
						}
				}
				break;
			case INT_UART_IN_BUFF_ECHO:
				{
				int char_echo = serial.read();
				serial.write(char_echo);
				if (char_echo == STRING_CARRETURN){
					interrupt_done();
					std::cout << std::endl;
					sram.write(data_in_registerpointer + address_pointer_offset, STRING_TERMINATOR);
				}
				else
					if (char_echo == 0x8 && address_pointer_offset>0) address_pointer_offset--;
					else{
						sram.write(data_in_registerpointer + address_pointer_offset, char_echo);
						address_pointer_offset++;
					}
				}
				break;
			case INT_UART_OUT_CHARAT:
				serial.writeat(sram.read(data_buffer_pointer), reg_in.readc(INT_REGDATA_BUFFER), reg_in.readc(INT_REGDATA_BUFFER + 1));
				interrupt_done();
				break;
			case INT_UART_OUT_BUFFAT:
				{
					if (!interrupt_init) serial.gotoxy(reg_in.readc(INT_REGDATA_BUFFER), reg_in.readc(INT_REGDATA_BUFFER + 1));
					int data_read = sram.read(data_buffer_pointer + address_pointer_offset);
					if (data_read == STRING_TERMINATOR){interrupt_done();serial.gotoxy_reset();}
					else{
						serial.write(data_read); 
						address_pointer_offset++;
					}
				}
				break;
			case INT_UART_GOTOXY:
				serial.gotoxy(reg_in.readc(INT_REGDATA_BUFFER), reg_in.readc(INT_REGDATA_BUFFER + 1));
				interrupt_done();
				break;			
			case INT_UART_GOTOXY_RESET:
				serial.gotoxy_reset();
				interrupt_done();
				break;
			case INT_UART_CLRSCR:
				serial.clrscr();
				interrupt_done();
				break;
			case INT_RESTARTSTACK:
				sram.restart_stack();
				interrupt_done();
				break;
			case INT_FLAGCLEAR: flags.clear(); break;
		}
		interrupt_init = true;
	}
	interrupt_signal = false;
}

void QEPU::interrupt_done(){
	interrupt_signal = true;
}