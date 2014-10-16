#**Project QEPU**  ######(Quantum Electronic Processing Unit)
###Description: 32 bit quantum processor
---
###**QEPU INSTRUCTION SET (109 Functions):**
* ###Data movement: 
>**1.** MOQ - Move qubit (from one qubit to another)  
**2.** MOR - Move register  
**3.** MOM - Move memory  
**4.** MORI - Move register indirect (using pointers)  
**5.** MORD - Move register indirect destination (using pointers)  
**6.** STORE - Store from registers to memory  
**7.** LOAD - Load from memory to registers  
**8.** CMW - Constant memory write (writes a constant on memory)  
**9.** CRW - Constant register write (writes a constant on a register)  
**10.** POP - Pops a value from the memory to a register  
**11.** POPA - Pops values from the memory stack to all registers  
**12.** PUSH - Pushes a value from the register to the memory stack  
**13.** PUSHA - Pushes all registers to the memory stack  
**14.** CMTH - Constant move theta (sets the dimension theta of a specified qubit to a constant angle)  
**15.** CMPH - Constant move phi (sets the dimension phi of a specified qubit to a constant angle)  

* ###Logic and functions: 
>**16.** CMP - Compare (compares two registers) 
**17.** SEA - Set attribute (sets flags, stack head and base and more in the future)  
**18.** GEA - Get attribute (gets flags, stack head and base and more in the future)  
**19.** FLC - Flag Clear (empties all flags)  
**20.** BES - Branch enable stack (enables the callstack for branching)  
**21.** BLW - Branch if lower  
**22.** BLE - Branch if lower or equal  
**23.** BEQ - Branch if equal  
**24.** BGE - Branch if greater or equal  
**25.** BGR - Branch if greater  
**26.** BDI - Branch if different  
**27.** BZE - Branch if zero  
**28.** BNZ - Branch if not zero  
**29.** BLWR - Branch if lower (uses register as a label pointer)  
**30.** BLER - Branch if lower or equal (uses register as a label pointer)  
**31.** BEQR - Branch if equal (uses register as a label pointer)  
**32.** BGER - Branch if greater or equal (uses register as a label pointer)  
**33.** BGRR - Branch if greater (uses register as a label pointer)  
**34.** BDIR - Branch if different (uses register as a label pointer)  
**35.** BZER - Branch if zero (uses register as a label pointer)  
**36.** BNZR - Branch if not zero  
**37.** CALL - Call jumps to a label incondicionally (uses callstack [or jumpstack, named by me])  
**38.** RET - Ret function returns from a call incondicionally (uses callstack [or jumpstack, named by me])  
**39.** JMP - Jumps incondicionally  
**40.** JMPR - Jumps incondicionally (uses register as a label pointer)  
**41.** CALLR - Call jumps to a label incondicionally (uses register as a label pointer and callstack [or jumpstack, named by me])  

