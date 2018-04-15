	.org	0x50
one:	.word	0x8000ffff
two:	.word	0x0000ffff
three:	.word	0x00000004
four:	.word	0x00000005
five:	.word	0x00000006
six:	.word	0x00007ff4
seven:	.word	0x00007ff5
eight:	.word	0x00007ff6
;
	.org	0x100
	.entry	main
main:
	lw	r1,0x50(r0)
	lw	r2,0x54(r0)
	lw	r3,0x58(r0)
	lw	r4,0x5c(r0)
	lw	r5,0x60(r0)
	lw	r6,0x64(r0)
	lw	r7,0x68(r0)
	lw	r8,0x6c(r0)
;
	slti	r10,r1,#4
	slti	r11,r1,#5
	slti	r12,r1,#6
	slti	r13,r1,#0x7ff4
	slti	r14,r1,#0x7ff5
	slti	r15,r1,#0x7ff6
;
	slti	r20,r2,#4
	slti	r21,r2,#5
	slti	r22,r2,#6
	slti	r23,r2,#0x7ff4
	slti	r24,r2,#0x7ff5
	slti	r25,r2,#0x7ff6
;
	slti	r10,r3,#0xffff
	slti	r11,r4,#0xffff
	slti	r12,r5,#0xffff
	slti	r13,r6,#0xffff
	slti	r14,r7,#0xffff
	slti	r15,r8,#0xffff
;
	slti	r20,r3,#0x7fff
	slti	r21,r4,#0x7fff
	slti	r22,r5,#0x7fff
	slti	r23,r6,#0x7fff
	slti	r24,r7,#0x7fff
	slti	r25,r8,#0x7fff
;
	slti	r10,r3,#0x7ff4
	slti	r11,r4,#0x7ff4
	slti	r12,r5,#0x7ff4
	slti	r13,r6,#0x7ff4
	slti	r14,r7,#0x7ff4
	slti	r15,r8,#0x7ff4
;
	halt
