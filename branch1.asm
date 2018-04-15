	.org	0x50
	.entry	main
main:
	lui	r22,#0xffff
	lui	r23,#0x7fff
	j	try1
	nop
;
	.org	0x100
try1:
	beq	r22,r3,bad1
	nop
	nop
	nop
	lui	r11,#0xace
	j	try2
	nop
	nop
	nop
bad1:	lui	r11,#0xbad
	j	try2
	nop
;
	.org	0x200
try2:	nop
	nop
	beq	r22,r22,good2
	nop
	nop
	nop
	lui	r12,#0xbad
	j	try3
	nop
	nop
	nop
good2:	lui	r12,#0xace
	j	try3
	nop
;
	.org	0x300
try3:	nop
	nop
	bne	r22,r3,good3
	nop
	nop
	nop
	lui	r13,#0xbad
	j	try4
	nop
	nop
	nop
good3:	lui	r13,#0xace
	j	try4
	nop
;
	.org	0x400
try4:	nop
	nop
	bne	r22,r22,bad4
	nop
	nop
	nop
	lui	r14,#0xace
	j	out
	nop
	nop
	nop
bad4:	lui	r14,#0xbad
	j	out
	nop
;
	.org	0x500
out:	break
	halt
