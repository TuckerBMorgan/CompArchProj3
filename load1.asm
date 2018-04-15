	.org	0x100
;
	.word	0x55555555
	.word	0x77777777
	.word	0xaaaaaaaa
	.word	0x99999999
;
	.org	0x500
	.entry	main
main:
	ori	r1,r1,0x0100
	ori	r2,r2,0x0110
	nop
	nop
	nop
;
	lw	r11,0(r1)
	lw	r12,4(r1)
	lw	r13,8(r1)
	lw	r14,12(r1)
;
	lw	r21,-4(r2)
	lw	r22,-8(r2)
	lw	r23,-12(r2)
	lw	r24,-16(r2)
;
	halt
