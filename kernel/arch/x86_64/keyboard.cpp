#include <kernel/interrupts.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>

namespace {
  constinit keyboard::Decoder decoder;
  constinit keyboard::Queue queue;

  bool writable() {
    for (size_t attempt = 0; attempt < 100000; attempt++) {
      if ((io::in(0x64) & 2) == 0) {
        return true;
      }
      io::wait();
    }
    return false;
  }

  bool command(uint8_t value) {
    if (!writable()) {
      return false;
    }
    io::out(0x64, value);
    return true;
  }

  bool data(uint8_t value) {
    if (!writable()) {
      return false;
    }
    io::out(0x60, value);
    return true;
  }

  bool receive(uint8_t& value) {
    for (size_t attempt = 0; attempt < 100000; attempt++) {
      const uint8_t status = io::in(0x64);
      if ((status & 1) != 0) {
        const uint8_t byte = io::in(0x60);
        if ((status & 0xe0) == 0) {
          value = byte;
          return true;
        }
      }
      io::wait();
    }
    return false;
  }

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
  const uint64_t flags = io::disableInterrupts();
  bool success = false;
  do {
    if (!command(0xad) || !command(0xa7)) {
      break;
    }
    for (size_t count = 0; count < 256 && (io::in(0x64) & 1) != 0; count++) {
      io::in(0x60);
    }
    uint8_t config;
    if (!command(0x20) || !receive(config)) {
      break;
    }
    config = static_cast<uint8_t>((config | 0x40) & ~0x13U);
    if (!command(0x60) || !data(config) || !command(0xab)) {
      break;
    }
    uint8_t response;
    if (!receive(response) || response != 0 || !command(0xae)) {
      break;
    }
    if (!command(0x60) || !data(static_cast<uint8_t>(config | 1))) {
      break;
    }
    success = interrupts::registerIrq(1, handle);
  } while (false);
  if (!success) {
    command(0xad);
  }
  io::restoreInterrupts(flags);
  return success;
}

bool keyboard::read(Key& key) {
  return queue.pop(key);
}

size_t keyboard::dropped() {
  return queue.dropped();
}
