#include "../../../stdafx.h"
#include "qepu.h"

QEPU::QEPU(char*eeprom_filepath):eeprom(eeprom_filepath),gates(&qmem){ serial.writestr("INITIALIZING . . . "); setup_seed(); program_counter = 0; program_counter_maximum = 0; }

void QEPU::setup_seed(){
	srand(time(NULL));
}

void QEPU::set_programcounter(int newaddress){
	program_counter = newaddress-1;
}

void QEPU::run(){
	//TEMPORARY PRINTING FOR DEBUGGING PURPOSES
	serial.writestrln(" RUNNING "); serial.writestrln("");
	int line_width = INSTR_WIDTH;
	program_counter_maximum = eeprom.count_lines();
	for (program_counter = 0; program_counter<program_counter_maximum; program_counter++){
		//FETCH OPERANDS FROM THE EEPROM:
		int eeprom_line_selection = program_counter*line_width;
		int func = 0;	char op1s[OP1_WIDTH] = ""; char op2s[OP2_WIDTH] = ""; char op3s[OP3_WIDTH] = "";
		func = eeprom.read(eeprom_line_selection + FIXED_FUNC_OFFSET); // FUNCTION FETCH
		for (int k = 0; k<QUBIT_BYTE_SIZE; k++){
			sprintf(op1s, "%s%x", op1s, eeprom.read(eeprom_line_selection + FIXED_OP1_OFFSET + k)); // HEX CONCAT TO STRING (OP1 FETCH)
			sprintf(op2s, "%s%x", op2s, eeprom.read(eeprom_line_selection + FIXED_OP2_OFFSET + k)); // HEX CONCAT TO STRING (OP2 FETCH)
			sprintf(op3s, "%s%x", op3s, eeprom.read(eeprom_line_selection + FIXED_OP3_OFFSET + k)); // HEX CONCAT TO STRING (OP3 FETCH)
		}
		//EXECUTE:
		execute(func, strtol(op1s, NULL, 16), strtol(op2s, NULL, 16), strtol(op3s, NULL, 16)); //INSTRUCTION DECODE AND EXECUTE
	}
	if (SHOW_LAST_STATE){
		qmem.dumpmem(QUBIT_COUNT);
		serial.writestrln("");
	}
	if(SHOW_SRAM) sram.dumpmem(30,true);
	
	serial.writestrln("");
	serial.writestr("The execution of the program has finished");
	std::cin.get();
	#pragma endregion
}

