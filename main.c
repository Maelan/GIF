#include <stdio.h>
#include "lzw_encoder.h"

int main(void)
{
	Byte in_bytes[256];
	Byte out_bytes[256];
	int c;
	size_t len, compressed_len;
	
	for(len = 0;  (c = getchar()) != EOF;  len++)
		in_bytes[len] = c, fprintf(stderr, "%02X ", in_bytes[len]);
	
	compressed_len = lzw_encode(BYTE_SZ, len, in_bytes, out_bytes);
	fprintf(stderr, "\nlen %u / compressed %u\n", len, compressed_len);
	
	for(size_t i = 0;  i < compressed_len;  i++)
		putchar(out_bytes[i]), fprintf(stderr, "%02X ", out_bytes[i]);
	putc('\n', stderr);
	
	return 0;
}
