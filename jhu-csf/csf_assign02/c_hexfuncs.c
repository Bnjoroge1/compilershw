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

     int n = write(1, s, len);
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
  if ((int) byteval >= 32 && (int) byteval <= 126) {
    return byteval;
  } else {
    char period = '.';
    return (int) period;
  }
  
}
