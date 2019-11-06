#!/bin/sh
set -e
. ./build.sh
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
 
cp sysroot/boot/hurbos.kernel isodir/boot/hurbos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hurbOS" {
	multiboot /boot/hurbos.kernel
}
EOF
grub-mkrescue -o hurbOS.iso isodir
