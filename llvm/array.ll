target triple = "x86_64-pc-linux-gnu"

define i64 @main(i64 %0, i8** %1) {
    ; Fixed size CArrExpNode
    %3 = alloca { i64, [3 x i64] }
    %4 = getelementptr { i64, [3 x i64] }, { i64, [3 x i64] }* %3, i32 0, i32 0
    store i64 3, i64* %4
    %5 = getelementptr { i64, [3 x i64] }, { i64, [3 x i64] }* %3, i32 0, i32 1, i32 0
    store i64 1, i64* %5
    %6 = getelementptr { i64, [3 x i64] }, { i64, [3 x i64] }* %3, i32 0, i32 1, i32 1
    store i64 2, i64* %6
    %7 = getelementptr { i64, [3 x i64] }, { i64, [3 x i64] }* %3, i32 0, i32 1, i32 2
    store i64 3, i64* %7
    ret i64 0
}
