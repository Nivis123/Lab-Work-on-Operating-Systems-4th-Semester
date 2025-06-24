section .data
    hello db 'Hello, World!', 0xA  ; строка с символами и переводом строки

section .text
    global _start

_start:
    
    mov edx, 14          ; длина строки
    mov ecx, hello       ; указатель на строку
    mov ebx, 1           ; дескриптор файла (stdout)
    mov eax, 4           ; номер системного вызова (write)
    int 0x80             ; вызов ядра

    
    mov eax, 1           ; номер системного вызова (exit)
    xor ebx, ebx         ; код возврата 0
    int 0x80             ; вызов ядра


;nasm -f elf32 hello.asm -o hello.o
;ld -m elf_i386 hello.o -o hello
