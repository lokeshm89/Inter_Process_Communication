#include <machine/asm.h>

IMPORT(_delete_)
ENTRY(delete_)
	jmp	_C_LABEL(_delete_)

