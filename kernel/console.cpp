#include <kernel/console.hpp>
#include <kernel/font.hpp>

namespace {
  size_t column = 0;
  size_t row = 0;
  size_t columns = 0;
  size_t rows = 0;
  constexpr uint32_t foreground = 0xffffff;
  constexpr uint32_t background = 0;

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
  if (!graphics::initialize(framebuffer)) {
    return false;
  }
  columns = graphics::width() / font::width;
  rows = graphics::height() / font::height;
  graphics::clear(background);
  return columns != 0 && rows != 0;
}

void console::putchar(char character) {
  if (column < columns && row < rows) {
    draw(character);
    column++;
  }
}
