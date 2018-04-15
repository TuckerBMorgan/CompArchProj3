	.org	0x100
counter:
	.word	3
;
; recursive subroutine
;
subr:
	nop
	nop
	nop
	sw	r31,-4(r14)	;; save the return address
	addi	r14,r14,#-4	;; adjust stack pointer
	lw	r2,0x100(r0)	;; load counter
	nop
	nop
	nop
	addi	r2,r2,#-1	;; decrement counter
	nop
	nop
	nop
	sw	r2,0x100(r0)
	beq	r2,r0,nocall	;; if count == 0, don't recurse
	nop
	slti	r7,r2,0		;; if count < 0, don't recurse
	bne	r7,r0,nocall
	nop
	nop
	jal	subr		;; recurse!
	nop
nocall:
	lw	r31,0(r14)	;; restore return address
	addi	r14,r14,#4	;; restore stack pointer
	nop
	nop
	nop
	jr	r31		;; return
	nop
;
	.org	0x200
	.entry	main
main:
	addi	r14,r0,#0x1ffc	;; initialize stack pointer
	nop
	nop
	nop
;
	jal	subr		;; call the subroutine
	nop
;
	halt
