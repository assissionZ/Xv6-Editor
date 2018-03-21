#include "syscall.h"
#include "traps.h"

.data
init:
	.string "/init"
argv:
	.long init
	.long 0

.text
.globl main
main:
	pushl argv
	pushl init
	movl $SYS_exec, %eax
	int $T_SYSCALL

