#ifndef INCLUDED_TYPEDEFS
#define INCLUDED_TYPEDEFS

#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char Byte;
#define  BYTE_MAX  UCHAR_MAX
#define  BYTE_SZ   CHAR_BIT

typedef uint16_t Byte2;
#define  BYTE2_MAX  UINT16_MAX
#define  BYTE2_SZ   16

#if !defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
  #define LITTLE_ENDIAN
#elif defined(BIG_ENDIAN)
  #undef LITTLE_ENDIAN
#endif

#if !defined(BITFIELD_LSB) && !defined(BITFIELD_MSB)
  #define BITFIELD_LSB
#elif defined(BITFIELD_MSB)
  #undef BITFIELD_LSB
#endif

#endif
