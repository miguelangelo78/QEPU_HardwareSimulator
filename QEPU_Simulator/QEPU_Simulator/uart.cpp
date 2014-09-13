#include "stdafx.h"
#include "uart.h"

Serial::Serial(){}

char Serial::read(){
	fflush(stdin);
	return _getch();
}
void Serial::write(char data){
	fflush(stdin);
	printf("%c",data);
}
void Serial::writestr(char * data){
	for (int i = 0; true; i++) if (data[i] == '\0') break; else write(data[i]);
}
void Serial::writeln(char data){
	write(data);
	write('\n'); //NEW LINE
}
void Serial::writestrln(char * data){
	writestr(data);
	write('\n'); //NEW LINE
}