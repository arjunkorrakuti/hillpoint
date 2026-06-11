#include <kernel/io.hpp>
#include <kernel/serial.hpp>
#include <stddef.h>
#include <stdint.h>

namespace {
  constexpr uint16_t port = 0x3f8;
  bool ready = false;

  using io::in;
  using io::out;
}

void serial::initialize() {
  out(port + 1, 0x00);
  out(port + 3, 0x80);
  out(port, 0x01);
  out(port + 1, 0x00);
  out(port + 3, 0x03);
  out(port + 2, 0xc7);
  out(port + 4, 0x1e);
  out(port, 0xae);
  ready = in(port) == 0xae;
  out(port + 4, 0x0f);
}

void serial::putchar(char character) {
  if (!ready) {
    return;
  }
  // A missing or stalled UART must not stop the kernel from booting
  for (size_t attempt = 0; attempt < 100000; attempt++) {
    if ((in(port + 5) & 0x20) != 0) {
      out(port, static_cast<uint8_t>(character));
      return;
    }
    asm volatile("pause");
  }
}
