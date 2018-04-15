;
; this test attempts to execute an instruction
; which has opcode 0x0f
;
	.org	0x100
	.entry	main
main:
	.word	0x3fffffff
;
	halt
