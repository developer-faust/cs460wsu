!================================== LEGEND ================================== 
!
!   All registers are 16-bit
!
!   IP = Instruction Pointer
!   PC = Program Counter --> next intstruction in memory
!   SR = Status Register = mode, interruptMask, conditionCode; mode=System/User
!   SP = Stack Pointer --> current stack top
!   AX = Return Value
!
!   FLAG = Status Register
!   
!   General Registers: AX  BX  CX  DX  BP  SI  DI

!   Segment Registers
!   -----------------
!   CS -> Code  Segment = program code or instructions
!   DS -> Data  Segment = static and global data (ONE COPY only)
!   SS -> Stack Segment = stack area for calling and local variables. 
!   ES -> Extra Segment = temp area; may be used for malloc()/mfree()
!=================================== ts.s =================================== 

        MTXSEG  = 0x1000                           
        .globl _main, _running, _scheduler
        .globl _proc, _procSize, _color
        .globl _putc, _getc, _tswitch, _getbp


        jmpi    start, MTXSEG                           ! CS=MTXSEG, IP=start

start:  mov     ax, cs                                  ! establish segments 
        mov     ds, ax                                  ! Let DS,SS,ES = CS=0x1000.
        mov     ss, ax
        mov     es, ax
        mov     sp, #_proc                              ! sp -> proc[0]
        add     sp, _procSize                           ! sp -> proc[0]''s HIGH END

        call    _main                                   ! call main() in C

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

_int80h:                                                ! save all Umode registers in ustack
        push    ax
        push    bx
        push    cx
        push    dx
        push    bp
        push    si
        push    di
        push    es
        push    ds
!                      |by INT 80 =>|   by _int80h: ============>|
!                   ---|-----------------------------------------------
! ustack contains : ???|flag,uCS,uPC|ax,bx,cx,dx,bp,si,di,ues,uds|
!                   -------------------------------------------|---- 
!                                                              |
!                                                             uSP

        push    cs                                      ! push kCS (0x1000) 
        pop     ds                                      ! let DS=CS = 0x1000 (KDS now) 

! All variables are now relative to DS=0x1000 of Kernel space
! save running proc''s Umode uSS and uSP into its PROC 

        mov     bx, _running                            ! ready to access proc 
        mov     USS[bx], ss                             ! save uSS in proc.USS
        mov     USP[bx], sp                             ! save uSP in proc.USP

! Change ES, SS to kernel segment

        mov     ax, ds                                  ! must mov segments this way!
        mov     es, ax                                  ! CS=DS=SS=ES in kmode 
        mov     ss, ax                                  ! SS is now KSS = 0x1000

! switch (running proc''s) stack from U space to K space.

        mov     sp, _running                            ! sp points at running proc (proc''s kstack [2 KB])
        add     sp, _procSize                           ! sp -> HIGH END of running''s kstack[]

! We are now completely in K space, and stack is running proc''s (empty) kstack
! *************   CALL handler in C ******************************

        call    _kcinth                                 ! call kcinth() in int.c
                                                        ! kc = Kmode code

! *************   RETURN TO U Mode ********************************
        jmp     _goUmode

!=============================================================================
! The assembly function goUmode() restores Umode stack, then restores Umode
! registers, followed by an IRET, causing the process to return to Umode.
!
! These assembly functions are keys to understanding Umode/Kmode transitions. 
!*****************************************************************************
_goUmode:
        cli                                             ! mask out interrupts
        mov bx,_running                                 ! bx -> proc
        mov ax,USS[bx]
        mov ss,ax                                       ! restore uSS
        mov sp,USP[bx]                                  ! restore uSP
  
        pop ds
        pop es
        pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax                                          ! NOTE: contains return value to Umode     
        
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
