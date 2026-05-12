#pragma once

#include <kernel/framebuffer.hpp>

namespace boot {
  bool supported();
  Framebuffer framebuffer();
}
