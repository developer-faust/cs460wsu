        MTXSEG  = 0x1000

        .globl _main, _running, _scheduler
        .globl _proc, _procSize, _color
        .globl _putc, _getc, _tswitch, _getbp


        jmpi    start, MTXSEG

start:  mov     ax, cs
        mov     ds, ax
        mov     ss, ax
        mov     es, ax
        mov     sp, #_proc
        add     sp, _procSize

        call    _main

_tswitch:
SAVE:   push    ax
        push    bx
        push    cx
        push    dx
        push    bp
        push    si
        push    di
        pushf
        mov     bx, _running
        mov     2[bx], sp

FIND:   call    _scheduler

RESUME: mov     bx, _running
        mov     sp, 2[bx]
        popf
        pop     di
        pop     si
        pop     bp
        pop     dx
        pop     cx
        pop     bx
        pop     ax

        ret

        ! added functions for KUMODE

        .globl  _int80h, _goUmode, _kcinth

        ! Offsets defined in PROC structure
USS = 4
USP = 6

_int80h:
        push    ax
        push    bx
        push    cx
        push    dx
        push    bp
        push    si
        push    di
        push    es
        push    ds

        ! ustack contains : flag, uCS, iPC, ax, bx, cx, dx, bp, si, di, ues, uds
        push    cs
        pop     ds              ! KDS now

        mov     bx, _running    ! ready to access proc 
        mov     USS[bx], ss     ! save uSS in proc.USS
        mov     USP[bx], sp     ! save uSP in proc.USP

        ! Change ES, SS to kernel segment
        mov     ax, ds          ! stupid !! why
        mov     es, ax          ! CS=DS=SS=ES in kmode 
        mov     ss, ax

        ! set sp to HI end of running!''s kstack[]
        mov     sp, _running    ! proc''s kstack[2 KB]
        add     sp, _procSize   ! HI end of PROC

        call    _kcinth
        jmp     _goUmode

_goUmode:
        cli
        mov bx,_running         ! bx -> proc
        mov ax,USS[bx]
        mov ss,ax               ! restore uSS
        mov sp,USP[bx]          ! restore uSP
  
        pop ds
        pop es
        pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax                  ! NOTE: contains return value to Umode     
        
        iret



_getbp:
        mov ax, bp
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
