.section .data
z: .space 8
a: .space 8
.section .text
jmp afterbar
bar:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
mov 16(%rbp), %rax
movq $10, %r10
movq $3, %r11
imul %r10, %r11
add %rax, %r11
mov %r11, 16(%rbp)
mov 16(%rbp), %rax
add $32, %rsp
pop %rbp
ret
afterbar:
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $10, a(%rip)
movq $99, z(%rip)
movq $2, %rax
movq $3, %r10
imul %rax, %r10
movq $2, %rax
movq $1, %r11
add %rax, %r11
movq $8, %rax
pushq %rax
call bar
add $8, %rsp
add $32, %rsp
pop %rbp
ret
