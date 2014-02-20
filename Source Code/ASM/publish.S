#include <machine/asm.h>

IMPORT(_publish)
ENTRY(publish)
	jmp	_C_LABEL(_publish)

