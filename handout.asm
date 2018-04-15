	.org	0x100
	.entry	main
main:
	lui	r1,#0x7fff
	lui	r2,#0x7fff
	nop
	nop
	nop
;
	ori	r3,r1,#0xffff
	ori	r4,r2,#0xffff
	nop
	nop
	nop
;
	add	r5,r3,r15
	addi	r6,r4,#0xffff
	nop
	nop
	nop
;
	break
	halt
