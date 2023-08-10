#!/bin/bash
# This script compiles all the sypher files present in the examples
# directory, and checks the exit code for deciding `PASS` / `FAIL`.

#Save current working directory.
CWD=$(pwd)
echo -e "\e[0;33m\n[ INFO ] : NAVIGATING to Sypherize ...\e[0;37m"
WORKING_DIR=$(find ~ -name Sypherize)

cd "${WORKING_DIR}"

echo -e "\e[0;33m[ INFO ] : COMPILING Sypherize ...\e[0;37m"
make clean all &> /dev/null
declare fail_flag=0

for file in ./examples/* ; do
    make test FILE_PATH="${file}" &> /dev/null
    if [[ $? -ne 0 ]] ; then
        file=$(echo "${file}" | sed 's|./examples/||')
        echo -e "\e[0;31m[ FAIL ] : ${file}\e[0;37m"
        fail_flag=1
    else
        file=$(echo "${file}" | sed 's|./examples/||')
        echo -e "\e[0;36m[ PASS ] : ${file}\e[0;37m"
    fi
done

if [[ "${fail_flag}" -eq 0 ]] ; then
    echo -e "\e[0;36m\nALL TESTS PASSED\e[0;37m"
fi
cd "${CWD}"
