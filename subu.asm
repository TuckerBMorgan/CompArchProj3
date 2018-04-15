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
	subu	r5,r1,r0
	subu	r6,r2,r1
	subu	r7,r2,r2
	subu	r8,r1,r1
;
	halt
