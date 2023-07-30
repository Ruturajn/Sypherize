#!/ bin / bash

#Save current working directory.
CWD=$(pwd)
WORKING_DIR=$(find ~ -name Sypherize)

cd "${WORKING_DIR}"

make clean all
printf "\n\n"

for file in ./examples/* ; do
    if [[ "${file}" != "./examples/test_all_files.sh" ]] ; then
        make test FILE_PATH="${file}"
        if [[ $? -ne 0 ]] ; then
            echo -e "\e[0;31m\nERROR : Failed Test - ${file}\e[0;37m"
            cd "${CWD}"
            exit 1
        fi
    fi
done

echo -e "\e[0;36m\nSUCCESS : All Tests Passed\e[0;37m"
cd "${CWD}"
