QEPU_HardwareSimulator
======================

This is a software implementation (a simulator) of the hardware to be used in the QEPU Project

get stdio.qasm
int2str:
	push {0}
	push {1}
	push {2}
	;;;;;;;;
	
	div {1},{0},10
	mov {2},{1}
	mul {1},{1},10
	sub {1},{0},{1}
	mov {0},{2}
	add {1},{1},48
	push {1}
	
	div {1},{0},10
	mov {2},{1}
	mul {1},{1},10
	sub {1},{0},{1}
	mov {0},{2}
	add {1},{1},48
	push {1}
	
	pop {0>>1}
	mov [($n+0>>1)],{0>>1}
	
	;;;;;;;;
	pop {2}
	pop {1}
	pop {0}
ret

main:
	$n,90
	
	mov {0},[$n]
	call @int2str
	mov {0},$n
	int 1
	
hlt
