.globl _tswitch,_getc,_putc, _color										!EXPORT to C
.globl _main,_running,_scheduler,_proc0,_procSize 	!IMPORT


start: 	
	mov		ax, cs 									! set DS=SS=CS = 0x1000
	mov		ds, ax 
	mov		ss, ax
	mov		sp, #_proc0								! set sp point to proc0 in C code
	add 	sp, _procSize							! sp point to high end of proc0
	call    _main								! call main() in C
	hlt 										! if main() returns, just halt
_tswitch:
SAVE:
	push	ax
	push	bx
	push	cx
	push	dx
	push	bp
	push	si
	push	di
	pushf
	
	push 	ds 									 ! Save cpus segment register
	push 	ss

	mov		bx, _running
	mov		2[bx], sp
FIND:
	call _scheduler
RESUME:
	mov		bx, _running
	mov		sp, 2[bx]
	pop 	ss
	pop 	ds
	popf
	pop 	di
	pop 	si
	pop 	bp
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
	ret
_getc:

    xorb   ah,ah           ! clear ah
    int    0x16            ! call BIOS to get a char in AX
    andb   al,#0x7F        ! 7-bit ascii  
    ret 

 
_putc: 

    push   bp
    mov    bp,sp
    movb   al,4[bp]        ! get the char into aL
    movb   ah,#14          ! aH = 14
    mov    bx,_color       ! cyan
    int    0x10            ! call BIOS to display the char
    pop    bp
    ret
