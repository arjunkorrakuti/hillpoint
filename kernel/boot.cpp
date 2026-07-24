#include <kernel/boot.hpp>
#include <kernel/console.hpp>
#include <limine.h>

namespace {
  __attribute__((used, section(".limine_requests_start")))
  volatile uint64_t requestsStart[] = LIMINE_REQUESTS_START_MARKER;

  __attribute__((used, section(".limine_requests")))
  volatile uint64_t baseRevision[] = LIMINE_BASE_REVISION(6);

  __attribute__((used, section(".limine_requests")))
  volatile limine_framebuffer_request framebufferRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0, .response = nullptr};

  __attribute__((used, section(".limine_requests")))
  volatile limine_bootloader_info_request infoRequest = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST_ID, .revision = 0, .response = nullptr};

  __attribute__((used, section(".limine_requests")))
  volatile limine_memmap_request memoryRequest = {
    .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0, .response = nullptr};

  __attribute__((used, section(".limine_requests_end")))
  volatile uint64_t requestsEnd[] = LIMINE_REQUESTS_END_MARKER;
}

bool boot::supported() {
  return LIMINE_BASE_REVISION_SUPPORTED(baseRevision);
}

Framebuffer boot::framebuffer() {
  const auto* response = framebufferRequest.response;
  if (response == nullptr || response->framebuffer_count == 0 ||
      response->framebuffers == nullptr) {
    return {};
  }
  for (uint64_t index = 0; index < response->framebuffer_count; index++) {
    const auto* buffer = response->framebuffers[index];
    if (buffer == nullptr || buffer->memory_model != LIMINE_FRAMEBUFFER_RGB) {
      continue;
    }
    Framebuffer candidate = {buffer->address, buffer->width, buffer->height,
                             buffer->pitch, buffer->bpp,
                             buffer->red_mask_size, buffer->red_mask_shift,
                             buffer->green_mask_size, buffer->green_mask_shift,
                             buffer->blue_mask_size, buffer->blue_mask_shift};
    if (graphics::supported(candidate)) {
      return candidate;
    }
  }
  return {};
}

void boot::printSummary() {
  const auto* info = infoRequest.response;
  if (info != nullptr) {
    console::printf("Bootloader: %s %s\n", info->name, info->version);
  }
  if (graphics::width() != 0) {
    console::printf("Framebuffer: %zu x %zu, 32-bit RGB\n",
                    graphics::width(), graphics::height());
  } else {
    console::printf("Framebuffer unavailable; using serial output.\n");
  }
  const auto* memory = memoryRequest.response;
  if (memory == nullptr || memory->entries == nullptr) {
    console::printf("Memory map unavailable.\n");
    return;
  }
  uint64_t usable = 0;
  for (uint64_t index = 0; index < memory->entry_count; index++) {
    const auto* entry = memory->entries[index];
    if (entry != nullptr && entry->type == LIMINE_MEMMAP_USABLE) {
      usable += entry->length;
    }
  }
  console::printf("Usable memory: %llu MiB (%llu regions in boot memory map)\n",
                  static_cast<unsigned long long>(usable / (1024 * 1024)),
                  static_cast<unsigned long long>(memory->entry_count));
}

size_t boot::memoryRegions() {
  const auto* response = memoryRequest.response;
  return response != nullptr && response->entries != nullptr ? response->entry_count : 0;
}

boot::MemoryRegion boot::memoryRegion(size_t index) {
  if (index >= memoryRegions()) {
    return {};
  }
  const auto* entry = memoryRequest.response->entries[index];
  return entry != nullptr ? MemoryRegion{entry->base, entry->length,
                                         entry->type == LIMINE_MEMMAP_USABLE}
                          : MemoryRegion{};
}
