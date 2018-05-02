	.org	0x50
low:	.word	0xffff
one:	.word	0x2
	.org	0x200
	.entry	main
main:
	lw	r8,0x54(r0)
	nop
	add	r10,r8,r8
	lw	r9,0x50(r0)
	lui	r1,#0x02
	nop
	nop
	nop
;
	lw	r7,0x54(r0)
	nop
	add r10, r7, r7
	nop
	nop
	nop
	add	r12,r7,r10
;
	halt
