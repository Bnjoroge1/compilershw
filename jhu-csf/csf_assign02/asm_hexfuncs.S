.section .rodata

hex_digits:
  .ascii "0123456789abcdef"

.section .text


# Format a byte value (in the range 0-255) as string consisting
# of two hex digits. The string is stored in sbuf.
#
# Parameters:
#   byteval - byte value to format
#   sbuf    - buffer to store the formatted string
hex_format_byte_as_hex:
  # Save the caller's registers
  pushq %rbp
  movq %rsp, %rbp
  pushq %rbx
  pushq %rsi
  pushq %rdi

  # Load the parameters into registers
  movb 8(%rbp), %sil   # byteval
  movq 16(%rbp), %rdi  # sbuf

  # Format the byte value as hexadecimal
  movl $2, %ecx
  movl $0, %ebx
  .L1:
    movzbl %sil, %edx
    andl $0xF, %edx
    movb hex_digits(,%rdx,1), %al
    movb %al, (%rdi,%rbx,1)
    
    shrl $4, %esi
    addl $1, %ebx
    dec %ecx
    jnz .L1

  # Null terminate the string
  movb $0, (%rdi,%rbx,1)

  # Restore the callee's registers
  popq %rdi
  popq %rsi
  popq %rbx
  leave
  ret

# Convert a byte value (in the range 0-255) to a printable character
# value. If byteval is already a printable character, it is returned
# unmodified. If byteval is not a printable character, then the
# ASCII code for '.' should be returned.
#
# Parameters:
#   byteval - byte value to convert
#
# Returns:
#   printable character value, or '.' if byteval is not printable
hex_to_printable:
  # Save the caller's registers
  pushq %rbp
  movq %rsp, %rbp

  # Load the parameter into a register
  movb 8(%rbp), %al  # byteval

  # Convert the byte value to a printable character
  cmpl $32, %eax
  jb .L2
  cmpl $126, %eax
  ja .L2
  movb %al, %dl
  jmp .L3
  .L2:
    movb $46, %dl  # ASCII code for '.'
  .L3:

  # Return the result
  leave
  ret

/* vim:ft=gas:
 */
