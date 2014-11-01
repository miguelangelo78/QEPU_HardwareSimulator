#**Project QEPU** - Hardware
###32 bit quantum processor
######(QEPU: Quantum Electronic Processing Unit) 
>![bloch sphere qubit representation](http://i.imgur.com/HtjlQ3l.png?1)  
	(bloch sphere qubit representation)

---
Intro:
------------
```
Coming soon . . .
```
__________
Potential:
------------
```
Coming soon . . .
```
__________
#**Specifications**:

###**INSTRUCTION SET (64 Functions):**
>###Data movement:
>**1.** MOV - Moves data between the registers and primary memory  
	**2.** PUSH - Pushes data to the stack in memory  
	**3.** PUSHA - Pushes all registers to the stack in memory  
	**4.** PUSHAQ - Pushes all quantum registers to the stack in memory  
	**5.** POP - Pops data from the stack to a register or another memory address  
	**6.** POPA - Pops data to all the registers  
	**7.** POPAQ - Pops data to all the quantum registers  
###Conditional and incondicional branching:
>**8.** CMP - Compares two values between registers and memory addresses  
	**9.** SET - Sets an attribute/property on the CPU  
	**10.** GET - Gets an attribute/property on the CPU  
	**11.** BRC - Conditional Branching which fetches the condition from a register (useful for functions and routines)  
	**12.** BLW - Branch to an address if flag 'LOWER' is set  
	**13.** BLE - Branch to an address if flag 'LOWER OR EQUAL' is set  
	**14.** BEQ - Branch to an address if flag 'EQUAL' is set  
	**15.** BGE - Branch to an address if flag 'GREATER OR EQUAL' is set  
	**16.** BGR - Branch to an address if flag 'GREATER' is set  
	**17.** BDI - Branch to an address if flag 'DIFFERENT' is set  
	**18.** BZE - Branch to an address if flag 'ZERO' is set  
	**19.** BNZ - Branch to an address if flag 'NOT ZERO' is set  
	**20.** CALL - Jumps inconditionally to an address and pushes the address from where it was called into a jumpstack/callstack  
	**21.** JMP - Jumps inconditionally without using a jumpstack/callstack  
	**22.** RET - Jumps to the address that is on top of the jumpstack/callstack by popping its value  
###Arithmetic:
>**23.** ADD - Adds **two** registers and stores the result in another register  
**24.** SUB - Subtracts **two** registers and stores the result in another register  
**25.** MUL- Multiplies **two** registers and stores the result in another register  
**26.** DIV - Divides **two** registers and stores the result in another register  
**27.** INC - Increments **one** register  
**28.** DEC - Decrements **one** register  
**29.** ABS - Applies absolute operator to **one** register and stores the result in another register  
**30.** MOD - Applies the modulus operator to **two** registers and stores the result in another  
**31.** NEG - Negates **one** register and stores the result in another register  
register  
###Logic:
>**32.** AND - "Ands" **two** registers and stores the result in another register  
**33.** OR -  "Ors" **two** registers and stores the result in another register  
**34.** NOR - "Nors" **two** registers and stores the result in another register  
**35.** XOR - "Xors" **two** registers and stores the result in another register  
**36.** NAND - "Nands" **two** registers and stores the result in another register  
**37.** NOT - Inverts **one** register and stores the result in another register  
**38.** SHL - Shifts left **one** register using another and stores the result in another register  
**39.** SHR - Shifts right **one** register using another and stores the result in another register  
**40.** ROL - Rotates left **one** register using another and stores the result in another register  
**41.** ROR - Rotates right **one** register using another and stores the result in another register  
###Misc and system functions/interrupts:
>**42.** INT - Calls a system interrupt  
**43.** DELAY - Delays the machine in miliseconds  
**44.** NOP - Does nope  
**45.** HALT - Halts the machine  
###Quantum functions:
>**46.** X - Quantum X gate  
**47.** Y - Quantum Y gate  
**48.** Z - Quantum Z gate  
**49.** H - Quantum Hadamard gate  
**50.** S - Quantum Phase S gate  
**51.** T - Quantum Phase T gate  
**52.** ROX - Quantum Rotate X gate  
**53.** ROY - Quantum Rotate Y gate  
**54.** ROZ - Quantum Rotate Z gate  
**55.** CNOT - Quantum controlled not gate  
**56.** CSIGN - Quantum csign gate  
**57.** SWAP - Quantum swap gate  
**58.** QINC - Quantum increment gate  
**59.** QDEC - Quantum decrement gate  
**60.** SWAPQ - Quantum swap square gate  
**61.** SWAPI - Quantum swap imaginary gate  
**62.** CSWAP - Quantum controlled swap gate (fredkin gate)  
**63.** TOFF - Quantum toffoli gate  
**64.** DEU - Quantum deutsch gate  

----------
Sintaxes:
------------
```
Coming soon . . . 
```
----------
System properties:
------------
```
Coming soon . . . 
```
----------
Available registers:
------------
```
Coming soon . . . 
```
----------
Primary memory properties:
------------
```
Coming soon . . . 
```
----------
Operator sizes and types:
------------
```
Coming soon . . .
```
----------
Available casts:
------------
```
Coming soon . . .
```
__________
I/O:
------------
```
Coming soon . . .
```
__________
Data storage (permanent and volatile):
------------
```
Coming soon . . .
```
__________
Quantum computation:
------------
```
Coming soon . . .
```
__________
Limitations:
------------
```
Coming soon . . .
```
__________
Example code:
------------
```
Coming soon . . .
```
__________
The future:
------------
```
Coming soon . . . 
```
----------
About:
------------
```
Coming soon . . . 
```
----------

