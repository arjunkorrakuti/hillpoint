#!/bin/sh

# Capture first argument as $DEBUG
DEBUG=$1

# -s -S pauses and waits for a debugger to attach
if [ "$DEBUG" = "debug" ]; then
	qemu-system-x86_64 -name hillpoint -enable-kvm -cpu host -m 2G -boot d \
		-snapshot -display gtk,show-tabs=on -cdrom build/hillpoint.iso -daemonize \
		-s -S
else
	qemu-system-x86_64 -name hillpoint -enable-kvm -cpu host -m 2G -boot d \
		-snapshot -display gtk,show-tabs=on -cdrom build/hillpoint.iso -daemonize
fi
