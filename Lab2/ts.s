        .globl _tswitch 		                          	! EXPORT 
        .globl _main,_running, _color, _scheduler			! IMPORT
        .globl _proc,_procSize, _getc, _putc 				! IMPORT

start:
        mov     ax,cs                   ! establish segments 
        mov     ds,ax                   ! Let DS,SS,ES = CS=0x1000.
        mov     ss,ax                   
        mov     es,ax
        
        mov     sp,#_proc               ! sp -> proc[0]
        add     sp,_procSize            ! sp -> proc[0]'s HIGH END
        
        call 	_main                      ! call main() in C
        hlt


_tswitch:
SAVE:	
        push ax
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        pushf							! Push flag register


        ! Added lines for lab 2
        
        push ds 						! push data segment
        push ss 						! push stack segment

        ! End added lines

        mov   bx, _running
        mov   2[bx], sp
        call _scheduler	
RESUME:	
        mov   bx, _running				
        mov   sp, 2[bx]					! accesses p->ksp (2[bx] second member variable of the running proc is that proc's ksp)
        
        ! Pop values to be used by c code
        pop ss
        pop ds
        popf							! Pop the flag register
        

        pop  di							! destination index
        pop  si 						! source index for string copying
        pop  bp 						! base pointer
        pop  dx 						! general purpose registers
        pop  cx
        pop  bx
        pop  ax
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
        movb   ah, #14          ! aH = 14
        mov    bx,_color       ! cyan
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret
