#include <kernel/line_editor.hpp>

void LineEditor::clear() {
  line[0] = '\0';
  length = 0;
}

LineEditor::Action LineEditor::feed(keyboard::Key key) {
  if (key == '\n') {
    return Action::submit;
  }
  if (key == '\b') {
    if (length == 0) {
      return Action::none;
    }
    line[--length] = '\0';
    return Action::erase;
  }
  if (key >= 32 && key <= 126 && length + 1 < sizeof(line)) {
    line[length++] = static_cast<char>(key);
    line[length] = '\0';
    return Action::append;
  }
  return Action::none;
}
