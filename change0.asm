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
	ori	r1,r1,0x100
	nop
	nop
	nop
	lw	r10,0(r1)
	lw	r11,4(r1)
	lw	r12,8(r1)
	lw	r13,12(r1)
	break
;
	lw	r0,0(r1)
	nop
	nop
	nop
	nop
;
	or	r0,r1,r2
	nop
	nop
	nop
	nop
;
	add	r0,r3,r4
	nop
	nop
	nop
	nop
;
	break
	halt
