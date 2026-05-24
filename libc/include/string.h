#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* destination, const void* source, size_t size);

void* memset(void* destination, int value, size_t size);

void* memmove(void* destination, const void* source, size_t size);

int memcmp(const void* left, const void* right, size_t size);

size_t strlen(const char* string);

int strcmp(const char* left, const char* right);

#ifdef __cplusplus
}
#endif
