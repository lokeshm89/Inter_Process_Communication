#include <machine/asm.h>

IMPORT(_retrieve)
ENTRY(retrieve)
	jmp	_C_LABEL(_retrieve)

