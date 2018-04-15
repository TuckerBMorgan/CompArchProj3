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
	sltu	r10,r1,r3
	sltu	r11,r1,r4
	sltu	r12,r1,r5
	sltu	r13,r1,r6
	sltu	r14,r1,r7
	sltu	r15,r1,r8
;
	sltu	r20,r2,r3
	sltu	r21,r2,r4
	sltu	r22,r2,r5
	sltu	r23,r2,r6
	sltu	r24,r2,r7
	sltu	r25,r2,r8
;
	sltu	r10,r3,r1
	sltu	r11,r4,r1
	sltu	r12,r5,r1
	sltu	r13,r6,r1
	sltu	r14,r7,r1
	sltu	r15,r8,r1
;
	sltu	r20,r3,r2
	sltu	r21,r4,r2
	sltu	r22,r5,r2
	sltu	r23,r6,r2
	sltu	r24,r7,r2
	sltu	r25,r8,r2
;
	sltu	r10,r3,r6
	sltu	r11,r4,r6
	sltu	r12,r5,r6
	sltu	r13,r6,r6
	sltu	r14,r7,r6
	sltu	r15,r8,r6
;
	halt
