#include <machine/asm.h>

IMPORT(_subscribe)
ENTRY(subscribe)
	jmp	_C_LABEL(_subscribe)

