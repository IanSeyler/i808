// Print Hello World //
// Run for 120 cycles for best result //

dest=i808

:Start
	mov A, 0x30	// memory address of start of message //
	mov D, 0x0F	// port to write message to //
:Next
	mov B, [A]	// get a byte of the message //
	cmp B, 0x00	// check it against a null byte //
	je End		// if equal then you are at the end of the message, bail out //
	out D, B	// output the message byte to the port //
	inc A
	jmp Next	// do the next one //
:End
	jmp End
@0x30
:Message	DATA	"Hello World!"	ENDDATA
