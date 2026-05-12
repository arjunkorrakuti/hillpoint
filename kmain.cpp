#include <kernel/boot.hpp>
#include <kernel/runtime.hpp>

extern "C" void kmain() {
  if (!boot::supported()) {
    halt();
  }
  const Framebuffer framebuffer = boot::framebuffer();
  if (!graphics::initialize(framebuffer)) {
    halt();
  }
  for (size_t y = 0; y < framebuffer.height; y++) {
    for (size_t x = 0; x < framebuffer.width; x++) {
      const uint32_t blue = x * 255 / framebuffer.width;
      const uint32_t green = y * 255 / framebuffer.height;
      graphics::pixel(x, y, (green << 8) | blue);
    }
  }
  halt();
}
