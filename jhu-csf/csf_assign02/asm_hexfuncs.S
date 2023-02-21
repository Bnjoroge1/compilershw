.section .rodata
//0				;136;0c
hex_digits:
  .ascii "0123456789abcdef"

.section .text


	.globl hex_read
hex_read:	
	/ /* TODO: implement */
         ret


.globl hex_write_string
hex_write_string:
  // push the registers onto the stack to preserve their values
  push %rbp
  push %rbx
  push %rcx
  push %rdx
  push %rsi
  push %rdi
  
  // set up the arguments for the write syscall
  mov $1, %rax      // write syscall number
  mov $1, %rdi      // stdout file descriptor
  mov %rsi, %rdx    // buffer address
  mov %rdx, %rcx    // buffer length
  
  // call the write syscall
  syscall
  
  // print some log statements
  mov $0x0a, %rdi   // newline character
  mov $0x0a, %rsi   // newline character
  lea .L1(%rip), %rdx // buffer address
  mov $6, %rcx      // buffer length
  call hex_write_string
  mov $0x0a, %rdi   // newline character
  mov $0x0a, %rsi   // newline character
  lea .L2(%rip), %rdx // buffer address
  mov $6, %rcx      // buffer length
  call hex_write_string
  
  // restore the registers from the stack
  pop %rdi
  pop %rsi
  pop %rdx
  pop %rcx
  pop %rbx
  pop %rbp
  
  // return to the caller
  ret
  
  .L1:
  .string "BUFFER"
  
  .L2:
  .string "LENGTH"



	.globl hex_format_offset
hex_format_offset:
	// sets byteval to r8
	mov	%rdi, %r8
	// sets sbuf[] to r9
	mov	%rsi, %r9
	// sets integer for loop
	movl	$7, %ecx
	jmp	.LcheckConf
.Ltopf:
	// moves byteval to eax
	movzbl	%r8b, %eax
	// uses AND operation with 0xF
	andl	$0xF, %eax
	// sets sbuf[ecx] to hex_digits[byteval & 0xF]
	movzbl	hex_digits(,%rax,1), %eax
	mov	%al, (%r9,%rcx)
	// right shifts byteval by 4
	shr	$4, %r8
	// decrements ecx by 1
	dec	%ecx
.LcheckConf:
	// compares ecx with 0; begins loop is greater than or equal
	cmp	$0, %ecx
	jge	.Ltopf
	// sets sbuf[2] to null-break
	mov	$0, %al
	mov	%al, 8(%r9)
	ret

  .globl hex_format_byte_as_hex
/* This function takes a byte value and returns a string with
   the two digit hexadecimal representation of the byte value.
   For example, if the byte value is 0x32, the string "32" is
   returned. */
hex_format_byte_as_hex:

	// sets byteval to r8
	mov	%rdi, %r8
	// sets sbuf[] to r9
	mov	%rsi, %r9
	// sets integer for loop
	movl	$1, %ecx
	jmp	.LcheckCond
.Ltop:
	// moves byteval to eax
	movzbl	%r8b, %eax
	// uses AND operation with 0xF
	andl	$0xF, %eax
	// sets sbuf[ecx] to hex_digits[byteval & 0xF]
	movzbl	hex_digits(,%rax,1), %eax
	mov	%al, (%r9,%rcx)
	// right shifts byteval by 4
	shr	$4, %r8
	// decrements ecx by 1
	dec	%ecx
.LcheckCond:
	// compares ecx with 0; begins loop is greater than or equal
	cmp	$0, %ecx
	jge	.Ltop
	// sets sbuf[2] to null-break
	mov	$0, %al
	mov	%al, 2(%r9)
	ret

//2. set up the arguments for the write syscall
//3. call the write syscall
//4. restore the registers from the stack
//5. return to the caller




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
# Format a byte value (in the range 0-255) as string consisting
# of two hex digits. The string is stored in sbuf.
#
# Parameters:
#   byteval - byte value to format
#   sbuf    - buffer to store the formatted string


/* vim:ft=gas:
 */
