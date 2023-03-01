// C implementation of hexdump functions
#include <unistd.h> // this is the only system header file you may include!
#include "hexfuncs.h"

// Read up to 16 bytes from standard input into data_buf using read().
// Returns the number of characters read.
unsigned hex_read(char data_buf[])
{
  // 1. read from stdin
  int n = read(0, data_buf, 16);
  
  // 2. return the number of characters read
  return n;
}
// Write given nul-terminated string to standard output.
void hex_write_string(const char s[])
{
    // get the length of the string
     int len = 0;
     while (s[len] != '\0')
     {
          len++;
     }

     // write the string to standard output
     write(1, s, len);

     // add the nul-terminator
     write(1, "\0", 1);
}
//// Format an unsigned value as an offset string consisting of exactly 8
// hex digits.  The formatted offset is stored in sbuf, which must
// have enough room for a string of length 8.
//
void hex_format_offset(unsigned offset, char sbuf[])
{
     static const char hex_digits[] = "0123456789abcdef";
     
    
    for (int i = 7; i >= 0; i--) {
        sbuf[i] = hex_digits[offset & 0xF];
        offset >>= 4;
    }
    sbuf[8] = '\0';
}



// Format a byte value (in the range 0-255) as string consisting
// of two hex digits. The string is stored in sbuf.
void hex_format_byte_as_hex(unsigned char byteval, char sbuf[]) {
  
  static const char hex_digits[] = "0123456789abcdef";

  for (int i = 1; i >= 0; i--) {
    sbuf[i] = hex_digits[byteval & 0xF];
    byteval >>= 4;
  }
  sbuf[2] = '\0';
}


//Convert a byte value (in the range 0-255) to a printable character
// value. If byteval is already a printable character, it is returned
// unmodified. If byteval is not a printable character, then the
// ASCII code for '.' should be returned.
char hex_to_printable(unsigned char byteval) {
  // 1. check if byteval is a printable character
  if (byteval >= 32 && byteval <= 126) {
    // 2. if it is, return it
    return byteval;
  } else {
    // 3. if it is not, return '.'
    return '.';
  }

  
  
}
