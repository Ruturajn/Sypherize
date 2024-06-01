; Generated From: ./tests/compile/test5.sy
target triple = "x86_64-pc-linux-gnu"

define i64 @main(i64 %_argc0, { i64, [ 0 x i8* ] }* %_argv3) {
	%_alloca_arg1 = alloca i64
	store i64 %_argc0, i64* %_alloca_arg1
	%_alloca_arg4 = alloca { i64, [ 0 x i8* ] }*
	store { i64, [ 0 x i8* ] }* %_argv3, { i64, [ 0 x i8* ] }** %_alloca_arg4
	%_g6 = alloca { i64, [ 0 x i64 ] }*
	%_alloca_carr7 = alloca { i64, [ 5 x i64 ] }
	%_carr_gep8 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 0
	store i64 5, i64* %_carr_gep8
	%_carr_gep10 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 1, i32 0
	store i64 0, i64* %_carr_gep10
	%_carr_gep12 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 1, i32 1
	store i64 1, i64* %_carr_gep12
	%_carr_gep14 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 1, i32 2
	store i64 2, i64* %_carr_gep14
	%_carr_gep16 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 1, i32 3
	store i64 3, i64* %_carr_gep16
	%_carr_gep18 = getelementptr { i64, [ 5 x i64 ] }, { i64, [ 5 x i64 ] }*  %_alloca_carr7, i32 0, i32 1, i32 4
	store i64 4, i64* %_carr_gep18
	%_bitcast_carr20 = bitcast { i64, [ 5 x i64 ] }* %_alloca_carr7 to { i64, [ 0 x i64 ] }*
	store { i64, [ 0 x i64 ] }* %_bitcast_carr20, { i64, [ 0 x i64 ] }** %_g6
	%_id_exp22 = load { i64, [ 0 x i64 ] }*, { i64, [ 0 x i64 ] }** %_g6
	%_index_gep23 = getelementptr { i64, [ 0 x i64 ] }, { i64, [ 0 x i64 ] }*  %_id_exp22, i32 0, i32 1, i32 3
	store i64 96, i64* %_index_gep23
	%_id_exp25 = load { i64, [ 0 x i64 ] }*, { i64, [ 0 x i64 ] }** %_g6
	%_index_gep26 = getelementptr { i64, [ 0 x i64 ] }, { i64, [ 0 x i64 ] }*  %_id_exp25, i32 0, i32 1, i32 3
	%_index_load27 = load i64, i64* %_index_gep26
	ret i64 %_index_load27
}
