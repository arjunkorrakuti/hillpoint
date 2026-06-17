#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/runtime.hpp>

extern "C" void interruptDispatch(const interrupts::Frame* frame) {
  console::printf("Exception %llu at %llx\n",
                  static_cast<unsigned long long>(frame->vector),
                  static_cast<unsigned long long>(frame->rip));
  panic("Unhandled CPU exception");
}
