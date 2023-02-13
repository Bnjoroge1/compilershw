// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!

int main(void) {
  //main function
  char data_buf[16];
  char sbuf[8];
  int n = 0;
  int offset = 0;
  int i = 0;
  int j = 0;
  //1. read from stdin
  n = hex_read(data_buf);
  //2. print the offset
  hex_format_offset(offset, sbuf);
  hex_write_string(sbuf);
  hex_write_string(": ");
  //3. print the hex values
  for(i = 0; i < n; i++) {
    hex_format_offset(data_buf[i], sbuf);
    hex_write_string(sbuf);
    hex_write_string(" ");
    if(i == 7) {
      hex_write_string(" ");
    }
  }
  //4. print the spaces
  for(j = 0; j < 16 - n; j++) {
    hex_write_string("   ");
    if(j == 7) {
      hex_write_string(" ");
    }
  }
  //5. print the ascii values
  hex_write_string("|");
  for(i = 0; i < n; i++) {
    if(data_buf[i] >= 32 && data_buf[i] <= 126) {
      hex_write_string(&data_buf[i]);
    } else {
      hex_write_string(".");
    }
  }
  hex_write_string("|");
  hex_write_string("");


}
