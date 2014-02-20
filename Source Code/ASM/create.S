#include <machine/asm.h>

IMPORT(_create)
ENTRY(create)
	jmp	_C_LABEL(_create)

