	.entry	start
;
start:	j	loc3
	nop
	nop
	nop
	.org	0x100
loc1:	j	loc5
	nop
	nop
	nop
	.org	0x200
loc2:	j	loc6
	nop
	nop
	nop
	.org	0x300
loc3:	j	loc1
	nop
	nop
	nop
	.org	0x400
loc4:	j	loc7
	nop
	nop
	nop
	.org	0x500
loc5:	j	loc2
	nop
	nop
	nop
	.org	0x600
loc6:	j	loc4
	nop
	nop
	nop
	.org	0x700
loc7:
	halt
