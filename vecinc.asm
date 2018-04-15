	.entry	start
;
	.org	0x100
vec:	.word	0x12334566
	.word	0x23445677
	.word	0x34556788
	.word	0x45667899
	.word	0x56778900
	.word	0x67889011
	.word	0x78990122
	.word	0x01223455
;
	.org	0x300
num:	.word	0xfffffff8
num2:	.word	0xfffffff8
;
	.org	0x30e
;
start:	addi	r1,r0,0
	ori	r2,r0,0x100
	nop
	nop
	nop
;
loop:	slti	r3,r1,8
	nop
	nop
	nop
	beq	r3,r0,quit
	nop
;
	lw	r4,0(r2)
	nop
	nop
	nop
	addi	r4,r4,1
	nop
	nop
	nop
	sw	r4,0(r2)
	addi	r2,r2,4
	addi	r1,r1,1
	nop
	nop
	j	loop
	nop
;
quit:
	lw	r1,0x100(r0)
	lw	r2,0x104(r0)
	lw	r3,0x108(r0)
	lw	r4,0x10c(r0)
	lw	r5,0x110(r0)
	lw	r6,0x114(r0)
	lw	r7,0x118(r0)
	lw	r8,0x11c(r0)
;
	break
	halt
