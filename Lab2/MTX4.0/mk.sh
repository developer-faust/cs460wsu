VFD=mtximage

echo compiling ts.s and making ts.o files
as86 -o ts.o ts.s

echo compiling t.c and linking library io functions
bcc -c -ansi t.c
ld86 -d -o mtx ts.o t.o myio.o mtxlib /usr/lib/bcc/libc.a
echo done!
echo
mount -o loop $VFD /mnt
echo $VFD mounted on /mnt
(ls -l /mnt)
 
echo

echo ready to copy mtx to mtximage/boot 
echo 
cp mtx /mnt/boot
(ls -l /mnt/boot)
echo done

echo umount /mnt
umount /mnt
echo done
echo booting image now .......
echo 
echo
qemu-system-i386 -fda $VFD -no-fd-bootchk