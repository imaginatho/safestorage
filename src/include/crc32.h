#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>

uint32_t crc32 ( const void* data, size_t bytes, uint32_t initcrc = 0 );

#endif
