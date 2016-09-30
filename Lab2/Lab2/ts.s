        OSSEG  = 0x1000
	
       .globl _main,_running,_scheduler
       .globl _proc, _procSize, _color
       .globl _tswitch,_resume, _getc, _putc, _getbp
	
        jmpi   start,OSSEG

start:	mov  ax,cs
	mov  ds,ax
	mov  ss,ax
        mov  es,ax
	mov  sp,#_proc
	add  sp,_procSize
		
	call _main

_tswitch:
SAVE:	
	push ax
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf
	mov  bx,_running
	mov  2[bx],sp

FIND:	call _scheduler

_resume:	
RESUME:
	mov  bx,_running
	mov  sp,2[bx]
	popf
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax

	ret

_getbp:
    mov ax, bp
    ret

_getc:
        xorb   ah,ah 
        int    0x16  
        ret 

_putc:           
        push   bp
        mov    bp,sp
        movb   al,4[bp]
        movb   ah,#14  
        mov    bx,_color   ! color is global set in C code by pid
        int    0x10    
        pop    bp
        ret

