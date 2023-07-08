.section .data
.section .text
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $2, %rax
movq $2, %r10
movq $3, %r11
imul %r10, %r11
add %rax, %r10
movq $8, %rax
sub %r10, %rax
mov %r10, %rax
add $32, %rsp
pop %rbp
ret
