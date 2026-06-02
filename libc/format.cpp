#include <format.hpp>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

size_t format::write(Writer writer, void* context, const char* pattern,
                     va_list arguments) {
  size_t count = 0;
  auto emit = [&](char character) {
    writer(character, context);
    count++;
  };
  while (*pattern != '\0') {
    if (*pattern != '%') {
      emit(*pattern++);
      continue;
    }
    pattern++;
    if (*pattern == 's') {
      const char* string = va_arg(arguments, const char*);
      if (string == nullptr) {
        string = "(null)";
      }
      while (*string != '\0') {
        emit(*string++);
      }
      pattern++;
    } else if (*pattern == 'c') {
      emit(static_cast<char>(va_arg(arguments, int)));
      pattern++;
    } else if (*pattern == '%') {
      emit(*pattern++);
    } else {
      emit('%');
    }
  }
  return count;
}
