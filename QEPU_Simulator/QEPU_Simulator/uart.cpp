#include "stdafx.h"
#include "uart.h"

Serial::Serial() : last_cursor_x(0), last_cursor_y(0), is_lastcurs_def(true), console_times_cleared(0){
	COORD coord; coord.X = 0; coord.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

char Serial::read(){
	fflush(stdin);
	return _getch();
}

void Serial::write(char data){
	fflush(stdout);
	printf("%c",data);
	if (is_lastcurs_def){
		if (last_cursor_x > get_console_width()){ last_cursor_x = 0; last_cursor_y++; }
		last_cursor_x++;
	}
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

void Serial::writeat(char data, int x, int y){
	gotoxy(x,y);
	write(data);
	gotoxy_reset();
}

void Serial::writestrat(char * data, int x, int y){
	gotoxy(x,y);
	writestr(data);
	gotoxy_reset();
}

void Serial::gotoxy(int x, int y){
	is_lastcurs_def = false;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Serial::gotoxy_reset(){
	is_lastcurs_def = true;
	COORD coord;
	coord.X = last_cursor_x;
	coord.Y = last_cursor_y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int Serial::get_console_width(){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) return csbi.srWindow.Right - csbi.srWindow.Left;
	else return -1;
}

int Serial::get_console_height(){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) return csbi.srWindow.Bottom - csbi.srWindow.Top;
	else return -1;
}

void Serial::clrscr(){
	int console_height = get_console_height();
	for (int i = 0; i < (console_height + 1) * 2; i++) write(STRING_NEWLINE);
	console_times_cleared++;
	gotoxy(0, last_cursor_y+console_height + 1);
	last_cursor_x = 0;
	last_cursor_y = console_height*console_times_cleared + 1;
}