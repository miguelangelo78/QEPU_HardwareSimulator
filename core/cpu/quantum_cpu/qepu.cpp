#include "../../../stdafx.h"
#include "qepu.h"

QEPU::QEPU(char*eeprom_filepath) :eeprom(eeprom_filepath), gates(&reg_in.get_quantummemory()){ setup_seed(); program_counter = 0; program_counter_maximum = 0; }

void QEPU::setup_seed(){
	srand(time(NULL));
}

void QEPU::set_programcounter(int newaddress){
	program_counter = newaddress-1;
}

void QEPU::run(){
	int line_width = INSTR_WIDTH;
	program_counter_maximum = eeprom.count_lines();
	for (program_counter = 0; program_counter<program_counter_maximum; program_counter++){
		//FETCH OPERANDS FROM THE EEPROM:
		int eeprom_line_selection = program_counter*line_width;
		int func = 0;	
		char op1s[9] = ""; char op1meta = 0; char op2s[9] = ""; char op2meta = 0; char op3s[9] = ""; char op3meta = 0; // SIZE 9 BECAUSE EACH OPERAND WILL HAVE 8 BITS OF SIZE
		
		func = eeprom.read(eeprom_line_selection + FIXED_FUNC_OFFSET); // FUNCTION FETCH
		op1meta = eeprom.read(eeprom_line_selection + FIXED_OP1_OFFSET + OP1_WIDTH);
		op2meta = eeprom.read(eeprom_line_selection + FIXED_OP2_OFFSET + OP2_WIDTH);
		op3meta = eeprom.read(eeprom_line_selection + FIXED_OP3_OFFSET + OP3_WIDTH);
		for (int k = 0; k<QUBIT_BYTE_SIZE; k++){
			sprintf(op1s, "%s%.2x", op1s, eeprom.read(eeprom_line_selection + FIXED_OP1_OFFSET + k)); // HEX CONCAT TO STRING (OP1 FETCH)
			sprintf(op2s, "%s%.2x", op2s, eeprom.read(eeprom_line_selection + FIXED_OP2_OFFSET + k)); // HEX CONCAT TO STRING (OP2 FETCH)
			sprintf(op3s, "%s%.2x", op3s, eeprom.read(eeprom_line_selection + FIXED_OP3_OFFSET + k)); // HEX CONCAT TO STRING (OP3 FETCH)
		}
		//EXECUTE:
		execute(func, strtoul(op1s, NULL, 16), op1meta, strtoul(op2s, NULL, 16), op2meta, strtoul(op3s, NULL, 16), op3meta); //INSTRUCTION DECODE AND EXECUTE
	}

	if (SHOW_LAST_STATE){
		reg_in.get_quantummemory().dumpmem(10);
		serial.writestrln("");
	}
	if (SHOW_SRAM) sram.dumpmem(30,true);
	
	serial.writestrln("");
	serial.writestr("The execution of the program has finished.");
	std::cin.get();
	#pragma endregion
}

