#ifndef INCLUDED_LZW_ENCODER
#define INCLUDED_LZW_ENCODER

#include <stddef.h>
#include <limits.h>
#include "typedefs.h"

/* Compresses the ‘len’-bytes data, each byte storing a source value, according
 * to the Variable-Width LZW encoding. Packs the resulting codes into the bytes
 * array given (if not NULL). Returns the number of bytes used to store the
 * result (thus can be used, without actual write, to compute the needed amount
 * of memory). */
size_t lzw_encode(size_t initial_code_size, size_t len, const Byte* data, Byte* out_bytes);

#endif
