	.org	0x50
one:	.word	0xa5a5a5a5
	.org	0x100
	.entry	main
main:
	addi	r1,r0,#0x0010
	addi	r2,r0,#0x0004
	addi	r3,r0,#0xffff
	nop
	nop
	nop
;
	lw	r4,0x50(r0)
	nop
	nop
	nop
;
	sll	r10,r4,#0x10
	srl	r11,r4,#0x10
	sra	r12,r4,#0x10
;
	sll	r13,r4,#0x4
	srl	r14,r4,#0x4
	sra	r15,r4,#0x4
;
	sll	r16,r4,#0x1f
	srl	r17,r4,#0x1f
	sra	r18,r4,#0x1f
;
	halt
