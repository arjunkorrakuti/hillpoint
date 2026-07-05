#include <kernel/keyboard.hpp>

namespace {
  constexpr uint8_t normal[] = {
    0, 0, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 8, 0,
    113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 10, 0, 97, 115,
    100, 102, 103, 104, 106, 107, 108, 59, 39, 96, 0, 92, 122, 120, 99, 118,
    98, 110, 109, 44, 46, 47, 0, 42, 0, 32,
  };
}

keyboard::Key keyboard::Decoder::decode(uint8_t code) {
  // Consume unsupported multi-byte keys so their bytes cannot become text
  if (pauseBytes != 0) {
    pauseBytes--;
    return none;
  }
  if (code == 0xe1) {
    pauseBytes = 5;
    extended = false;
    return none;
  }
  if (code == 0xe0) {
    extended = true;
    return none;
  }
  if (extended) {
    extended = false;
    return none;
  }
  return code < sizeof(normal) ? normal[code] : none;
}