* ###Arithmetic:  
>**42.** ADD_ - Adds two registers (Ra = Rb + Rc) (unsigned)  
**43.** ADD_RK - Adds one register (Ra = Rb + K) (unsigned)  
**44.** INC - Increments a register  
**45.** DEC - Decrements a register  
**46.** ABS - Absolute value of a number (unsigned [forced])  
**47.** MOD_ - Modulus (remainder of a division between two registers -  Ra = Rb % Rc) (unsigned)  
**48.** MOD_RK - Modulus (remainder of a division between one register -  Ra = Rb % K) (unsigned)  
**49.** MOD_KR - Modulus (remainder of a division between one register - Ra = K % Rc) (unsigned)  
**50.** SUB_ - Subtracts two registers (Ra = Rb - Rc) (unsigned)  
**51.** SUB_RK - Subtracts one register (Ra = Rb - K) (unsigned)  
**52.** SUB_KR - Substracts one register (Ra= K - Rc) (unsigned)  
**53.** MUL_ - Multiplies two registes (Ra = Rb * Rc) (unsigned)  
**54.** MUL_RK - Multiplies one register (Ra = Rb * K) (unsigned)  
**55.** DIV_ - Divides two registers (Ra = Rb / Rc) (unsigned)  
**56.** DIV_RK - Divides one register (Ra = Rb / K) (unsigned)  
**57.** DIV_KR - Divides one register (Ra = K / Rc) (unsigned)  
**58.** AND_ - Ands two registers (Ra = Rb & Rc)  
**59.** AND_RK - Ands one register (Ra = Rb & K)  
**60.** AND_KR - Ands one register (Ra = K & Rc)  
**61.** OR_ - Ors two registers (Ra = Rb | Rc)  
**62.** OR_RK - Ors one register (Ra = Rb | K)  
**63.** OR_KR - Ors one register (Ra = K | Rc)  
**64.** NOR_ - Nors two registers (Ra = Rb !| Rc)  
**65.** NOR_RK - Nors one register (Ra = Rb !| K)  
**66.** NOR_KR - Nors one register (Ra = K !| Rc)  
**67.** XOR_ - Xors two registers (Ra = Rb xor Rc)  
**68.** XOR_RK - Xors one register (Ra = Rb xor K)  
**69.** XOR_KR - Xors one register (Ra = K xor Rc)  
**70.** NAND_ - Nands two registers (Ra = Rb !& Rc)  
**71.** NAND_RK - Nands one register (Ra = Rb !& K)  
**72.** NAND_KR - Nands one register (Ra = K !& Rc)  
**73.** NOT - Nots a register  
**74.** SHL_ - Shifts left a register (Ra = Rb << Rc)  
**75.** SHL_RK - Shifts left a register with K (Ra = Rb << K)  
**76.** SHL_KR - Shifts left a register with K (Ra = K << Rc)  
**77.** SHR_ - Shifts right a register (Ra = Rb >> Rc)  
**78.** SHR_RK - Shifts right a register with K (Ra = Rb >> K)  
**79**. SHR_KR - Shifts right a register with K (Ra = K >> Rc)  
**80.** NEG - Negates a register  
**81.** ROL_ - Rols left a register (Ra = Rb rol Rc)  
**82.** ROL_RK - Rols left a register with K (Ra =  Rb rol K)  
**83.** ROL_KR - Rols left a register with K (Ra = K rol Rc)  
**84.** ROR_ - Rols right a register (Ra = Rb ror Rc)  
**85.** ROR_RK - Rols right a register with K (Ra =  Rb ror K)  
**86.** ROR_KR - Rols right a register with K (Ra = K ror Rc)  

* ###Misc and system functions:  
>**87.** INT - Calls a system interrupt  
**88.** DELAY - Delays the machine in miliseconds  
**89.** NOP - Does nope  
**90.** HLT - Halts the machine  

* ###Quantum functions  
>**91.** X - Quantum X gate  
**92.** Y - Quantum Y gate  
**93.** Z - Quantum Z gate  
**94.** H - Quantum Hadamard gate  
**95.** S - Quantum Phase S gate  
**96.** T - Quantum Phase T gate  
**97.** ROX - Quantum Rotate X gate  
**98.** ROY - Quantum Rotate Y gate  
**99.** ROZ - Quantum Rotate Z gate  
***100*.** CNOT - Quantum controlled not gate  
**101.** CSIGN - Quantum csign gate  
**102.** SWAP - Quantum swap gate  
**103.** QINC - Quantum increment gate  
**104.** QDEC - Quantum decrement gate  
**105.** SWAPQ - Quantum swap square gate  
**106.** SWAPI - Quantum swap imaginary gate  
**107.** CSWAP - Quantum controlled swap gate (fredkin gate)  
**108.** TOFF - Quantum toffoli gate  
**109.** DEU - Quantum deutsch gate  

----------

Sintaxes: 
------------
```
Coming soon . . . 
```


----------


Instruction required sizes:
------------
```
Coming soon . . .
```


