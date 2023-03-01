// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!
//expected output format:
//00000000: 54 68 69 73 20 69 73 20 61 20 6c 6f 6e 67 65 72  This is a longer
//00000010: 20 65 78 61 6d 70 6c 65 20 6f 66 20 61 20 68 65   example of a he
//00000020: 78 64 75 6d 70 2e 20 4d 61 72 76 65 6c 20 61 74  xdump. Marvel at
//00000030: 20 69 74 73 20 6d 61 67 6e 69 66 69 63 65 6e 63   its magnificenc
//00000040: 65 2e 0a                                         e..


int main(void) {
    char data_buf[16];
    char sbuf[16];
    unsigned offset = 0;
    int n;
    while ((n = hex_read(data_buf)) > 0) {
        hex_format_offset(offset, sbuf);
        hex_write_string(sbuf);
        hex_write_string(": ");
        for (int i = 0; i < n; i++) {
            hex_format_byte_as_hex(data_buf[i], sbuf);
            //remove any initial 0s
            hex_write_string(sbuf);
            hex_write_string(" ");
        }
        for (int i = n; i < 16; i++) {
            hex_write_string("   ");
        }
        hex_write_string("  ");
        for (int i = 0; i < n; i++) {
            char printable = hex_to_printable(data_buf[i]);
            hex_format_byte_as_hex(printable, sbuf);
            //hex_write_string(" ");
            

            //hex_write_string(printable == '.' ? "." : &printable);
            //hex_write_string(sbuf);
        }
        char temp_buf[17];
for (int i = 0; i < n; i++) {
    hex_to_printable(data_buf[i]);
    temp_buf[i] = hex_to_printable(data_buf[i]);
}
temp_buf[n] = '\0'; // add null terminator
hex_write_string(temp_buf);
for (int i = n; i < 16; i++) {
    hex_write_string(" ");
}
//hex_write_string("\n");

        hex_write_string(" ");
        hex_write_string("\n");
        offset += 16;
    }

    return 0;
}