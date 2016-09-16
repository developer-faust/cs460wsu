echo compiling ts.s and t.c file
as86 -o ts.o ts.s
bcc -c -ansi t.c 	

echo linking ts.o t.o with library and creating mtx0 program
ld86 -d -o mtx0 ts.o t.o mtxlib /usr/lib/bcc/libc.a

echo copying mtx0 to mtximage/boot/mtx0
mount -o loop mtximage /mnt
cp mtx0 /mnt/boot/
umount /mnt

echo done


echo booting
qemu-system-i386 -fda mtximage -no-fd-bootchk
