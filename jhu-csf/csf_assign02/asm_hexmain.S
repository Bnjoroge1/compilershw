


	
	/* Hexdump main function, assembly language version */

.section .data
	data_buf: .skip 16
	sbuf: .skip 16
	offset: .long 0
	n: .long 0
	//save colon and space
	sColonSpace: .string ": "
	//save space
	sSpace: .string " "
	//save double space
	sDoubleSpace: .string "  "
	//temp_buf
	sTempBuff: .skip 16
	//sNullTerminator
	sNullTerminator: .string "\0"
	//sNewline
	sNewline: .string "\n"
	
.extern hex_read 
.extern hex_format_offset
.extern hex_write_string
.extern hex_format_byte_as_hex
.extern hex_to_printable

.section .text
	.globl main
main:
    //push the base pointer
    pushq %rbp
    //move the stack pointer to the base pointer
    movq %rsp, %rbp
    //allocate space for 16 bytes
    subq $16, %rsp
    //set the offset to 0
    movq $0, offset
    //initialize n to 0
    movq $0, n
    //call hex read
    leaq data_buf, %rdi
    call hex_read
    //move the output of hex_read to the n register
    movl %eax, n
    //check if n is greater than 0
    cmp $0, %eax
    //jump to the end of the loop if n is less than or equal to 0
    jle .LdoneWithRead
    //set up arguments for hex_format_offset
    leaq sbuf, %rdi
    movq offset, %rsi
    //call hex_format_offset
    call hex_format_offset
    //set up arguments for hex_write_string
    leaq sbuf, %rdi
    //call hex_write_string
    call hex_write_string
    //set up arguments for hex_write_string
    movq $sColonSpace, %rdi
    //call hex_write_string
    call hex_write_string
    //set the loop counter to 0
    movl $0, %ecx
.LwriteDataLoop:
    //compare the loop counter to the number of bytes read
    movl n, %eax
    //jump to the end of the loop if the loop counter is greater than or equal to the number of bytes read
    cmp %eax, %ecx
    //jump to the end of the loop if the loop counter is greater than or equal to the number of bytes read
    jge .LfillRemaining
    //move data buffer to the rdi register
    leaq data_buf, %rdi
    //move the loop counter to the rsi register
    movl %ecx, %esi
    //get sbuf to the rdx register
    leaq sbuf, %rdx
    //call hex_format_byte_as_hex
    call hex_format_byte_as_hex
    //set up arguments for hex_write_string
    leaq sbuf, %rdi
    //call hex_write_string
    call hex_write_string
    //set up arguments for hex_write_string
    movq $sSpace, %rdi
    //call hex_write_string
    call hex_write_string
    //increment the loop counter
    inc %ecx
    //jump to the beginning of the loop
    jmp .LwriteDataLoop

.LfillRemaining:
//initialize the loop counter to 0
    movl $0, %ecx
    //compare the loop counter to 16
    cmp $16, %ecx
    //jump to the end of the loop if the loop counter is greater than or equal to 16
    jge .LwritePrintables
    //move double space to the string buffer
    movq $sDoubleSpace, %rdi
    //call hex_write_string
    call hex_write_string
    //increment the loop counter
    inc %ecx
    //jump to the beginning of the loop
    jmp .LfillRemaining
.LwritePrintables:
//write spaces 
    movq $sSpace, %rdi
    //call hex_write_string
    call hex_write_string

    //initialize the loop counter to 0
    movl $0, %ecx
    //compare the loop counter to the number of bytes read
    cmp n, %ecx
    //jump to the end of the loop if the loop counter is greater than or equal to the number of bytes read
    jge .LtempBuffer
    //move data buffer to the rdi register
    leaq data_buf, %rdi
    //move the loop counter to the rsi register
    movl %ecx, %esi
    //get sbuf to the rdx register
    leaq sbuf, %rdx
    //call hex_to_printable
    call hex_to_printable
    //set up arguments for format byte as hex
    //move output of hex_to_printable to the rdi register
    movb %al, %dil
    //move sbuf to the rsi register
    leaq sbuf, %rsi
    //call hex_format_byte_as_hex
    call hex_format_byte_as_hex
    //back to the main loop
    jmp .LwritePrintables
.LtempBuffer:
    //set 17 to the stack 
    movq $17, %rdi
    //move sTempBuff to the rsi register
    leaq sTempBuff, %rsi
    //back to the main loop
    jmp .LtempBuffer
    
    //initialize the loop counter to 0
    movl $0, %ecx
.LwritePrintableLoop:
    push %rbp          //save the base pointer
    movq %rsp, %rbp     //set the base pointer to the current stack pointer
    mov (%rdi), %al    //move the input byte into the low 8 bits of %rax
          //move the input byte into the low 8 bits of %rax
    shr $4, %al        //shift the high 4 bits of the byte into the low 4 bits
    add $0x30, %al     //convert the high 4 bits to the corresponding printable character
    cmp $0x3a, %al     //if the result is in the range 'A'-'Z', add 7 to get 'a'-'z'
    jl  .L1
    add $7, %al

.L1:
    mov %al, (%rsi)    //store the result in the output buffer
    pop %rbp           //restore the base pointer
    ret                //return to the caller
    

    //initialize the loop counter to 0
    movl $0, %ecx
.LwriteSpacesLoop:
    //move 16 to the eax register
    movl $16, %eax
    //compare the loop counter to 16
    cmp %eax, %ecx
    //jump to the end of the loop if the loop counter is greater than or equal to 16
    jge .LwritePrintableLoop
    //set up arguments for hex_write_string
    //move double space to the rdi register
    movq $sDoubleSpace, %rdi
    //call hex_write_string
    call hex_write_string
    //increment the loop counter
    inc %ecx
    //jump to the beginning of the loop
    jmp .LwriteSpacesLoop

    //move space to the rdi register
    movq $sSpace, %rdi
    //call hex_write_string
    call hex_write_string
    //move newline to the rdi register
    movq $sNewline, %rdi
    //call hex_write_string
    call hex_write_string
    //move offset to the rdi register
    movq offset, %rdi
    //increase the offset by 16
    addq $16, offset
    //jump to the rest of the loop
    jmp .LwriteDataLoop

.LdoneWithRead:
    //return 0
    movl $0, %eax
    //restore the stack pointer
    movq %rbp, %rsp
    //restore the base pointer
    popq %rbp
    //return
    ret


















  


	

/* vim:ft=gas:
 */
