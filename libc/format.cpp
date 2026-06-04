#include <format.hpp>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

namespace {
  enum class Length { normal, longValue };
}

size_t format::write(Writer writer,
                     void* context,
                     const char* pattern,
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
    const char* start = pattern++;
    Length length = Length::normal;
    if (*pattern == 'l') {
      pattern++;
      length = Length::longValue;
    }
    const char specifier = *pattern;
    if (specifier != '\0') {
      pattern++;
    }
    if (length == Length::normal &&
        (specifier == 's' || specifier == 'c' || specifier == '%')) {
      char character = '%';
      const char* string = &character;
      size_t size = 1;
      if (specifier == 's') {
        string = va_arg(arguments, const char*);
        if (string == nullptr) {
          string = "(null)";
        }
        size = strlen(string);
      } else if (specifier == 'c') {
        character = static_cast<char>(va_arg(arguments, int));
      }
      for (size_t index = 0; index < size; index++) {
        emit(string[index]);
      }
      continue;
    }
    const bool pointer = specifier == 'p' && length == Length::normal;
    const bool signedValue = specifier == 'd' || specifier == 'i';
    if (!pointer && !signedValue && specifier != 'u' && specifier != 'x' &&
        specifier != 'X') {
      while (start != pattern) {
        emit(*start++);
      }
      continue;
    }
    uint64_t value = 0;
    bool negative = false;
    if (pointer) {
      value = reinterpret_cast<uintptr_t>(va_arg(arguments, void*));
    } else if (signedValue) {
      int64_t number = 0;
      switch (length) {
        case Length::normal:
          number = va_arg(arguments, int);
          break;
        case Length::longValue:
          number = va_arg(arguments, long);
          break;
      }
      negative = number < 0;
      value = negative ? uint64_t {0} - static_cast<uint64_t>(number)
                       : static_cast<uint64_t>(number);
    } else {
      switch (length) {
        case Length::normal:
          value = va_arg(arguments, unsigned int);
          break;
        case Length::longValue:
          value = va_arg(arguments, unsigned long);
          break;
      }
    }
    const uint64_t base = pointer || specifier == 'x' || specifier == 'X' ? 16 : 10;
    const char* digits = specifier == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
    char buffer[20];
    size_t size = 0;
    do {
      buffer[size++] = digits[value % base];
      value /= base;
    } while (value != 0);
    if (negative) {
      emit('-');
    } else if (pointer) {
      emit('0');
      emit('x');
    }
    while (size != 0) {
      emit(buffer[--size]);
    }
  }
  return count;
}
