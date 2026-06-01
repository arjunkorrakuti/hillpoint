#include <kernel/console.hpp>
#include <kernel/font.hpp>
#include <string.h>

namespace {
  size_t column = 0;
  size_t row = 0;
  size_t columns = 0;
  size_t rows = 0;
  constexpr uint32_t foreground = 0xffffff;
  constexpr uint32_t background = 0;

  void newline() {
    column = 0;
    row++;
    if (row == rows) {
      graphics::scroll(font::height, background);
      row--;
    }
  }

  void draw(char character) {
    for (size_t y = 0; y < font::height; y++) {
      for (size_t x = 0; x < font::width; x++) {
        const bool filled = font::pixel(static_cast<unsigned char>(character), x, y);
        graphics::pixel(column * font::width + x, row * font::height + y,
                        filled ? foreground : background);
      }
    }
  }
}

bool console::initialize(const Framebuffer& framebuffer) {
  columns = 0;
  rows = 0;
  column = 0;
  row = 0;
  if (!graphics::initialize(framebuffer)) {
    return false;
  }
  columns = graphics::width() / font::width;
  rows = graphics::height() / font::height;
  clear();
  return columns != 0 && rows != 0;
}

void console::putchar(char character) {
  if (column == columns && character != '\n') {
    newline();
  }
  if (character == '\n') {
    newline();
    return;
  }
  if (column < columns && row < rows) {
    draw(character);
    column++;
  }
}

void console::write(const char* string, size_t size) {
  for (size_t index = 0; index < size; index++) {
    putchar(string[index]);
  }
}

void console::write(const char* string) {
  write(string, strlen(string));
}

void console::clear() {
  graphics::clear(background);
  column = 0;
  row = 0;
}
