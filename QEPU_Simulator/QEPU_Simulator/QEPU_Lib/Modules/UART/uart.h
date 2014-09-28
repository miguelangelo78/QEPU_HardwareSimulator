#ifndef UART_H_
#define UART_H_
#include "../../Globals/qepu_constants.h"
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class Serial{
	public:
		Serial();
		char read();
		void write(char data);
		void writestr(char * data);
		void writeln(char data);
		void writestrln(char * data);
		void writeat(char data, int x, int y);
		void writestrat(char * data, int x, int y);
		void gotoxy_reset();
		void gotoxy(int x, int y);
		void clrscr();
	private:
		int last_cursor_x;
		int last_cursor_y;
		int console_times_cleared;
		bool is_lastcurs_def;
		//EXCLUSIVE FOR THE HARDWARE SIMULATOR:
		int get_console_width();
		int get_console_height();
};
#endif