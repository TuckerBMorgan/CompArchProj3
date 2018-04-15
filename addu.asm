	.org	0x50
low:	.word	0xffff
one:	.word	0x1
	.org	0x100
	.entry	main
main:
	lui	r1,#0x7fff
	lw	r8,0x54(r0)
	lw	r9,0x50(r0)
	nop
	nop
	nop
;
	addu	r7,r1,r0
	addu	r10,r1,r9
	nop
	nop
	nop
	addu	r12,r7,r10
;
	halt
