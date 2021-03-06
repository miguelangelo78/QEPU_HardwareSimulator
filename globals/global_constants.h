#ifndef GLOBAL_CONSTANTS_H_
#define GLOBAL_CONSTANTS_H_
#include <stdint.h>
#include <conio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <inttypes.h>

//QUBIT MEMORY AND INSTRUCTIONS CONSTANTS:
#define REGISTER_SIZE NUMERIC_SYSTEM
#define REGISTER_COUNT 25
#define QUBIT_COUNT REGISTER_SIZE*REGISTER_COUNT
#define QUBIT_BYTE_SIZE 4
#define MAX_NEWTHEPHI 6
#define WRITE_QB 6
#define READ_QB 5
#define DIM_SEL 4
#define THE 0
#define PHI 1
//CLASSICAL REGISTERS PROPERTIES:
#define C_REGISTER_COUNT 25

//REGISTER INTERFACE CONSTANTS
#define ID_REG_CMEM 0
#define ID_REG_QMEM 1

//INSTRUCTION DEFINITION CONSTANTS:
#define OPERAND_COUNT 3
#define FUNC_WIDTH 1
#define OP1_WIDTH 4
#define OP2_WIDTH 4
#define OP3_WIDTH 4

//#define INSTR_WIDTH (FUNC_WIDTH+OP1_WIDTH+OP2_WIDTH+OP3_WIDTH)-(OP1N2_OFFSET*OPERAND_COUNT) // "DYNAMIC" SIZE INSTRUCTION WIDTH
#define INSTR_WIDTH 16 // FIXED INSTRUCTION WIDTH
#define FIXED_FUNC_OFFSET 0
#define FIXED_OP1_OFFSET 1
#define FIXED_OP2_OFFSET 6
#define FIXED_OP3_OFFSET 11
#define OP1N2_OFFSET 3

//OPERAND TYPES:
#define TYPE_NULL 0
#define TYPE_CREG 1
#define TYPE_CREG_POINTER 2
#define TYPE_QREG 3
#define TYPE_QREG_POINTER 4
#define TYPE_MEM_CONTAINER 5
#define TYPE_MEM_ADDRESS 6
#define TYPE_CONSTANT 7
#define TYPE_QUBIT 8
#define TYPE_QUBIT_THETA 9
#define TYPE_QUBIT_PHI 10
#define TYPE_STACKHEAD 11
#define TYPE_STACKBASE 12

//SRAM CONSTANTS:
#define MEMORY_HEAD_PERMISSION_OFFSET 0
#define MEMORY_TAIL_PERMISSION_OFFSET 0
#define ADDRESS_MAX 256
#define HEAD_PROGRAMMER_OFFSET 20
#define TAIL_PROGRAMMER_OFFSET 0
#define RAM_62256_ADDRESS_MAXSIZE pow(2,15)
#define RAM_MODEL pow(2,23) // ~ 1MB OF RAM
// MEMORY DIAGRAM:
// [MHPO|HPO|FREE|TPO|MTPF]
//FLAG CONSTANTS:
#define FLAG_COUNT 9
#define CND_LWER 0
#define CND_LWER_EQUAL 1
#define CND_EQUAL 2
#define CND_GRTER_EQUAL 3
#define CND_GRTER 4
#define CND_DIFF 5
#define CND_ZERO 6
#define CND_NOT_ZERO 7
#define ALLOW_BRANCH_STACK 8
//JUMP STACK CONSTANTS:
#define JUMP_STACK_SIZE 100
//INTERRUPT CONSTANTS:
	//REGISTER INTERRUPT TYPES:
#define INT_RESERVED_ADDRESSPOINTER 0		// RESERVED FOR OUTPUTS (POINTS TO A BUFFER IN THE RAM)
#define INT_RESERVED_ADDRESSDATASTORE 1		// RESERVED FOR INPUTS (POINTS TO A BUFFER IN THE RAM)
#define INT_RESERVED_RET_ADDRESSPOINTER 2	// RETURN REGISTER (POINTS TO A BUFFER IN THE RAM)
#define INT_REGDATA_BUFFER 3				// HOLDS THE DATA (MISC DATA) FOR INTERRUPTS (IS A BUFFER (USES N REGISTERS WITH DATA))
	//INTERRUPT TYPE CALLS:
#define INT_UART_OUT_CHAR 0
#define INT_UART_OUT_BUFFER 1
#define INT_UART_IN_CHAR_NO_ECHO 2
#define INT_UART_IN_CHAR_ECHO 3
#define INT_UART_IN_BUFF_NO_ECHO 4
#define INT_UART_IN_BUFF_ECHO 5
#define INT_UART_OUT_CHARAT 6
#define INT_UART_OUT_BUFFAT 7
#define INT_UART_GOTOXY 8
#define INT_UART_GOTOXY_RESET 9
#define INT_UART_CLRSCR 10
#define INT_RESTARTSTACK 11
#define INT_FLAGCLEAR 12
//ATTRIBUTE TYPES:
#define ATT_FLAG 0
#define ATT_SB 1
#define ATT_SH 2
#define ATT_BES 3
//GENERAL PURPOSE CONSTANTS:
#define BINARY_FILE_EOF 0xFF
#define STRING_TERMINATOR 0x0
#define STRING_CARRETURN 0xD
#define STRING_NEWLINE 0xA
#define FALSE 0
#define TRUE 1
#define INPUT 0x0
#define OUTPUT 0xFF
#define HIGH 1
#define LOW 0
#define HIGH 1
#define LOW 0
#define M_PI 3.14159265358979323846f
#define F_CPU 14745600
#define MAX_THEORETICAL_PC 1000000
#define DEBUG_MODE false
#define SHOW_LAST_STATE false
#define SHOW_SRAM false
#define SHOW_QUANTUMCALC false
#define KRON_SIZE_EACH_VECTOR 2
#define KRON_MAX_SIZE 3
#define NUMERIC_SYSTEM 32 // THIS SYSTEM WORKS IN 32 BITS
typedef unsigned long intq;
#endif