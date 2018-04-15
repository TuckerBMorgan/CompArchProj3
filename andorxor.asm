	.org	0x50
aaa:	.word	0xaaaaaaaa
fff:	.word	0x55555555
nnn:	.word	0x99999999
	.org	0x100
	.entry	main
main:
	lw	r1,0x50(r0)
	lw	r2,0x54(r0)
	lw	r3,0x58(r0)
	nop
	nop
	nop
	nop
	break
;
	and	r4,r1,r2
	and	r5,r1,r3
	and	r6,r2,r3
;
	or	r7,r1,r2
	or	r8,r1,r3
	or	r9,r2,r3
;
	xor	r10,r1,r2
	xor	r11,r1,r3
	xor	r12,r2,r3
;
	halt
