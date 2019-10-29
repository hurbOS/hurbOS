mkdir -p isodir/boot/grub
cp hurbOS.bin isodir/boot/hurbOS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o hurbOS.iso isodir
