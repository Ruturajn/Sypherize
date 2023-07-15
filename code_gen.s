.section .data
test: .space 8
new_val: .space 8
a: .space 8
.section .text
jmp afteradd_int
add_int:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $33, %rax
mov %rax, 24(%rbp)
movq $99, %rax
mov %rax, 16(%rbp)
movq $10, %rax
mov %rax, 24(%rbp)
movq $100, %rax
mov %rax, 16(%rbp)
sub $8, %rsp
movq $10, %rax
mov %rax, 16(%rbp)
add $40, %rsp
pop %rbp
ret
afteradd_int:
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $10, a(%rip)
movq $93, a(%rip)
movq $11, new_val(%rip)
movq $83, new_val(%rip)
movq $13, %rax
pushq %rax
movq $94, %rax
pushq %rax
call add_int
add $16, %rsp
movq $14, %rax
movq $12, %r10
add %rax, %r10
mov %r10, test(%rip)
movq $2, %rax
movq $3, %r10
imul %rax, %r10
movq $2, %rax
movq $1, %r11
add %rax, %r11
mov %r11, %rax
add $32, %rsp
pop %rbp
ret
