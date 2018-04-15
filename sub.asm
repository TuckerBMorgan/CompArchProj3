	.org	0x50
one:	.word	0xf0000000
two:	.word	0x0000ffff
	.org	0x100
	.entry	main
main:
;
	lw	r1,0x50(r0)
	lw	r2,0x54(r0)
	nop
	nop
	nop
;
	sub	r5,r1,r0
	sub	r6,r2,r1
	sub	r7,r2,r2
	sub	r8,r1,r1
;
	halt
