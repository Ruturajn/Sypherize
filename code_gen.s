.section .data
a: .space 8
.section .text
jmp afteradd_int
add_int:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
sub $8, %rsp
mov 0(%rbp), %rax
add $40, %rsp
add $32, %rsp
pop %rbp
ret
afteradd_int:
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $-2, %r10
movq $3, %r11
imul %r10, %r11
movq $1, %r10
add %r11, %r10
movq $2, %r11
sub %r10, %r11
sub $8, %rsp
mov $a(%rip), %r11
mov %r11, %rax
add $40, %rsp
add $32, %rsp
pop %rbp
ret
