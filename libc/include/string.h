#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* destination, const void* source, size_t size);

void* memset(void* destination, int value, size_t size);

#ifdef __cplusplus
}
#endif
