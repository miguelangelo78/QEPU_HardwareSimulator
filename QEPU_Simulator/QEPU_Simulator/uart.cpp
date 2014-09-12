#include "stdafx.h"
#include "uart.h"

Serial::Serial(){}

char Serial::read(){
	return getchar();
}
void Serial::write(char data){
	std::cout << data;
}
void Serial::writestr(char * data){
	for (int i = 0; true; i++) if (data[i] == '\0') break; else write(data[i]);
}
void Serial::writeln(char data){
	write(data);
	write(13); //NEW LINE
}
void Serial::writestrln(char * data){
	writestr(data);
	write(13); //NEW LINE
}