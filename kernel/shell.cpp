#include <kernel/console.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>
#include <kernel/line_editor.hpp>
#include <kernel/shell.hpp>

namespace {
  constinit LineEditor editor;

  void prompt() {
    console::write("hillpoint> ");
  }

  void execute() {
    console::printf("%s\n", editor.text());
  }

  void accept(keyboard::Key key) {
    switch (editor.feed(key)) {
      case LineEditor::Action::none:
        break;
      case LineEditor::Action::append:
        console::putchar(editor.text()[editor.size() - 1]);
        break;
      case LineEditor::Action::erase:
        console::putchar('\b');
        break;
      case LineEditor::Action::submit:
        console::putchar('\n');
        execute();
        editor.clear();
        prompt();
        break;
    }
  }
}

[[noreturn]] void shell::run() {
  prompt();
  while (true) {
    keyboard::Key key;
    io::disableInterrupts();
    if (keyboard::read(key)) {
      asm volatile("sti" ::: "memory");
      accept(key);
    } else {
      asm volatile("sti; hlt" ::: "memory");
    }
  }
}
