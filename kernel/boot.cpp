#include <kernel/boot.hpp>
#include <limine.h>

namespace {
  __attribute__((used, section(".limine_requests_start")))
  volatile uint64_t requestsStart[] = LIMINE_REQUESTS_START_MARKER;

  __attribute__((used, section(".limine_requests")))
  volatile uint64_t baseRevision[] = LIMINE_BASE_REVISION(6);

  __attribute__((used, section(".limine_requests")))
  volatile limine_framebuffer_request framebufferRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0, .response = nullptr};

  __attribute__((used, section(".limine_requests_end")))
  volatile uint64_t requestsEnd[] = LIMINE_REQUESTS_END_MARKER;
}

bool boot::supported() {
  return LIMINE_BASE_REVISION_SUPPORTED(baseRevision);
}

Framebuffer boot::framebuffer() {
  const auto* response = framebufferRequest.response;
  if (response == nullptr || response->framebuffer_count == 0 ||
      response->framebuffers == nullptr || response->framebuffers[0] == nullptr) {
    return {};
  }
  const auto* buffer = response->framebuffers[0];
  return {buffer->address, buffer->width, buffer->height, buffer->pitch, buffer->bpp,
          buffer->red_mask_size, buffer->red_mask_shift,
          buffer->green_mask_size, buffer->green_mask_shift,
          buffer->blue_mask_size, buffer->blue_mask_shift};
}
