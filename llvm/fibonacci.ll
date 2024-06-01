; Generated From: ./tests/compile/test7.sy
target triple = "x86_64-pc-linux-gnu"

define i64 @fib(i64 %_n0) {
	%_alloca_arg1 = alloca i64
	store i64 %_n0, i64* %_alloca_arg1
	%_id_exp3 = load i64, i64* %_alloca_arg1
	%_icmp4 = icmp slt i64 %_id_exp3, 1
	br i1 %_icmp4, label %_if_then5, label %_if_else6
_if_then5:
	ret i64 1
_if_else6:
	br label %_if_merge7
_if_merge7:
	%_sum8 = alloca i64
	store i64 0, i64* %_sum8
	%_prev10 = alloca i64
	store i64 0, i64* %_prev10
	%_curr12 = alloca i64
	store i64 1, i64* %_curr12
	%_i14 = alloca i64
	store i64 0, i64* %_i14
	br label %_for_cond16
_for_cond16:
	%_id_exp17 = load i64, i64* %_i14
	%_id_exp18 = load i64, i64* %_alloca_arg1
	%_icmp19 = icmp slt i64 %_id_exp17, %_id_exp18
	br i1 %_icmp19, label %_for_body20, label %_for_exit21
_for_body20:
	%_id_exp22 = load i64, i64* %_prev10
	%_id_exp23 = load i64, i64* %_curr12
	%_bop24 = add i64 %_id_exp22, %_id_exp23
	store i64 %_bop24, i64* %_sum8
	%_id_exp26 = load i64, i64* %_curr12
	store i64 %_id_exp26, i64* %_prev10
	%_id_exp28 = load i64, i64* %_sum8
	store i64 %_id_exp28, i64* %_curr12
	%_id_exp30 = load i64, i64* %_i14
	%_bop31 = add i64 %_id_exp30, 1
	store i64 %_bop31, i64* %_i14
	br label %_for_cond16
_for_exit21:
	%_id_exp33 = load i64, i64* %_sum8
	ret i64 %_id_exp33
}

define i64 @fib_recurse(i64 %_n42) {
	%_alloca_arg43 = alloca i64
	store i64 %_n42, i64* %_alloca_arg43
	%_id_exp45 = load i64, i64* %_alloca_arg43
	%_icmp46 = icmp slt i64 %_id_exp45, 1
	br i1 %_icmp46, label %_if_then47, label %_if_else48
_if_then47:
	ret i64 1
_if_else48:
	br label %_if_merge49
_if_merge49:
	%_id_exp50 = load i64, i64* %_alloca_arg43
	%_bop51 = sub i64 %_id_exp50, 1
	%_funcall52 = call i64 @fib (i64 %_bop51)
	%_id_exp53 = load i64, i64* %_alloca_arg43
	%_bop54 = sub i64 %_id_exp53, 2
	%_funcall55 = call i64 @fib (i64 %_bop54)
	%_bop56 = add i64 %_funcall52, %_funcall55
	ret i64 %_bop56
}

define i64 @main(i64 %_argc62, { i64, [ 0 x i8* ] }* %_argv65) {
	%_alloca_arg63 = alloca i64
	store i64 %_argc62, i64* %_alloca_arg63
	%_alloca_arg66 = alloca { i64, [ 0 x i8* ] }*
	store { i64, [ 0 x i8* ] }* %_argv65, { i64, [ 0 x i8* ] }** %_alloca_arg66
	%_fib_ans168 = alloca i64
	%_funcall69 = call i64 @fib (i64 5)
	store i64 %_funcall69, i64* %_fib_ans168
	%_fib_ans271 = alloca i64
	%_funcall72 = call i64 @fib_recurse (i64 5)
	store i64 %_funcall72, i64* %_fib_ans271
	%_id_exp74 = load i64, i64* %_fib_ans271
	ret i64 %_id_exp74
}
