#ifndef UTILS_H_
#define UTILS_H_
#include "../../Globals/qepu_constants.h"
#include <math.h>
#include <limits.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class Utils{
	public:
		char* char2str(char c);
		int* str2intarr(char* c);
		int dec2hex(int dec);
		char* int2str(int dec);
		int countdigits(int dec);
		int concint(int n1, int n2);
		void delay(int ms);
		char *int2binstr(int num, int strlength);
		float custom_pow(float base, int exp);
		int * arrint_shiftright(int * arr, int newelem, int array_length);
};

#endif