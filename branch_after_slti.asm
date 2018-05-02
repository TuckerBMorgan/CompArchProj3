	.org	0x100
	.entry	main
main:
	xori    r2,r0,#0x5
	nop
	nop
	nop
	nop
	slti	r7,r2,#0x60
	bne	r7,r0,asdf
	nop
	nop
	nop
	nop
	break
	break
	break
	nop
	nop
	nop
	nop
asdf:
	halt
