	.org	0x100
	.entry	main
main:
	xori	r2,r0,#0x5
	xori	r3,r0,#0xa
	xori	r6,r0,#0x64
	xori	r4,r0,#0x2
	nop
	nop
	nop
	nop
	nop
	add	r6,r2,r3
	nop
	nop
	add	r5,r6,r4
;
	break
	halt
