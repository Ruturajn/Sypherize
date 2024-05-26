#!/bin/bash
# This script compiles all the sypher files present in the examples
# directory, and checks the exit code for deciding `PASS` / `FAIL`.

echo -e "\e[0;33m[ INFO ] : COMPILING Sypherc ...\e[0;37m"
make clean all &> /dev/null
declare fail_flag=0

echo -e "\n\e[0;33m[ INFO ] : RUNNING Parser Test ...\e[0;37m"
for file in ./tests/parser/* ; do
    make test FILE_PATH="${file}" &> /dev/null
    if [[ $? -ne 0 ]] ; then
        echo -e "\e[0;31m[ FAIL ] : ${file}\e[0;37m"
        fail_flag=1
    else
        echo -e "\e[0;36m[ PASS ] : ${file}\e[0;37m"
    fi
done

echo -e "\n\e[0;33m[ INFO ] : RUNNING Typechecker Test ...\e[0;37m"
for file in ./tests/typecheck/* ; do
    make test FILE_PATH="${file}" &> /dev/null
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
