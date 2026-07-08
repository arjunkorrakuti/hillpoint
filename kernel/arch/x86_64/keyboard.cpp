#include <kernel/interrupts.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>

namespace {
  constinit keyboard::Decoder decoder;
  constinit keyboard::Queue queue;

  void handle() {
    if ((io::in(0x64) & 1) == 0) {
      return;
    }
    const keyboard::Key key = decoder.decode(io::in(0x60));
    if (key != keyboard::none) {
      queue.push(key);
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
