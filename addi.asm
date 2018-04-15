	.org	0x50
x:	.word	0xffff
;
	.org	0x100
	.entry	main
main:
	lui	r1,#0x7fff
	nop
	nop
	nop
;
	addi	r5,r1,#0x0
	addi	r6,r1,#0x7fff
	addi	r7,r1,#0xffff
;
	halt
