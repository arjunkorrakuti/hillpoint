#pragma once

#include <stdarg.h>
#include <stddef.h>

namespace format {
  using Writer = void (*)(char character, void* context);
  size_t write(Writer writer, void* context, const char* pattern, va_list arguments);
} // namespace format
