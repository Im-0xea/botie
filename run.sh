#!/bin/sh
WAYLAND_DISPLAY=wayland-1 \
SDL_VIDEODRIVER=wayland \
qemu-system-x86_64 -drive file=hd.img,format=raw -display sdl
