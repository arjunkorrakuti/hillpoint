#include <limine.h>
#include <stddef.h>
#include <stdint.h>

extern "C" void kmain() {
  const uint8_t limineRevision = 6;

  // Create a framebuffer request and place it in the requests section
  // of the final executable
  __attribute__((
    used,
    section(".limine_requests"))) static const volatile struct limine_framebuffer_request
    framebufferRequest = {.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
                          .revision = limineRevision,
                          .response = nullptr};

  // Take our first framebuffer and get the address of memory
  struct limine_framebuffer const* framebuffer =
    framebufferRequest.response->framebuffers[0];
  volatile uint32_t* fbPtr = static_cast<uint32_t*>(framebuffer->address);

  // Write to every pixel in the framebuffer with an increasing green
  // and blue value creating a gradient
  const uint32_t shift = 8;
  for (size_t y = 0; y < framebuffer->height; y++) {
    for (size_t x = 0; x < framebuffer->width; x++) {
      const uint32_t nX = x * 255 / framebuffer->width;
      const uint32_t nY = y * 255 / framebuffer->height;
      fbPtr[(y * (framebuffer->pitch / 4)) + x] = (nY << shift) | nX;
    }
  }

  while (true) {
    asm("hlt");
  }
}
