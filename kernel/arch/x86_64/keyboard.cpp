#include <kernel/interrupts.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>

namespace {
  constinit keyboard::Decoder decoder;
  constinit keyboard::Queue queue;

  void handle() {
    for (size_t count = 0; count < 32; count++) {
      const uint8_t status = io::in(0x64);
      if ((status & 1) == 0) {
        break;
      }
      const uint8_t code = io::in(0x60);
      if ((status & 0xe0) != 0) {
        continue;
      }
      const keyboard::Key key = decoder.decode(code);
      if (key != keyboard::none) {
        queue.push(key);
      }
    }
  }
}

bool keyboard::initialize() {
  return interrupts::registerIrq(1, handle);
}

bool keyboard::read(Key& key) {
  return queue.pop(key);
}

size_t keyboard::dropped() {
  return queue.dropped();
}
