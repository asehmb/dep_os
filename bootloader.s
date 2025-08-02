org 0x7C00

start:
    cli          ; disable interrupts
    xor ax, ax  ; clear AX
    mov ds, ax ; set data segment to zero, references start at 0x0000
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print "Hello World!"
    mov si, hello_world_string ; a pointer to the string
    call print_string

print_string:
    mov ah, 0x0E          ; teletype output BIOS function
.print_loop:
    lodsb                 ; AL = [SI], SI++
    cmp al, 0
    je .done ; if al == 0, null terminator, end of string
    int 0x10
    jmp .print_loop
.done:
    ret

done:
    hlt
    jmp done

hello_world_string:
    db "Hello World!", 0


times 510 - ($-$$) db 0
dw 0xAA55