void QEPU::execute(int func, int32_t op1, int32_t op2, int32_t op3){
	int *newthephi = nullptr;
	if (DEBUG_MODE)
		std::cout << "Function: " << func 
				  << " , OP1: " << op1 
				  << " , OP2: " << op2 
				  << " , OP3: " << op3 
				  << " , PC: " << program_counter 
				  << " , Max PC: "<<program_counter_maximum
				  << std::endl;
	//TODO: MAKE A SWITCH ON THE FUNCTION
	switch (func){
		//DATA MOVEMENT AND PROGRAM CONTROL/FLUX/IO FUNCTIONS:
	case 0x01: /*MOQ (move qubit) */
		qmem.write(op1, THE, qmem.read(op2, THE));
		qmem.write(op1, PHI, qmem.read(op2, PHI));
		break;
	case 0x02: /*MOR (move register)*/
		qmem.set_register(op1, qmem.fetch_register(op1)); // TOUCH ITSELF 1ST
		qmem.set_register(op1, qmem.fetch_register(op2)); // MOVE TO THE NEW REGISTER (IF IT NEEDS TO BE UNTOUCHED IT NEEDS TO USE MATRICES INSTEAD)
		break;
	case 0x03: /*MOM (move memory)*/
		sram.write(op1, sram.read(op2));
		break;
	case 0x04: /*MOMI (move memory indirect origin) */
		sram.write(op1,sram.read(qmem.fetch_register(op2)));
		break;
	case 0x05: /*MOMD (move memory indirect destination)*/
		sram.write(qmem.fetch_register(op1), qmem.fetch_register(op2));
		break;
	case 0x06: /*STR (store)*/
		sram.write(op1, qmem.fetch_register(op2));
		break;
	case 0x07: /*LOD (load)*/
		qmem.set_register(op1, sram.read(op2));
		break;
	case 0x08: /*CMW (Constant Memory Write)*/
		sram.write(op1, op2);
		break;
	case 0x09: /*CRW* (Constant Register Write)*/
		qmem.set_register(op1, op2);
		break;
	case 0x0A: /*POP (pop)*/
		qmem.set_register(op1, sram.pop());
		break;
	case 0x0B: /*PSH (push)*/
		sram.push(qmem.fetch_register(op1));
		break;
	case 0x0C: /*CMT (constantmovtheta)*/
		qmem.write(op1, THE, op2);
		break;
	case 0x0D: /*CMP (constantmovphi)*/
		qmem.write(op1, PHI, op2);
		break;
	case 0x0E: /*CME (compare)*/
		flags.compare(qmem.fetch_register(op1), qmem.fetch_register(op2));
		break;
	case 0x0F: /*SEA (SET ATTRIBUTE)*/
		set_attribute(op1);
		break;
	case 0x10: /*GEA (GET ATTRIBUTE)*/
		get_attribute(op1);
		break;
		/*IMPLEMENT CONDICIONAL/INCONDICIONAL BRANCHES HERE*/
	case 0x11: /*BES (branch enable stack)*/
		flags.flaglist[ALLOW_BRANCH_STACK] = op1;
		break;
	case 0x12: /*BLW (branch if lower (with jumpstack))*/
		if (flags.flaglist[CND_LWER]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x13: /*BLE (branch if lower or equal (with jumpstack))*/
		if (flags.flaglist[CND_LWER_EQUAL]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x14: /*BEQ (branch if equal) (with jumpstack)*/
		if (flags.flaglist[CND_EQUAL]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x15: /*BGE (branch if greater or equal (with jumpstack))*/
		if (flags.flaglist[CND_GRTER_EQUAL]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x16: /*BGR (branch if greater (with jumpstack))*/
		if (flags.flaglist[CND_GRTER]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x17: /*BDI (branch if different (with jumpstack))*/
		if (flags.flaglist[CND_DIFF]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x18: /*BZE (branch if register(op1)=0 (zero) (with jumpstack))*/
		if (flags.flaglist[CND_ZERO]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x19: /*BNZ (branch if register(op1)!=0 (not zero) (with jumpstack))*/
		if (flags.flaglist[CND_NOT_ZERO]){
			if (flags.flaglist[ALLOW_BRANCH_STACK])
				jumpstack.push(program_counter + 1);
			set_programcounter(op1);
		}
		break;
	case 0x1A: /*CALL (incondicional branch WITH jumpstack)*/
		jumpstack.push(program_counter + 1);
		set_programcounter(op1);
		break;
	case 0x1B: /*RET (return)*/
		set_programcounter(jumpstack.pop());
		break;
	case 0x1C: /*JMP (jump (incondicional branch WITHOUT jumpstack))*/
		set_programcounter(op1);
		break;
		/*IMPLEMENT LOGIC AND ARITHMETIC (CLASSICAL) CALCULATIONS HERE*/
	case 0x1D: /*ADD*/
		qmem.set_register(op1, qmem.fetch_register(op2) + qmem.fetch_register(op3));
		break;
	case 0x1E: /*ADD RK (add reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) + op3);
		break;
	case 0x1F: /*SUB*/
		qmem.set_register(op1, qmem.fetch_register(op2) - qmem.fetch_register(op3));
		break;
	case 0x20: /*SUB RK (sub reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) - op3);
		break;
	case 0x21: /*SUB KR (sub const reg)*/
		qmem.set_register(op1, op2 - qmem.fetch_register(op3));
		break;
	case 0x22: /*MUL*/
		qmem.set_register(op1, qmem.fetch_register(op2)*qmem.fetch_register(op3));
		break;
	case 0x23: /*MUL RK (mul reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2)*op3);
		break;
	case 0x24: /*DIV*/
		qmem.set_register(op1, qmem.fetch_register(op2) / qmem.fetch_register(op3));
		break;
	case 0x25: /*DIV RK (div reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2)*1.0f / op3);
		break;
	case 0x26: /*DIV KR (div const reg)*/
		qmem.set_register(op1, op2*1.0f / qmem.fetch_register(op3));
		break;
	case 0x27: /*AND*/
		qmem.set_register(op1, qmem.fetch_register(op2)&qmem.fetch_register(op3));
		break;
	case 0x28: /*AND RK (and reg const)*/
		sram.write(op1, qmem.fetch_register(op2)&op3);
		break;
	case 0x29: /*AND KR (and const reg)*/
		qmem.set_register(op1, op2&qmem.fetch_register(op3));
		break;
	case 0x2A: /*OR*/
		qmem.set_register(op1, qmem.fetch_register(op2) | qmem.fetch_register(op3));
		break;
	case 0x2B: /*OR RK (or reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) | op3);
		break;
	case 0x2C: /*OR KR (or const reg)*/
		qmem.set_register(op1, op2 | qmem.fetch_register(op3));
		break;
	case 0x2D: /*NOR*/
		qmem.set_register(op1, ~(qmem.fetch_register(op2) | qmem.fetch_register(op3)));
		break;
	case 0x2E: /*NOR RK (nor reg const)*/
		qmem.set_register(op1, ~(qmem.fetch_register(op2) | op3));
		break;
	case 0x2F: /*NOR KR (nor const reg)*/
		qmem.set_register(op1, ~(op2 | qmem.fetch_register(op3)));
		break;
	case 0x30: /*XOR*/
		qmem.set_register(op1, qmem.fetch_register(op2) ^ qmem.fetch_register(op3));
		break;
	case 0x31: /*XOR RK (xor reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) ^ op3);
		break;
	case 0x32: /*XOR KR (xor const reg)*/
		qmem.set_register(op1, op2^qmem.fetch_register(op3));
		break;
	case 0x33: /*NAN*/
		qmem.set_register(op1, ~(qmem.fetch_register(op2)&qmem.fetch_register(op3)));
		break;
	case 0x34: /*NAN RK (nand reg const)*/
		qmem.set_register(op1, ~(qmem.fetch_register(op2)&op3));
		break;
	case 0x35: /*NAN KR (nand const reg)*/
		qmem.set_register(op1, ~(op2&qmem.fetch_register(op3)));
		break;
	case 0x36: /*NOT*/
		qmem.set_register(op1, ~qmem.fetch_register(op2));
		break;
	case 0x37: /*SHL*/
		qmem.set_register(op1, qmem.fetch_register(op2) << qmem.fetch_register(op3));
		break;
	case 0x38: /*SHL RK (shl reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) << op3);
		break;
	case 0x39: /*SHL KR (shl const reg)*/
		qmem.set_register(op1, op2 << qmem.fetch_register(op2));
		break;
	case 0x3A: /*SHR*/
		sram.write(op1, qmem.fetch_register(op2) >> qmem.fetch_register(op3));
		break;
	case 0x3B: /*SHR RK (shr reg const)*/
		qmem.set_register(op1, qmem.fetch_register(op2) >> op3);
		break;
	case 0x3C: /*SHR KR (shr const reg)*/
		qmem.set_register(op1, op2 >> qmem.fetch_register(op2));
		break;
	case 0x3D: /*INT (interrupt)*/ //NEEDS TABLE SYSTEM
		interrupt_cpu(op1);
		break;
	case 0x3E: /*DLY (delay)*/
		utils.delay(op1);
		break;
	case 0x3F: /*NOP (nop) - DOES NOTHING*/
		break;
	case 0x40: /*HLT (halt)*/
		program_counter = program_counter_maximum;
		break;
		//QUANTUM FUNCTIONS:
		//1 QUBIT GATES -
	case 0x41: // X GATE
		{
			newthephi = new int[2]; newthephi = gates.X(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x42: // Y GATE
		{
			newthephi = new int[2]; newthephi = gates.Y(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x43: // Z GATE
		{
			newthephi = new int[2]; newthephi = gates.Z(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x44: // H GATE
		{
			newthephi = new int[2]; newthephi = gates.H(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x45: // S GATE
		{
			newthephi = new int[2]; newthephi = gates.S(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x46: // T GATE
		{
			newthephi = new int[2]; newthephi = gates.T(qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x47: // ROTATE X GATE
		{
			newthephi = new int[2]; newthephi = gates.ROX(qmem.read(op1, THE), qmem.read(op1, PHI), op2);
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x48: // ROTATE Y GATE
		{
			newthephi = new int[2]; newthephi = gates.ROY(qmem.read(op1, THE), qmem.read(op1, PHI), op2);
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
	case 0x49: // ROTATE Z GATE
		{
			newthephi = new int[2]; newthephi = gates.ROZ(qmem.read(op1, THE), qmem.read(op1, PHI), op2);
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
		}
		break;
		//2 QUBIT GATES -
	case 0x4A: // CNOT GATE
		{
			newthephi = new int[4]; newthephi = gates.CNO(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x4B: // CSIGN GATE
		{
			newthephi = new int[4]; newthephi = gates.CSI(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x4C: // SWAP GATE
		{
			newthephi = new int[4]; newthephi = gates.SWA(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x4D: // INCREMENT GATE
		{
			newthephi = new int[4]; newthephi = gates.INC(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x4E: // DECREMENT GATE
		{
			newthephi = new int[4]; newthephi = gates.DEC(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x4F: // SWAGSQ GATE
		{
			newthephi = new int[4]; newthephi = gates.SWQ(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
	case 0x50: // SWAPI GATE
		{
			newthephi = new int[4]; newthephi = gates.SWI(qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
		}
		break;
		//3 QUBIT GATES -
	case 0x51: // CONTROL SWAP GATE
		{
			newthephi = new int[6]; newthephi = gates.CSW(qmem.read(op3, THE), qmem.read(op3, PHI), qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
			qmem.write(op3, THE, newthephi[4]); qmem.write(op3, PHI, newthephi[5]);
		}
		break;
	case 0x52: // TOFFOLI GATE
		{
			newthephi = new int[6]; newthephi = gates.TOF(qmem.read(op3, THE), qmem.read(op3, PHI), qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
			qmem.write(op3, THE, newthephi[4]); qmem.write(op3, PHI, newthephi[5]);
		}
		break;
	case 0x53: // DEUTSCH GATE
		{
			newthephi = new int[6]; newthephi = gates.DEU(qmem.read(op3, THE), qmem.read(op3, PHI), qmem.read(op2, THE), qmem.read(op2, PHI), qmem.read(op1, THE), qmem.read(op1, PHI), qmem.read(0, THE));
			qmem.write(op1, THE, newthephi[0]); qmem.write(op1, PHI, newthephi[1]);
			qmem.write(op2, THE, newthephi[2]); qmem.write(op2, PHI, newthephi[3]);
			qmem.write(op3, THE, newthephi[4]); qmem.write(op3, PHI, newthephi[5]);
		}
		break;
	default: /*THIS FUNCTION DOES NOT EXIST*/
		break;
	}
	delete[] newthephi;
}