void QEPU::execute(int func, intq op1, byte op1meta, intq op2, byte op2meta, intq op3, byte op3meta){
	if (DEBUG_MODE)
		std::cout	<< "Function: " << func
					<< " , OP1: " << op1
					<< " (" << (op1meta + 0) << ")"
					<< " , OP2: " << op2
					<< " (" << (op2meta + 0) << ")"
					<< " , OP3: " << op3
					<< " (" << (op3meta + 0) << ")"
					<< " , PC: " << program_counter
					<< " , Max PC: " << program_counter_maximum
					<< std::endl;
	
	int *newthephi = nullptr;
	int op1type = op1meta, op2type = op2meta, op3type = op3meta; // METADATA CURRENTLY REPRESENTS ONLY THE OPERAND TYPE, MIGHT REPRESENT MORE INFO IN THE FUTURE

	switch (func){
		case NOP: break;
		case MOV:
			//
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				reg_in.write(op1, THE, reg_in.read(op2, THE));
				reg_in.write(op1, PHI, reg_in.read(op2, PHI));
			}
			if (op1type == TYPE_QUBIT && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				reg_in.write(op1, THE, op2);
				reg_in.write(op1, PHI, op2);
			}
			if (op1type == TYPE_QUBIT_THETA && op2type == TYPE_QUBIT_THETA){
				reg_in.write(op1, THE, reg_in.read(op2, THE));
			}
			if (op1type == TYPE_QUBIT_THETA && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				reg_in.write(op1, THE, op2);
			}
			if (op1type == TYPE_QUBIT_PHI && op2type == TYPE_QUBIT_PHI){
				reg_in.write(op1, PHI, reg_in.read(op2, PHI));
			}
			if (op1type == TYPE_QUBIT_PHI && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				reg_in.write(op1, PHI, op2);
			}
			if(op1type == TYPE_QUBIT_THETA && op2type == TYPE_QUBIT_PHI){
				reg_in.write(op1, THE, reg_in.read(op2, PHI));
			}
			if (op1type == TYPE_QUBIT_PHI && op2type == TYPE_QUBIT_THETA){
				reg_in.write(op1, PHI, reg_in.read(op2, THE));
			}
			//

			//
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_MEM_CONTAINER)
				sram.write(op1, sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_CREG)
				sram.write(op1, reg_in.readid(ID_REG_CMEM,op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_QREG)
				sram.write( op1, reg_in.readid(ID_REG_QMEM,op2));
			
			if (op1type == TYPE_CREG && op2type == TYPE_MEM_CONTAINER)
				reg_in.writeid(ID_REG_CMEM,op1, sram.read(op2));
			
			if (op1type == TYPE_QREG && op2type == TYPE_MEM_CONTAINER)
				reg_in.writeid(ID_REG_QMEM,op1, sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_CREG_POINTER)
				sram.write(op1, sram.read(reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_QREG_POINTER)
				sram.write(op1, sram.read(reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_MEM_CONTAINER && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				sram.write(op1, op2);
			
			//

			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM,op1, reg_in.readid(ID_REG_CMEM, op2));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, op2);
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG_POINTER){
				reg_in.writeid(ID_REG_CMEM,op1, sram.read(reg_in.read(ID_REG_CMEM, op2)));
			}
			if (op1type == TYPE_CREG_POINTER && op2type == TYPE_CREG)
				sram.write(reg_in.readid(ID_REG_CMEM, op1), reg_in.readid(ID_REG_CMEM,op2));
			
			if (op1type == TYPE_CREG_POINTER && op2type == TYPE_MEM_CONTAINER)
				sram.write(reg_in.readid(ID_REG_CMEM,op1), sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_CREG_POINTER)
				sram.write(op1, sram.read(reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG_POINTER && op2type == TYPE_CREG_POINTER)
				sram.write(reg_in.readid(ID_REG_CMEM, op1), sram.read(reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG_POINTER && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				sram.write(reg_in.readid(ID_REG_CMEM,op1), op2);
			
			//

			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, op2);
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG_POINTER)
				reg_in.writeid(ID_REG_QMEM, op1, sram.read(reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_QREG_POINTER && op2type == TYPE_QREG)
				sram.write(reg_in.readid(ID_REG_QMEM, op1), reg_in.readid(ID_REG_QMEM,op2));
			
			if (op1type == TYPE_QREG_POINTER && op2type == TYPE_MEM_CONTAINER)
				sram.write(reg_in.readid(ID_REG_QMEM,op1), sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_QREG_POINTER)
				sram.write(op1, sram.read(reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_QREG_POINTER && op2type == TYPE_QREG_POINTER)
				sram.write(reg_in.readid(ID_REG_QMEM, op1), sram.read(reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_QREG_POINTER && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				sram.write(reg_in.readid(ID_REG_QMEM,op1), op2);
			
			//

			//
			if (op1type == TYPE_CREG && op2type == TYPE_QREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2));
			
			if (op1type == TYPE_QREG && op2type == TYPE_CREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG_POINTER)
				reg_in.writeid(ID_REG_CMEM, op1, sram.read(reg_in.readid(ID_REG_QMEM, op2)));
			
			if (op1type == TYPE_QREG && op2type == TYPE_CREG_POINTER)
				reg_in.writeid(ID_REG_QMEM, op1, sram.read(reg_in.readid(ID_REG_CMEM, op2)));
			
			//

			//
			if (op1type == TYPE_STACKHEAD && op2type == TYPE_STACKBASE)
				sram.set_stack_head(sram.get_stack_base());
			
			if (op1type == TYPE_STACKBASE && op2type == TYPE_STACKHEAD)
				sram.set_stack_base(sram.get_stack_head());

			if (op1type == TYPE_STACKHEAD && op2type == TYPE_CREG)
				sram.set_stack_head(reg_in.readid(ID_REG_CMEM,op2));
			
			if (op1type == TYPE_CREG && op2type == TYPE_STACKHEAD)
				reg_in.writeid(ID_REG_CMEM,op1, sram.get_stack_head());
			
			if (op1type == TYPE_STACKHEAD && op2type == TYPE_CREG_POINTER)
				sram.set_stack_head(sram.read(reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG_POINTER && op2type == TYPE_STACKHEAD)
				sram.write(reg_in.readid(ID_REG_CMEM,op1), sram.get_stack_head());
			
			if (op1type == TYPE_STACKHEAD && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				sram.set_stack_head(op2);

			//

			if (op1type == TYPE_STACKBASE && op2type == TYPE_CREG)
				sram.set_stack_base(reg_in.readid(ID_REG_CMEM,op2));
			
			if (op1type == TYPE_CREG && op2type == TYPE_STACKBASE)
				reg_in.writeid(ID_REG_CMEM,op1, sram.get_stack_base());
			
			if (op1type == TYPE_STACKBASE && op2type == TYPE_CREG_POINTER)
				sram.set_stack_base(sram.read(reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG_POINTER && op2type == TYPE_STACKBASE)
				sram.write(reg_in.readid(ID_REG_CMEM,op1), sram.get_stack_base());
			
			if (op1type == TYPE_STACKBASE && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				sram.set_stack_base(op2);

			break;
		case POP:
			if (op1type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM,op1, sram.pop());
			
			if (op1type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM,op1, sram.pop());
			
			if (op1type == TYPE_MEM_CONTAINER)
				sram.write(op1, sram.pop());

			if (op1type == TYPE_CREG_POINTER)
				sram.write(reg_in.readid(ID_REG_CMEM,op1), sram.pop());
			
			if (op1type == TYPE_QREG_POINTER)
				sram.write(reg_in.readid(ID_REG_QMEM,op1), sram.pop());
			
			if (op1type == TYPE_STACKHEAD)
				sram.set_stack_head(sram.pop());
			if (op1type == TYPE_STACKBASE)
				sram.set_stack_base(sram.pop());
			break;
		case POPA:
			for (int i = REGISTER_COUNT - 1; i >= 0; i--) reg_in.writeid(ID_REG_CMEM,i, sram.pop());
			break;
		case POPAQ:
			for (int i = REGISTER_COUNT - 1; i >= 0; i--) reg_in.writeid(ID_REG_QMEM,i, sram.pop());
			break;
		case PUSH:
			if (op1type == TYPE_CREG)
				sram.push(reg_in.readid(ID_REG_CMEM,op1));
			
			if (op1type == TYPE_QREG)
				sram.push(reg_in.readid(ID_REG_QMEM,op1));
			
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				sram.push(op1);
			
			if (op1type == TYPE_MEM_CONTAINER)
				sram.push(sram.read(op1));
			
			if (op1type == TYPE_CREG_POINTER)
				sram.push(sram.read(reg_in.readid(ID_REG_CMEM,op1)));
			
			if (op1type == TYPE_QREG_POINTER)
				sram.push(sram.read(reg_in.readid(ID_REG_QMEM,op1)));
			
			if (op1type == TYPE_STACKHEAD)
				sram.push(sram.get_stack_head());

			if (op1type == TYPE_STACKBASE)
				sram.push(sram.get_stack_base());
			break;
		case PUSHA:
			for (int i = 0; i < REGISTER_COUNT; i++) sram.push(reg_in.readid(ID_REG_CMEM,i));
			break;
		case PUSHAQ:
			for (int i = 0; i < REGISTER_COUNT; i++) sram.push(reg_in.readid(ID_REG_QMEM,i));
			break;
		case CMP:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG)
				flags.compare(reg_in.readid(ID_REG_CMEM,op1), reg_in.readid(ID_REG_CMEM, op2));
			
			if (op1type == TYPE_CREG && op2type == TYPE_MEM_CONTAINER)
				flags.compare(reg_in.readid(ID_REG_CMEM,op1), sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_CREG)
				flags.compare(sram.read(op1), reg_in.readid(ID_REG_CMEM,op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_MEM_CONTAINER)
				flags.compare(sram.read(op1), sram.read(op2));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				flags.compare(reg_in.readid(ID_REG_CMEM,op1), op2);
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG)
				flags.compare(reg_in.readid(ID_REG_QMEM, op1), reg_in.readid(ID_REG_QMEM,op2));
			
			if (op1type == TYPE_QREG && op2type == TYPE_MEM_CONTAINER)
				flags.compare(reg_in.readid(ID_REG_QMEM,op1), sram.read(op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_QREG)
				flags.compare(sram.read(op1), reg_in.readid(ID_REG_QMEM,op2));
			
			if (op1type == TYPE_MEM_CONTAINER && op2type == TYPE_MEM_CONTAINER)
				flags.compare(sram.read(op1), sram.read(op2));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				flags.compare(reg_in.readid(ID_REG_QMEM,op1), op2);
			
			//
			if (op1type == TYPE_CREG && op2type == TYPE_QREG)
				flags.compare(reg_in.readid(ID_REG_CMEM, op1), reg_in.readid(ID_REG_QMEM, op2));
			
			if (op1type == TYPE_QREG && op2type == TYPE_CREG)
				flags.compare(reg_in.readid(ID_REG_QMEM, op1), reg_in.readid(ID_REG_CMEM, op2));
			
			//
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS) && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				flags.compare(op1, op2);
			break;
		case SET:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				set_attribute(op1);
			break;
		case GET:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				get_attribute(op1);
			break;
		case BRC: // CONDITION IS INSIDE A REGISTER
			if (op1type == TYPE_CREG || op1type == TYPE_QREG || op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS){
				intq condition = reg_in.readid(ID_REG_CMEM,0); // GET CONDITION FROM CLASSICAL REGISTER 0 (AX)
				
				int address_to_jmp = 0;
				if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS)) address_to_jmp = op1; // LABEL TO JUMP IS CONSTANT
				else if (op1type == TYPE_CREG) address_to_jmp = reg_in.readid(ID_REG_CMEM,op1); // LABEL TO JUMP IS INSIDE CLASSICAL REGISTER
				else if (op1type == TYPE_QREG) address_to_jmp = reg_in.readid(ID_REG_QMEM,op1);  // LABEL TO JUMP IS INSIDE QUANTUM REGISTER

				switch (condition){
					case CND_LWER:
						if (flags.flaglist[CND_LWER]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_LWER_EQUAL:
						if (flags.flaglist[CND_LWER_EQUAL]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_EQUAL:
						if (flags.flaglist[CND_EQUAL]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_GRTER_EQUAL:
						if (flags.flaglist[CND_GRTER_EQUAL]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_GRTER:
						if (flags.flaglist[CND_GRTER]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_DIFF:
						if (flags.flaglist[CND_DIFF]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_ZERO:
						if (flags.flaglist[CND_ZERO]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					case CND_NOT_ZERO:
						if (flags.flaglist[CND_NOT_ZERO]){
							if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
							set_programcounter(address_to_jmp);
						}
						break;
					default: break;
				}
			}
			break;
		case BLW:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_LWER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_LWER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_LWER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BLE:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_LWER_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_LWER_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_LWER_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BEQ:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BGE:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_GRTER_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_EQUAL]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BGR:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_GRTER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_GRTER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_GRTER]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BDI:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_DIFF]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_DIFF]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_DIFF]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BZE:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			break;
		case BNZ:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				if (flags.flaglist[CND_NOT_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(op1);
				}

			if (op1type == TYPE_CREG)
				if (flags.flaglist[CND_NOT_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
				}
			
			if (op1type == TYPE_QREG)
				if (flags.flaglist[CND_NOT_ZERO]){
					if (flags.flaglist[ALLOW_BRANCH_STACK])	jumpstack.push(program_counter + 1);
					set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
				}
			
			break;
		case CALL:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS)){
				jumpstack.push(program_counter + 1);
				set_programcounter(op1);
			}
			
			if (op1type == TYPE_CREG){
				jumpstack.push(program_counter + 1);
				set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
			}
			
			if (op1type == TYPE_QREG){
				jumpstack.push(program_counter + 1);
				set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
			}
			break;
		case RET:
			set_programcounter(jumpstack.pop());
			break;
		case JMP:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				set_programcounter(op1);
			
			if (op1type == TYPE_CREG)
				set_programcounter(reg_in.readid(ID_REG_CMEM,op1));
			
			if (op1type == TYPE_QREG)
				set_programcounter(reg_in.readid(ID_REG_QMEM,op1));
			break;
		case ADD:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM,op1, reg_in.readid(ID_REG_CMEM, op2) + reg_in.read(ID_REG_CMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) + op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 + reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, op2 + op3);
			//
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) + reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) + reg_in.readid(ID_REG_CMEM, op3));
			//
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) + reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) + op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 + reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, op2 + op3);
			break;
		case SUB:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) - reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) - op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 - reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 - op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) - reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) - reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) - reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) - op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 - reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 - op3);
			break;
		case MUL:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) * reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) * op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 * reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 * op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) * reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) * reg_in.readid(ID_REG_CMEM, op3));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) * reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) * op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 * reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 * op3);
			break;
		case DIV:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) / reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) / op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 / reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 / op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) / reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) / reg_in.readid(ID_REG_CMEM, op3));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) / reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) / op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 / reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 / op3);
			break;
		case INC_:
			if (op1type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op1) + 1);
			
			if (op1type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op1) + 1);
			break;
		case DEC_:
			if (op1type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op1) - 1);
			
			if (op1type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op1) - 1);
			break;
		case ABS:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, abs((long) reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, abs((long) op2));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, abs((long) reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, abs((long) op2));
			break;
		case MOD:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.read(op2) % reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) % op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 % reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 % op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) % reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) % reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) % reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) % op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 % reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 % op3);
			break;
		case AND:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) & reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) & op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 & reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 & op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) & reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) & reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) & reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) & op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 & reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 & op3);
			break;
		case OR:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) | reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) | op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 | reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 | op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) | reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) | reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) | reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) | op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 | reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 | op3);
			break;
		case NOR:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_CMEM, op2) | reg_in.readid(ID_REG_CMEM,op3)));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_CMEM,op2) | op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(op2 | reg_in.readid(ID_REG_CMEM,op3)));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, ~(op2 | op3));
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG){
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_CMEM, op2) | reg_in.readid(ID_REG_QMEM, op3)));
			}
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_QMEM, op2) | reg_in.readid(ID_REG_CMEM, op3)));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_QMEM, op2) | reg_in.readid(ID_REG_QMEM,op3)));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_QMEM,op2) | op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~(op2 | reg_in.readid(ID_REG_QMEM,op3)));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, ~(op2 | op3));
			break;
		case XOR:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) ^ reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) ^ op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 ^ reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS)){
				reg_in.write(op1, op2 ^ op3);
			}
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) ^ reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) ^ reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) ^ reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) ^ op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 ^ reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 ^ op3);
			break;
		case NAND:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_CMEM, op2) & reg_in.readid(ID_REG_CMEM,op3)));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_CMEM,op2) & op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(op2 & reg_in.readid(ID_REG_CMEM,op3)));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, ~(op2 & op3));
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_CMEM, op2) & reg_in.readid(ID_REG_QMEM, op3)));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~(reg_in.readid(ID_REG_QMEM, op2) & reg_in.readid(ID_REG_CMEM, op3)));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_QMEM, op2) & reg_in.readid(ID_REG_QMEM,op3)));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, ~(reg_in.readid(ID_REG_QMEM,op2) & op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~(op2 & reg_in.readid(ID_REG_QMEM,op3)));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, ~(op2 & op3));
			break;
		case NOT:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~((long) reg_in.readid(ID_REG_CMEM,op2)));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, ~((long) op2));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~((long) reg_in.readid(ID_REG_QMEM,op2)));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, ~((long) op2));
			break;
		case SHL:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) << reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) << op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 << reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 << op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) << reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) << reg_in.readid(ID_REG_CMEM, op3));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) << reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) << op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 << reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 << op3);
			break;
		case SHR:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM, op2) >> reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_CMEM,op2) >> op3);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, op2 >> reg_in.readid(ID_REG_CMEM,op3));
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 >> op3);
			//
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_CMEM, op2) >> reg_in.readid(ID_REG_QMEM, op3));
			
			if (op1type == TYPE_CREG && op2type == TYPE_QREG && op3type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, reg_in.readid(ID_REG_QMEM, op2) >> reg_in.readid(ID_REG_CMEM, op3));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM, op2) >> reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM, op1, reg_in.readid(ID_REG_QMEM,op2) >> op3);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, op2 >> reg_in.readid(ID_REG_QMEM,op3));
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.write(op1, op2 >> op3);
			break;
		case NEG:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG)
				reg_in.writeid(ID_REG_CMEM, op1, ~((long) reg_in.readid(ID_REG_CMEM,op2)) + 1);
			
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, ~((long) op2) + 1);
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG)
				reg_in.writeid(ID_REG_QMEM, op1, ~((long) reg_in.readid(ID_REG_QMEM,op2)) + 1);
			
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, ~((long) op2) + 1);
			break;
		case ROL:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG){
				intq op2t = reg_in.readid(ID_REG_CMEM, op2); intq op3t = reg_in.readid(ID_REG_CMEM,op3);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2t << op3t) | (op2t >> (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS)){
				intq op2t = reg_in.readid(ID_REG_CMEM,op2);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2t << op3) | (op2t >> (NUMERIC_SYSTEM - op3))));
			}
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG){
				intq op3t = reg_in.readid(ID_REG_CMEM,op3);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2 << op3t) | (op2 >> (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, ((op2 << op3) | (op2 >> (NUMERIC_SYSTEM - op3))));
			
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG){
				intq op2t = reg_in.readid(ID_REG_QMEM, op2); intq op3t = reg_in.readid(ID_REG_QMEM,op3);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2t << op3t) | (op2t >> (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS)){
				intq op2t = reg_in.readid(ID_REG_QMEM,op2);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2t << op3) | (op2t >> (NUMERIC_SYSTEM - op3))));
			}
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG){
				intq op3t = reg_in.readid(ID_REG_QMEM,op3);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2 << op3t) | (op2 >> (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, ((op2 << op3) | (op2 >> (NUMERIC_SYSTEM - op3))));
			break;
		case ROR:
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && op3type == TYPE_CREG){
				intq op2t = reg_in.readid(ID_REG_CMEM, op2); intq op3t = reg_in.readid(ID_REG_CMEM,op3);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2t >> op3t) | (op2t << (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_CREG && op2type == TYPE_CREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS)){
				intq op2t = reg_in.readid(ID_REG_CMEM,op2);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2t >> op3) | (op2t << (NUMERIC_SYSTEM - op3))));
			}
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_CREG){
				intq op3t = reg_in.readid(ID_REG_CMEM,op3);
				reg_in.writeid(ID_REG_CMEM,op1, ((op2 >> op3t) | (op2 << (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_CREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_CMEM,op1, ((op2 >> op3) | (op2 << (NUMERIC_SYSTEM - op3))));
			//
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && op3type == TYPE_QREG){
				intq op2t = reg_in.readid(ID_REG_QMEM, op2); intq op3t = reg_in.readid(ID_REG_QMEM,op3);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2t >> op3t) | (op2t << (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_QREG && op2type == TYPE_QREG && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS)){
				intq op2t = reg_in.readid(ID_REG_QMEM,op2);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2t >> op3) | (op2t << (NUMERIC_SYSTEM - op3))));
			}
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && op3type == TYPE_QREG){
				intq op3t = reg_in.readid(ID_REG_QMEM,op3);
				reg_in.writeid(ID_REG_QMEM,op1, ((op2 >> op3t) | (op2 << (NUMERIC_SYSTEM - op3t))));
			}
			if (op1type == TYPE_QREG && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS) && (op3type == TYPE_CONSTANT || op3type == TYPE_MEM_ADDRESS))
				reg_in.writeid(ID_REG_QMEM,op1, ((op2 >> op3) | (op2 << (NUMERIC_SYSTEM - op3))));
			break;
		case INT:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				interrupt_cpu(op1);
			break;
		case DELAY:
			if ((op1type == TYPE_CONSTANT || op1type == TYPE_MEM_ADDRESS))
				utils.delay(op1);
			break;
		case HALT:
			program_counter = program_counter_maximum;
			break;
		case X_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.X(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case Y_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.Y(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case Z_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.Z(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case H_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.H(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case S_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.S(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case T_:
			if (op1type == TYPE_QUBIT){
				newthephi = new int[2]; newthephi = gates.T(reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case ROX_:
			if (op1type == TYPE_QUBIT && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				newthephi = new int[2]; newthephi = gates.ROX(reg_in.read(op1, THE), reg_in.read(op1, PHI), op2);
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case ROY_:
			if (op1type == TYPE_QUBIT && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				newthephi = new int[2]; newthephi = gates.ROY(reg_in.read(op1, THE), reg_in.read(op1, PHI), op2);
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case ROZ_:
			if (op1type == TYPE_QUBIT && (op2type == TYPE_CONSTANT || op2type == TYPE_MEM_ADDRESS)){
				newthephi = new int[2]; newthephi = gates.ROZ(reg_in.read(op1, THE), reg_in.read(op1, PHI), op2);
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
			}
			break;
		case CNOT:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.CNO(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case CSIGN:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.CSI(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case SWAP:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.SWA(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case QINC:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.INC(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case QDEC:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.DEC(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case SWAPSQ:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.SWQ(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case SWAPI:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT){
				newthephi = new int[4]; newthephi = gates.SWI(reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
			}
			break;
		case CSWAP:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT && op3type == TYPE_QUBIT){
				newthephi = new int[6]; newthephi = gates.CSW(reg_in.read(op3, THE), reg_in.read(op3, PHI), reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
				reg_in.write(op3, THE, newthephi[4]); reg_in.write(op3, PHI, newthephi[5]);
			}
			break;
		case TOFF:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT && op3type == TYPE_QUBIT){
				newthephi = new int[6]; newthephi = gates.TOF(reg_in.read(op3, THE), reg_in.read(op3, PHI), reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
				reg_in.write(op3, THE, newthephi[4]); reg_in.write(op3, PHI, newthephi[5]);
			}
			break;
		case DEU_:
			if (op1type == TYPE_QUBIT && op2type == TYPE_QUBIT && op3type == TYPE_QUBIT){
				newthephi = new int[6]; newthephi = gates.DEU(reg_in.read(op3, THE), reg_in.read(op3, PHI), reg_in.read(op2, THE), reg_in.read(op2, PHI), reg_in.read(op1, THE), reg_in.read(op1, PHI), reg_in.read(0, THE));
				reg_in.write(op1, THE, newthephi[0]); reg_in.write(op1, PHI, newthephi[1]);
				reg_in.write(op2, THE, newthephi[2]); reg_in.write(op2, PHI, newthephi[3]);
				reg_in.write(op3, THE, newthephi[4]); reg_in.write(op3, PHI, newthephi[5]);
			}
			break;
		default: break;
	}
	delete[] newthephi;
}