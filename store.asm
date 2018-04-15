	.org	0x50
one:	.word	0xffffffff
two:	.word	0x87659abc
	.org	0x100
	.entry	main
main:
;
	lw	r1,0x50(r0)
	nop
	nop
	nop
	sw	r1,0x40(r0)
	nop
	nop
	nop
	lw	r2,0x40(r0)
	nop
	nop
	nop
;
	lw	r3,0x54(r0)
	nop
	nop
	nop
	sw	r3,0x30(r0)
	nop
	nop
	nop
	lw	r4,0x30(r0)
;
	halt
