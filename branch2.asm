;
; this is the same program as in the branch1.asm test files, but
; this version takes advantage of the branch delay slots to perform
; some operations
;
	.org	0x50
	.entry	main
main:
	lui	r22,#0xffff
	j	try1
	lui	r23,#0x7fff	; delay slot
	nop
;
	.org	0x100
try1:	nop
	nop
	beq	r22,r3,bad1
	nop
	nop
	nop
	j	try2
	lui	r11,#0xace	; delay slot
	nop
	nop
bad1:	j	try2
	lui	r11,#0xbad	; delay slot
	nop
;
	.org	0x200
try2:	nop
	nop
	beq	r22,r22,good2
	nop
	nop
	nop
	j	try3
	lui	r12,#0xbad	; delay slot
	nop
	nop
good2:	j	try3
	lui	r12,#0xace	; delay slot
	nop
;
	.org	0x300
try3:	nop
	nop
	bne	r22,r3,good3
	nop
	nop
	nop
	j	try4
	lui	r13,#0xbad	; delay slot
	nop
	nop
good3:	j	try4
	lui	r13,#0xace	; delay slot
	nop
;
	.org	0x400
try4:	nop
	nop
	bne	r22,r22,bad4
	nop
	nop
	nop
	j	out
	lui	r14,#0xace	; delay slot
	nop
	nop
bad4:	j	out
	lui	r14,#0xbad	; delay slot
	nop
;
	.org	0x500
out:	break
	halt
