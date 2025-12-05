/* Initial boot
 *
 * Copyright (C) 2025 Chris January
 * Copyright (c) 2006 CodeSourcery Inc
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

	#include "asm.h"	
	.text

#ifdef ROM
	.extern __stack
#endif
	.extern __start1

	/* __reset should cause a HALT in a hosted executable and
	   fall into __start for an unhosted executable.  The user is
	   free to override this with their own declaration.  */
	.globl __reset
	.weak __reset
__reset:	

	.globl __start
__start:
	/* Update POST code */
	move.l IMM(0x80000c),a0
	moveb #4,a0@

#ifdef ROM
    /* Initialize stack */
	move.l IMM(__stack), sp
#endif
	move.l IMM(0), fp

	move.l sp,sp@-
	move.l fp,sp@-  /* Dummy return address */
	jmp __start1
