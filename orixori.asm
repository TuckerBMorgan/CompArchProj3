	.org	0x50
aaa:	.word	0xaaaaaaaa
fff:	.word	0x55555555
nnn:	.word	0x99999999
	.org	0x100
	.entry	main
main:
	lw	r1,0x50(r0)
	lw	r2,0x54(r0)
	nop
	nop
	nop
	break
;
	ori	r7,r1,#0x5555
	ori	r8,r1,#0x9999
	ori	r9,r2,#0x9999
;
	xori	r10,r1,#0x5555
	xori	r11,r1,#0x9999
	xori	r12,r2,#0x9999
;
	halt
