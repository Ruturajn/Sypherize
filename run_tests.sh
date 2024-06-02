#!/bin/bash
# This script compiles all the sypher files present in the examples
# directory, and checks the exit code for deciding `PASS` / `FAIL`.

declare fail_flag=0
PROG=./bin/sypherc

echo -e "\e[0;33m[ INFO ] : RUNNING Parser Tests ...\e[0;37m"
for file in ./tests/parser/* ; do
    "${PROG}" --run-parser "${file}" &> /dev/null
    if [[ $? -ne 0 ]] ; then
        echo -e "\e[0;31m[ FAIL ] : ${file}\e[0;37m"
        fail_flag=1
    else
        echo -e "\e[0;36m[ PASS ] : ${file}\e[0;37m"
    fi
done

echo -e "\n\e[0;33m[ INFO ] : RUNNING Typechecker Tests ...\e[0;37m"
for file in ./tests/typecheck/* ; do
    "${PROG}" --run-typechecker "${file}" &> /dev/null
    if [[ $? -ne 0 ]] ; then
        echo -e "\e[0;31m[ FAIL ] : ${file}\e[0;37m"
        fail_flag=1
    else
        echo -e "\e[0;36m[ PASS ] : ${file}\e[0;37m"
    fi
done

if [[ "${fail_flag}" -eq 0 ]] ; then
    echo -e "\e[0;36m\nALL TESTS PASSED\e[0;37m"
fi

echo -e "\n\e[0;33m[ INFO ] : RUNNING LLVM Compiler Tests ...\e[0;37m"
for file in ./tests/compile/* ; do
    "${PROG}" "${file}" &> /dev/null
    if [[ $? -ne 0 ]] ; then
        echo -e "\e[0;31m[ FAIL ] : ${file}\e[0;37m"
        fail_flag=1
    else
        echo -e "\e[0;36m[ PASS ] : ${file}\e[0;37m"
    fi
done

if [[ "${fail_flag}" -eq 0 ]] ; then
    echo -e "\e[0;36m\nALL TESTS PASSED\e[0;37m"
fi
