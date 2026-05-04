#!/bin/sh

DEBUG=$1

if [ "$DEBUG" = "debug" ]; then
	qemu-system-x86_64 -name "Hillpoint - Debug" -enable-kvm -cpu host -m 2G -boot d \
		-snapshot -display gtk,show-tabs=on -cdrom build/hillpoint.iso -daemonize \
		-s -S
else
	qemu-system-x86_64 -name "Hillpoint" -enable-kvm -cpu host -m 2G -boot d \
		-snapshot -display gtk,show-tabs=on -cdrom build/hillpoint.iso -daemonize
fi
