	.org	0x50
	.word	loc1
	.word	loc2
	.word	loc3
	.word	loc4
	.word	loc5
	.word	loc6
	.word	loc7
;
	.org	0x100
	.entry	start
;
start:
	lw	r1,0x50(r0)
	lw	r2,0x54(r0)
	lw	r3,0x58(r0)
	lw	r4,0x5c(r0)
	lw	r5,0x60(r0)
	lw	r6,0x64(r0)
	lw	r7,0x68(r0)
	nop
	nop
	nop
;
	jr	r3
	nop
;
	.org	0x210
loc1:	jr	r5
	nop
	nop
	nop
loc2:	jr	r6
	nop
	nop
	nop
loc3:	jr	r1
	nop
	nop
	nop
loc4:	jr	r7
	nop
	nop
	nop
loc5:	jr	r2
	nop
	nop
	nop
loc6:	jr	r4
	nop
	nop
	nop
;
loc7:
	halt
