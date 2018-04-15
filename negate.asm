; negate a number, num
;
	.entry	start
;
	.org	0x100
;
num:	.word	0xfffffff0
tmp:	.word	0
res:	.word	0
;
	.org	0x200
;
start:	lw	r1,0x100(r0)	; tmp = num
	nop
	nop
	nop
	sw	r1,0x104(r0)
;
loop:
	lw	r2,0x108(r0)	; res += 1
	nop
	nop
	nop
	addi	r2,r2,1
	nop
	nop
	nop
	sw	r2,0x108(r0)
;
	lw	r3,0x100(r0)	; num += 1
	nop
	nop
	nop
	addi	r3,r3,1
	nop
	nop
	nop
	sw	r3,0x100(r0)
	nop
	nop
	nop
;
	bne	r3,r0,loop	; if num != 0, loop
	nop
	nop
	nop
;
	j	done
	nop
	nop
	nop
;
	.org	0xf00
;
done:	lw	r4,0x108(r0)	; c is now the negative of what a was; a is 0
	nop
	nop
	nop
	break
	halt
