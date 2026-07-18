#include <kernel/console.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>
#include <kernel/line_editor.hpp>
#include <kernel/shell.hpp>
#include <string.h>

namespace {
  constinit LineEditor editor;

  void prompt() {
    console::write("hillpoint> ");
  }

  char* token(char*& input) {
    while (*input == ' ') {
      input++;
    }
    char* start = input;
    while (*input != '\0' && *input != ' ') {
      input++;
    }
    if (*input != '\0') {
      *input++ = '\0';
    }
    while (*input == ' ') {
      input++;
    }
    return start;
  }

  void echo(char* arguments) {
    console::printf("%s\n", arguments);
  }

  void clear(char*) {
    console::clear();
  }

  void about(char*) {
    console::printf(
      "Hillpoint: x86-64, C++23, Limine.\n"
      "Single-core kernel shell; US PS/2 keyboard input.\n"
      "Type a command, use Backspace to edit, and Enter to run it.\n"
      "Type help for commands.\n");
  }

  void help(char*);

  struct Command {
    const char* name;
    const char* description;
    void (*run)(char* arguments);
  };

  const Command commands[] = {
    {"about", "Kernel features and editing keys", about},
    {"clear", "Clear the screen", clear},
    {"echo", "Print text", echo},
    {"help", "List commands", help}};

  void help(char*) {
    for (const Command& command : commands) {
      console::printf("%s  %s\n", command.name, command.description);
    }
  }

  void execute() {
    char buffer[256];
    memcpy(buffer, editor.text(), editor.size() + 1);
    char* arguments = buffer;
    const char* name = token(arguments);
    if (*name == '\0') {
      return;
    }
    for (const Command& command : commands) {
      if (strcmp(name, command.name) == 0) {
        command.run(arguments);
        return;
      }
    }
    console::printf("Unknown command: %s. Type help.\n", name);
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
