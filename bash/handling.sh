#!/bin/bash

function error_handle {
    case $1 in
        --wrong-arg-numbers)
            shift
            echo "Handler 1: Passed wrong number of arguments (expected $1)"
        ;;
        --not-a-number)
            shift
            echo "Handler 2: \"$1\" is not a number"
        ;;
        --unexpected-value)
            shift
            local input=${1}
            shift
            local expectation=${1}
            printf "Handler 3: \"%s\" is unexpected value" ${input}
            if [[ ${expectation} ]];then printf " (Expectations: \"%s\")\n" ${expectation}; fi
        ;;
        *) return
        ;;
    esac
}

## Test handlers
[[ $# -ne 1 ]] && error_handle --wrong-arg-numbers 1
[[ 'a' =~ ^[0-9]+$ ]] || error_handle --not-a-number 'a'
[[ "myvar" != "var" ]] && error_handle --unexpected-value "myvar" "var"
