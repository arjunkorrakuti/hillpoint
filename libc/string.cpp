#include <stdint.h>
#include <string.h>

extern "C" void* memcpy(void* destination, const void* source, size_t size) {
  auto* output = static_cast<unsigned char*>(destination);
  const auto* input = static_cast<const unsigned char*>(source);
  for (size_t index = 0; index < size; index++) {
    output[index] = input[index];
  }
  return destination;
}

extern "C" void* memmove(void* destination, const void* source, size_t size) {
  auto* output = static_cast<unsigned char*>(destination);
  const auto* input = static_cast<const unsigned char*>(source);
  if (reinterpret_cast<uintptr_t>(output) < reinterpret_cast<uintptr_t>(input)) {
    return memcpy(destination, source, size);
  }
  while (size != 0) {
    size--;
    output[size] = input[size];
  }
  return destination;
}

extern "C" void* memset(void* destination, int value, size_t size) {
  auto* output = static_cast<unsigned char*>(destination);
  for (size_t index = 0; index < size; index++) {
    output[index] = static_cast<unsigned char>(value);
  }
  return destination;
}

extern "C" int memcmp(const void* left, const void* right, size_t size) {
  const auto* first = static_cast<const unsigned char*>(left);
  const auto* second = static_cast<const unsigned char*>(right);
  for (size_t index = 0; index < size; index++) {
    if (first[index] != second[index]) {
      return first[index] - second[index];
    }
  }
  return 0;
}

extern "C" size_t strlen(const char* string) {
  size_t size = 0;
  while (string[size] != '\0') {
    size++;
  }
  return size;
}

extern "C" int strcmp(const char* left, const char* right) {
  while (*left != '\0' && *left == *right) {
    left++;
    right++;
  }
  return static_cast<unsigned char>(*left) - static_cast<unsigned char>(*right);
}
