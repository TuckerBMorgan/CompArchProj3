;
; this test attempts to execute an instruction
; which has opcode 0x00 and function code 0x3f
;
	.org	0x100
	.entry	main
main:
	.word	0x000000ff
;
	halt
