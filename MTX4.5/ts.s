.globl		_tswitch 											!EXPORT to C
.globl 		_main,_running,_scheduler,_proc0,_procSize 			!IMPORT
 

start: 
	mov 	ax,cs 					! set DS=SS=CS = 0x1000
	mov 	ds,ax
	mov 	ss,ax
	mov 	sp, #_proc0				! set sp point to proc0 in C code
	add 	sp,_procSize			! sp point to high end of proc0
	call 	_main					! call main() in C 
	hlt 							! if main() returns, just halt.

_tswitch:							! tswitch() function

SAVE:
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	bp
	push 	si
	push 	di
	pushf
	mov		bx,_running				! bx -> proc
	mov		2[bx],sp				! save sp to proc.ksp

FIND:	
	call 	_scheduler				! call scheduler() in C

RESUME: mov 	bx,_running				! bx -> running proc
	mov 	sp,2[bx]				! load sp with proc.ksp
	popf	 
	pop 	di
	pop 	si
	pop		bp
	pop		dx
	pop		cx
	pop		bx
	pop		ax
	ret
