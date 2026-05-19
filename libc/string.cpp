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

extern "C" void* memset(void* destination, int value, size_t size) {
  auto* output = static_cast<unsigned char*>(destination);
  for (size_t index = 0; index < size; index++) {
    output[index] = static_cast<unsigned char>(value);
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
