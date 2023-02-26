bits 32
global rtc_isr, sys_call_isr, serial_isr

; RTC interrupt handler
; Tells the slave PIC to ignore interrupts from the RTC
rtc_isr:
	cli
	push ax
	
	; Tell the PIC this is EOI
	; This really should be done
	; at the RTC level -- but this is
	; okay for now...
	mov al, 0x20
	out 0xA0, al
	
	pop ax
	sti
	iret

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
    cmp eax, 3
    je pre_exit         ; Check if we should jump to the end
    cmp eax, 2
    je pre_exit         ; Check if we should jump to the end
    pusha               ; Push all the general things into the stack
    push ss
    push ds
    push es
    push fs
    push gs
    push esp
    push eax
	call sys_call
	mov ESP, EAX
	pop gs
	pop fs
	pop es
	pop ds
	pop ss
	popa                ; It appears that the issue is stemming from ESP not being set properly. Apparently popa does not set esp, but iret does?
	iret
	pre_exit:
	mov eax, -1
	iret

;;; Serial port ISR. To be implemented in Module R6
serial_isr:
	iret
