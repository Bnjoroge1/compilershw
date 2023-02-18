/* Assembly language implementation of hexdump functions */

	.section .rodata

	/* TODO: add read-only data such as strings here */

	.section .text


	.globl hex_read
hex_read:	
	/ /* TODO: implement */
         ret

	.globl hex_write_string
hex_write_string:
	 /* TODO: implement */
         ret

	.globl hex_format_offset
hex_format_offset:
	/* TODO: implement */
	ret

	.globl hex_format_byte_as_hex
hex_format_byte_as_hex:
         /* TODO: implement */
         ret

	.globl hex_to_printable
hex_to_printable:
	// Compare "parameter" to 32 ASCII
	cmp	$32, %rdi
	// Fails if less than 32
	jl	.LelsePart
	// Compare "parameter to 126 ASCII
	cmp	$126, %rdi
	// Fails in greater than 126
	jg	.LelsePart
	// Returns "parameter" if both statements are true
	mov	%rdi, %rax
	ret

.LelsePart:
	// Returns 46, the ASCII code for "."
	mov	$46, %rax
	ret
	

/* vim:ft=gas:
 */
