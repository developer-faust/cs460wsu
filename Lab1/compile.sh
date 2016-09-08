#/bin/sh

as86 -o boot.o  boot.s
bcc -c -ansi  main.c
ld86 -d boot.o main.o myio.o /usr/lib/bcc/libc.a

dd if=a.out of=vdisk bs=16 count=27 conv=notrunc
dd if=a.out of=vdisk bs=512 seek=1  conv=notrunc