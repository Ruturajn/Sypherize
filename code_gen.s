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
add $32, %rsp
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
movq $14, %rax
movq $12, %r10
add %rax, %r10
mov %r10, test(%rip)
add $32, %rsp
pop %rbp
ret
