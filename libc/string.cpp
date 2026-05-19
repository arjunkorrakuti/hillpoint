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
