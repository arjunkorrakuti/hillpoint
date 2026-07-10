#pragma once

#include <kernel/keyboard.hpp>

class LineEditor {
 public:
  enum class Action { none, append, erase, submit };
  Action feed(keyboard::Key key);
  void clear();
  const char* text() const { return line; }
  size_t size() const { return length; }

 private:
  char line[256] = {};
  size_t length = 0;
};
