#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/timer.hpp>
#include <kernel/io.hpp>
#include <kernel/keyboard.hpp>
#include <kernel/line_editor.hpp>
#include <kernel/memory.hpp>
#include <kernel/runtime.hpp>
#include <kernel/shell.hpp>
#include <string.h>

namespace {
  constinit LineEditor editor;
  void* allocations[16] = {};

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

  void uptime(char*) {
    const uint64_t milliseconds = timer::milliseconds();
    console::printf("Uptime: %llu ms (%llu PIT ticks)\n",
                    static_cast<unsigned long long>(milliseconds),
                    static_cast<unsigned long long>(timer::ticks()));
  }

  void irq(char*) {
    console::printf(
      "IRQ 0 timer: %llu\nIRQ 1 keyboard: %llu\n"
      "Dropped keys: %zu\n",
      static_cast<unsigned long long>(interrupts::count(0)),
      static_cast<unsigned long long>(interrupts::count(1)), keyboard::dropped());
  }

  void stop(char*) {
    console::printf("System halted.\n");
    halt();
  }

  void mem(char*) {
    const memory::PageStats pages = memory::pages().stats();
    const memory::HeapStats heap = memory::heap().stats();
    console::printf("Pages: %zu free / %zu managed; %zu metadata pages, %zu regions\n",
                    pages.freePages, pages.totalPages, pages.metadataPages,
                    pages.regions);
    console::printf("Heap: %zu used, %zu free, %zu largest free, %zu allocations\n",
                    heap.usedBytes, heap.freeBytes, heap.largestFree, heap.allocations);
  }

  bool number(const char* text, size_t& value) {
    value = 0;
    if (*text == '\0') {
      return false;
    }
    while (*text != '\0') {
      if (*text < '0' || *text > '9') {
        return false;
      }
      const size_t digit = static_cast<size_t>(*text++ - '0');
      if (value > (SIZE_MAX - digit) / 10) {
        return false;
      }
      value = value * 10 + digit;
    }
    return true;
  }

  void alloc(char* arguments) {
    size_t size;
    if (!number(arguments, size) || size == 0) {
      console::printf("Usage: alloc <bytes>\n");
      return;
    }
    for (size_t slot = 0; slot < 16; slot++) {
      if (allocations[slot] == nullptr) {
        allocations[slot] = memory::heap().allocate(size);
        if (allocations[slot] == nullptr) {
          console::printf("Allocation failed: insufficient contiguous heap space.\n");
        } else {
          console::printf("Allocated slot %zu: %zu bytes at %p\n", slot, size,
                          allocations[slot]);
        }
        return;
      }
    }
    console::printf("All 16 demo slots are occupied. Free one first.\n");
  }

  void help(char*);

  struct Command {
    const char* name;
    const char* description;
    void (*run)(char* arguments);
  };

  const Command commands[] = {
    {"alloc", "alloc <bytes>: allocate a demo heap block", alloc},
    {"mem", "Physical page and heap statistics", mem},
    {"halt", "Stop the CPU", stop},
    {"irq", "Interrupt and input counters", irq},
    {"uptime", "Time since the PIT started", uptime},
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
