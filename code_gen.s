.section .data
.section .text
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $-2, %rax
movq $3, %r10
imul %rax, %r10
movq $1, %rax
add %r10, %rax
movq $2, %r10
sub %rax, %r10
add $32, %rsp
pop %rbp
ret
