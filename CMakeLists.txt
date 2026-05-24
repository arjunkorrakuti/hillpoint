cmake_minimum_required(VERSION 3.16)

# Load FetchContent module for fetching external dependencies
include(FetchContent)

# Stop CMake from trying to compile a non-freestanding test program with a freestanding compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(hillpoint LANGUAGES CXX)

# Set C++23 standard and enforce
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Remove defualt CMake flags
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)

# Define kernel executable
add_executable(kernel
  kmain.cpp
  kernel/runtime.cpp
  kernel/boot.cpp
  kernel/framebuffer.cpp
  libc/string.cpp
  kernel/font.cpp
)
target_include_directories(kernel PRIVATE kernel/include libc/include)
set_target_properties(kernel PROPERTIES OUTPUT_NAME "hillpoint")

# Fetch latest limine.h header from source and provide it to compilers include path
FetchContent_Declare(
  limine_h
  URL https://raw.githubusercontent.com/Limine-Bootloader/limine-protocol/refs/heads/trunk/include/limine.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(limine_h)
target_include_directories(kernel PRIVATE ${limine_h_SOURCE_DIR})

# Flags
target_compile_options(kernel PRIVATE
  # Warning flags
  -Wall
  -Wextra
  -Wpedantic
  -Wnull-dereference
  -Wsign-conversion
  -Wcast-qual
  -Wswitch-enum
  -Wredundant-decls
  -Woverloaded-virtual
  -Wold-style-cast
  -Woverloaded-virtual
  -Wnon-virtual-dtor
  -Wzero-as-null-pointer-constant
  -Wextra-semi
  -Wunknown-pragmas
  -Werror
  -Wshadow=local
  -Wlogical-op
  -Wduplicated-cond
  -Wduplicated-branches
  -Warray-bounds=2
  -Wuseless-cast

  # Kernel-specific flags
  -ffreestanding
  -fno-exceptions
  -fno-rtti
  -fno-stack-protector
  -fno-stack-check
  -mno-red-zone
  -mcmodel=kernel
  -fno-PIC
  -fno-lto
  -ffunction-sections
  -fdata-sections

  # Arch flags
  -m64
  -march=x86-64
  -mabi=sysv
  -mno-80387
  -mno-mmx
  -mno-sse
  -mno-sse2

  # Debug flags
  $<$<CONFIG:Debug>:-g3>

  # RelWithDebInfo flags
  $<$<CONFIG:RelWithDebInfo>:-O3>
  $<$<CONFIG:RelWithDebInfo>:-g3>

  # Release flags
  $<$<CONFIG:Release>:-O3>

  #MinSizeRel flags
  $<$<CONFIG:MinSizeRel>:-Os>
)
# Linker flags
target_link_options(kernel PRIVATE
  -nostdlib
  -static
  -z max-page-size=0x1000
  -T${PROJECT_SOURCE_DIR}/linker.lds
  -Wl,-Map=kernel.map
)

# Define script to build ISO
set(BUILD_ISO_SCRIPT [[
#!/bin/sh
set -eu

KERNEL="$1"
PROJECT_SOURCE_DIR="$2"
LIMINE_DIR="$3"
OUTPUT_ISO="$4"
ISO_ROOT="$(dirname "$OUTPUT_ISO")/iso_root"

rm -rf "$ISO_ROOT"
mkdir -p "$ISO_ROOT/boot/limine"
mkdir -p "$ISO_ROOT/EFI/BOOT"
cp -v "$KERNEL" "$ISO_ROOT/boot/hillpoint"
cp -v "$PROJECT_SOURCE_DIR/limine.conf" \
	"$LIMINE_DIR/limine-bios.sys" \
	"$LIMINE_DIR/limine-bios-cd.bin" \
	"$LIMINE_DIR/limine-uefi-cd.bin" \
	"$ISO_ROOT/boot/limine/"
cp -v "$LIMINE_DIR/BOOTX64.EFI" "$ISO_ROOT/EFI/BOOT/"
cp -v "$LIMINE_DIR/BOOTIA32.EFI" "$ISO_ROOT/EFI/BOOT/"

xorriso -as mkisofs -R -r -J \
	-b boot/limine/limine-bios-cd.bin \
	-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
	-apm-block-size 2048 \
	--efi-boot boot/limine/limine-uefi-cd.bin \
	-efi-boot-part --efi-boot-image --protective-msdos-label \
	"$ISO_ROOT" -o "$OUTPUT_ISO"

make -j"$(nproc)" -C "$LIMINE_DIR" >/dev/null 2>&1
"$LIMINE_DIR/limine" bios-install "$OUTPUT_ISO" >/dev/null 2>&1
]])
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tools/build_iso.sh" "${BUILD_ISO_SCRIPT}")

# Fetch Limine binary and call build_iso script to create bootable ISO
FetchContent_Declare(
  limine_binary
  URL https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(limine_binary)
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/hillpoint.iso"
  COMMAND sh "${CMAKE_CURRENT_BINARY_DIR}/tools/build_iso.sh"
          "$<TARGET_FILE:kernel>"
          "${PROJECT_SOURCE_DIR}"
          "${limine_binary_SOURCE_DIR}"
          "${CMAKE_CURRENT_BINARY_DIR}/hillpoint.iso"
  DEPENDS kernel
          "${CMAKE_CURRENT_BINARY_DIR}/tools/build_iso.sh"
          "${PROJECT_SOURCE_DIR}/limine.conf"
  VERBATIM
  USES_TERMINAL
)
add_custom_target(iso DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/hillpoint.iso")

# Export compile commands for tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
