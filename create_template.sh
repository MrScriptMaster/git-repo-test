#!/bin/bash
################################################################################
## Version: 1.0
## Description:
##   Script for parsing core-dumps and creating reports.
##
## Author: Grigory Okhmak
################################################################################
##---------------------
## GLOBAL VARIABLES
##---------------------
## Print debug messages if it's not zero
FLAG_VERBOSE=0
ANSWER="n"
################################################################################
## LIBRARY
################################################################################
## _say <message> [-E|-W|-h|-i]
##
##  Print the message if FLAG_VERBOSE is not zero.
##
##  Params:
##   <message> (string) - printing message
##   Optional flags (for coloring printing text):
##     -E|--error
##     -W|--warning
##     -h|--head-line
##     -i|--info
##
##  Example:
##    _say "Print me" --info
##  
################################################################################
_say() {
    if [ $FLAG_VERBOSE -ne 0 ]; then
        case $2 in
            -E|--error)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[31;1m%s\e[0m\n' "$1"
            ;;
            -W|--warning)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[33;1m%s\e[0m\n' "$1"
            ;;
            -h|--head-line)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[32;3;1m%s\e[0m\n' "$1"
            ;;
            -i|--info)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[35m%s\e[0m\n' "$1"
            ;;
            *)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[0;36m%s \e[0;32m%s \e[0m\n' "$1"
            ;;
        esac
    fi
}
################################################################################
## _say1 <message> [-E|-W|-h|-i]
##
##  Print the message.
##
##  Params:
##   <message> (string) - printing message
##   Optional flags (for coloring printing text):
##     -E|--error
##     -W|--warning
##     -h|--head-line
##     -i|--info
##
##  Example:
##    _say1 "Print me" --info
##  
################################################################################
_say1() {
    case $2 in
            -E|--error)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[31;1m%s\e[0m\n' "$1"
            ;;
            -W|--warning)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[33;1m%s\e[0m\n' "$1"
            ;;
            -h|--head-line)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[32;3;1m%s\e[0m\n' "$1"
            ;;
            -i|--info)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[35m%s\e[0m\n' "$1"
            ;;
            *)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[0;36m%s \e[0;32m%s \e[0m\n' "$1"
            ;;
        esac
}
################################################################################
## die <optional-message>
##
##  Kill this process (for error situations).
##  
################################################################################
die() {
    _say1 "$1" --error
    exit 1
}
################################################################################
## _ask <question>
##
##  Ask some qustion to the user. Answer will be written to ANSWER.
##
##  Params:
##   <question> (string) - A question. The answer should be yes or no.
##
##  Example:
##    _ask "Continue?"
##  
################################################################################
_ask() {
    printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
    printf '\e[0;36m%s \e[0;32m%s \e[0m' "$1 ([y|]n):"
    read ANSWER
    if [ $ANSWER != "y" ]; then
        ANSWER="n"
    else
        ANSWER="y"
    fi
}
################################################################################
## _ask1 <question>
##
##  Asking for input data. Answer will be written to ANSWER.
##
##  Params:
##   <question> (string) - A question.
##
##  Example:
##    _ask "Enter your name"
##  
################################################################################
_ask1() {
    printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
    printf '\e[0;36m%s \e[0;32m%s \e[0m' "$1:"
    read ANSWER
}
################################################################################
assert_not_empty_string() {
    if [ -n "$2" ]; then
        _say "$1 = $2" --info
    else
        die "\"$1\" must not be empty"
    fi
}
################################################################################
## SCRIPT'S PROCEDURES
################################################################################
## usage
##
##  Print usage message for this script.
##  
################################################################################
usage() {
    echo "Usage:"
    printf "$1 [-h|-?|--help] [-v|--verbose] [-t|--test]\n"
}
################################################################################
## process_flags <args>
##
##  Process input flags for this script.
##  
################################################################################
process_flags() {
    while :; do
        case $1 in
            -t|--test)
                _say1 "TERM_SYSROOT_PATH=$TERM_SYSROOT_PATH" --info
                _say1 "TERM_COMPILER_PATH=$TERM_COMPILER_PATH" --info
                exit 0
                ;;
            -h|-\?|--help)
                usage $0
                exit
                ;;
            -v|--verbose)
                FLAG_VERBOSE=1
                ;;
            --)
                shift
                break
                ;;
            -?*)
                printf '\e[33;1m[WARNING]: Unknown option (ignored): %s\e[0m\n' "$1" >&2
                ;;
            *)
                break
        esac
        shift
    done
}
################################################################################
## MAIN
################################################################################
process_flags $*
## Initializing
OUTPUT_FILE_NAME="trace-$(date '+%F:%T:%N').txt"; assert_not_empty_string "OUTPUT_FILE_NAME" $OUTPUT_FILE_NAME
if [ ! "$TERM_SYSROOT_PATH" ]; then
    _ask1 "Enter path to the sysroot"; SYSROOT_PATH=$ANSWER; assert_not_empty_string "SYSROOT_PATH" $SYSROOT_PATH
else
    SYSROOT_PATH="$TERM_SYSROOT_PATH"; assert_not_empty_string "SYSROOT_PATH" $SYSROOT_PATH
fi
if [ ! "$TERM_COMPILER_PATH" ]; then
    _ask1 "Enter path to the compiler"; COMPILER_PATH=$ANSWER; assert_not_empty_string "COMPILER_PATH" $COMPILER_PATH
else
    COMPILER_PATH=$TERM_COMPILER_PATH; assert_not_empty_string "COMPILER_PATH" $COMPILER_PATH
fi
_ask1 "Enter path to the core-file"; CORE_FILE=$ANSWER; assert_not_empty_string "CORE_FILE" $CORE_FILE
_ask1 "Enter firmware-sym"; FIRMWARE_SYM_FILE=$ANSWER; assert_not_empty_string "FIRMWARE_SYM_FILE" $FIRMWARE_SYM_FILE
## Batch body
PARAMS="--batch -ex set width 0 "\
"-ex set solib-absolute-prefix "$SYSROOT_PATH" "\
"-ex file "$FIRMWARE_SYM_FILE" "\
"-ex core-file "$CORE_FILE" "\
"-ex thread apply all bt > "$OUTPUT_FILE_NAME" "
## Command
echo "$COMPILER_PATH $PARAMS"
exit 0
