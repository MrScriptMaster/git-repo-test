#!/bin/bash
# Script version: 2.0                                                          #
# Description:                                                                 #
#   This script controls the procedure for applying and rolling back patches   #
#   for the Callback Assist components.                                        #
#   For detailed usage information, use -h key or --help.                      #
#   Review the README.txt file that comes with this script before applying     #
#   patches.                                                                   #
################################################################################
# !!! IMPORTANT !!!                                                            #
# DO NOT EDIT THIS SCRIPT IF YOU ARE NOT SURE OF WHAT A PARTICULAR PART OF IT  #
# DOES                                                                         #
################################################################################

PATCH_NAME="enter-patch-version"
CBA_VERSION="5.x.x.x"
DESCRIPTION="The script apply $PATCH_NAME for Callback Assist $CBA_VERSION."
CONTENT_DIR="content"
ROOT_DIR='.' #'/opt/Avaya'
TARGET_DIR="$ROOT_DIR/callbackassist"
VERSION_FILE="$ROOT_DIR/callbackassist/version"
PATCH_SH_TEMPLATE='patch.sh'
PATCH_README_TEMPLATE='readme.txt'
STORAGE='storage.data'
BACKUP_DIR_PREFIX='backup'
CONFIGURATION=""
STOP_AT_FIRST_FAILURE=0
MISSES=0
LIB='.toolkit.include'

#-------------------------------------------------------------------------------
# Shared variables
#-------------------------------------------------------------------------------
# do not use outer variables here (there is some exceptions)
# <<includable_vars_start>>
VERBOSE=1
LOG='output.log'
PC_LOG_FILE_PATH_SUFFIX='/.patching.log'
PC_LOG_FILE_PATH="${TARGET_DIR}${PC_LOG_FILE_PATH_SUFFIX}"
PC_HASH_SOLVER='md5sum'
PC_NEED_REWRITE_HISTORY=0
declare -a pc_patch_history

MSG_REMOVE_AND_TRY_AGAIN="You can delete '$PC_LOG_FILE_PATH' and try to patch again."
# <<includable_vars_end>>

#-------------------------------------------------------------------------------
# Constants
#-------------------------------------------------------------------------------
VERSION_SORTER_ASC='sort -V'
VERSION_SORTER_DSC='sort -rV'
getCurDir="dirname "$(readlink -f ${BASH_SOURCE[0]})""
#-------------------------------------------------------------------------------
# Shared constants
#-------------------------------------------------------------------------------
# <<global_consts_start>>
SUCCESS=0
USER_MESSAGE_ERR=253
USER_MESSAGE_WARN=254
ERR_UNKNOWN_PROBLEM=1
ERR_NOPARAMS=2
ERR_HAS_UNKNOWN_PARAMS=3
ERR_DIRECTORY_NOT_EXIST=4
ERR_FILE_NOT_EXIST=5
ERR_CORRUPTED_FILE=6
ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST=7
ERR_PATCH_CONFLICT=8
ERR_PATCH_ALREADY_APLIED=9
ERR_PATCH_NOT_APLIED=10

MSG_1_UNKNOWN_PROBLEM="The execution was interrupted for an unknown reason."
MSG_2_NOPARAMS="This operation requires at least one required parameter."
MSG_3_HAS_UNKNOWN_PARAMS="An invalid parameter was passed."
MSG_4_DIRECTORY_NOT_EXIST="This operation requires the directory to exist and not be empty: "
MSG_5_FILE_NOT_EXIST="This operation requires the file to exist: "
MSG_6_CORRUPTED_FILE="This file is damaged or has an invalid format: "
MSG_7_CANCEL_BECAUSE_HISTORY_NOT_EXIST="This operation requires a patch history file."
MSG_8_PATCH_CONFLICT="This patch conflicts with an already applied patch: "
MSG_9_PATCH_ALREADY_APLIED="According to history, this patch has already been applied."
MSG_10_PATCH_NOT_APLIED="According to history, this patch has not been applied."
# <<global_consts_end>>

# clean log
echo "" > $LOG

function usage () {
	echo $0; echo
	echo $DESCRIPTION;
	printf "
Syntax:
	$0 -h|--help
		Show this message.
		
	$0 [--stop-at-first-failure] (-a|--apply) [<mask1> ...]
		Apply patch $PATCH_NAME.
		
	$0 [--stop-at-first-failure] (-r|--rollback) [<mask1> ...]
		Rollback patch $PATCH_NAME.

Options:
	--stop-at-first-failure
		Stops the procedure on any first failure. By default,
		the script ignores any errors that occur in the process.
		The option must be entered first on the command line.

Options to show information:
	--check-patches
		Display the patches applied to the product.

	--find-backups
		Tries to find all backup files to roll back changes in
		$ROOT_DIR and shows them.
		
	--describe
		Displays the contents of this patch. The output includes
		a list of nested patches and their description.
"
}

function upvar() {
    if unset -v "$1"; then
        if (( $# == 2 )); then
            eval $1=\"\$2\"
        else
            eval $1=\(\"\${@:2}\"\)
        fi
    fi
}

function getPatchName() {
	[[ $# == 2 ]] || return
	local input=${2%/*.*}
	input=${input##*/}
	upvar ${1} ${input}
}

# <<common_start>>
if [[ -z "${COLUMNS}" ]]; then
    COLUMNS=$(stty size)
    COLUMNS=${COLUMNS##* }
fi
if [[ $COLUMNS -eq 0 ]]; then COLUMNS=80; fi
COL=$((${COLUMNS} - 15))
WCOL=$((${COL} - 2))

SET_COL="\\033[${COL}G"
SET_WCOL="\\033[${WCOL}G"
CURS_UP="\\033[1A\\033[0G"

NORMAL="\\033[0;39m"
SUCCESSS="\\033[0;32m"
WARNING="\\033[0;33m"
FAILURE="\\033[0;31m"
INFO="\\033[0;36m"
BRACKET="\\033[1;34m"
STRING_LENGTH=0

ECHO='echo'

function mesg() {
    local parm=""
    while true; do
        case "${1}" in
            -n)
                parm=" -n "
                shift 1
                ;;
            -*)
                return 1
                ;;
            *)
                break
                ;;
        esac
    done
    STRING_LENGTH=$((${#1} + 1))
    ${ECHO} $parm -e "${2}${1}"
    [[ $VERBOSE == 0 ]] || ${ECHO} $parm -e "${2}${1}" >> $LOG
}

function mesg_flush() {
    STRING_LENGTH=0
}

function echo_ok() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[${SUCCESSS} SUCCESS ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [SUCCESS]${NORMAL}" >> $LOG
}

function echo_patch() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[${SUCCESSS} PATCH   ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [PATCH]${NORMAL}" >> $LOG
}

function echo_rollback() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[${SUCCESSS} ROLLBCK ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [ROLLBCK]${NORMAL}" >> $LOG
}

function echo_info() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[ INFO    ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [INFO]${NORMAL}" >> $LOG
}

function echo_failure() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[${FAILURE} FAILURE ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [FAILURE]${NORMAL}" >> $LOG
}

function echo_warning() {
    ${ECHO} -n -e "${CURS_UP}${SET_COL}${BRACKET}[${WARNING} WARNING ${BRACKET}]"
    ${ECHO} -e "${NORMAL}"
    mesg_flush
    [[ $VERBOSE == 0 ]] || ${ECHO} -e " [WARNING]${NORMAL}" >> $LOG
}

function print_status() {
    [[ $# != 0 ]] || return 1
    case "${1}" in
        --success)
            echo_ok
            ;;
        --patch)
            ${ECHO}
            ${ECHO} -e -n "${CURS_UP}"
            ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
            mesg "${2}" ${SUCCESSS}
            [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${2}" >> $LOG
            echo_patch
            ;;
        --rollback)
            ${ECHO}
            ${ECHO} -e -n "${CURS_UP}"
            ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
            mesg "${2}" ${SUCCESSS}
            [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${2}" >> $LOG
            echo_rollback
            ;;
        -i|--info)
            ${ECHO}
            ${ECHO} -e -n "${CURS_UP}"
            ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
            mesg "${2}" ${NORMAL}
            [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${2}" >> $LOG
            echo_info
            ;;
        -w|--warning)
            case "${2}" in
                1)  
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "$MSG_1_UNKNOWN_PROBLEM" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "$MSG_1_UNKNOWN_PROBLEM" >> $LOG
                    echo_warning
                    ;;
                2)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "$MSG_2_NOPARAMS" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "$MSG_2_NOPARAMS" >> $LOG
                    echo_warning
                    ;;
                3)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "$MSG_3_HAS_UNKNOWN_PARAMS" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "$MSG_3_HAS_UNKNOWN_PARAMS" >> $LOG
                    echo_warning
                    ;;
                4)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_4_DIRECTORY_NOT_EXIST}${3}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_4_DIRECTORY_NOT_EXIST}${3}" >> $LOG
                    echo_warning
                    ;;
                5)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_5_FILE_NOT_EXIST}${3}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_5_FILE_NOT_EXIST}${3}" >> $LOG
                    echo_warning
                    ;;
                6)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_6_CORRUPTED_FILE}${3}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_6_CORRUPTED_FILE}${3}" >> $LOG
                    echo_warning
                    ;;
                7)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "$MSG_7_CANCEL_BECAUSE_HISTORY_NOT_EXIST" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "$MSG_7_CANCEL_BECAUSE_HISTORY_NOT_EXIST" >> $LOG
                    echo_warning
                    ;;
                8)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_8_PATCH_CONFLICT}${3}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_8_PATCH_CONFLICT}${3}" >> $LOG
                    echo_warning
                    ;;
                9)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_9_PATCH_ALREADY_APLIED}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_9_PATCH_ALREADY_APLIED}" >> $LOG
                    echo_warning
                    ;;
                10)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "${MSG_10_PATCH_NOT_APLIED}" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${MSG_10_PATCH_NOT_APLIED}" >> $LOG
                    echo_warning
                    ;;
                253|254)
                    ${ECHO} -e -n "${CURS_UP}"
                    ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
                    mesg "$3" ${WARNING}
                    [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "$3" >> $LOG
                    echo_warning
                    ;;
            esac
            ;;
        --failure)
            echo_failure
            ;;
        --failure-msg)
            ${ECHO}
            ${ECHO} -e -n "${CURS_UP}"
            ${ECHO} -e -n "\\033[${STRING_LENGTH}G   "
            mesg "${2}" ${FAILURE}
            [[ $VERBOSE == 0 ]] || ${ECHO} -e -n "${2}" >> $LOG
            echo_failure
            ;;
    esac
}

function say() {
	case $2 in
		-e|--error)
			${ECHO} -e "${FAILURE}ERROR:${NORMAL} $1"
            ${ECHO} -e "${FAILURE}ERROR:${NORMAL} $1" >> $LOG
		;;
		-w|--warning)
			${ECHO} -e "${WARNING}WARNING:${NORMAL} $1"
            ${ECHO} -e "${WARNING}WARNING:${NORMAL} $1" >> $LOG
		;;
		-i|--info)
			${ECHO} -e "${INFO}INFO:${NORMAL} $1"
            ${ECHO} -e "${INFO}INFO:${NORMAL} $1" >> $LOG
		;;
		*)
			printf "$1\n"
            printf "$1\n" >> $LOG
		;;
	esac
}

function die() {
	say "$1" --error 
	if [[ -z "$2" ]]; then
		exit $ERR_UNKNOWN_PROBLEM
	fi
	exit $2
}

function assert_directory_must_exist() {
    if [[ ! -d "$1" ]]; then
        die "Directory \"$1\" must exist." $ERR_DIRECTORY_NOT_EXIST
    fi
}

function assert_file_must_exist() {
    if [[ ! -f "$1" ]]; then
        die "File \"$1\" must exist." $ERR_FILE_NOT_EXIST
    fi
}
# <<common_end>>
# <<pclib_start>>
function pc_get_utc_time() {
    if [[ $# == 1 ]]; then 
        printf $(date --utc --date="$1" '+%Y-%m-%dT%H:%M:%S.%N')
    else
        printf $(date --utc '+%Y-%m-%dT%H:%M:%S.%N')
    fi
}

function pc_get_time_for_record() {
    local d=$(date --rfc-3339='ns' | sed 's/ /T/')
    printf "$d"
}

function pc_resolve_hash() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    local hash=$(sed -n '2,$ p' "$PC_LOG_FILE_PATH" | $PC_HASH_SOLVER | cut -f1 -d" ")
    printf "$hash"
    return 0
}

function pc_replace_hash() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    local hash="$(pc_resolve_hash)"
    local date="$(pc_get_time_for_record)"
    sed -i '1 s/^\(.*\),.*,\(.*\),.*$/\1,'"$hash"',\2,'"$date"'/' "$PC_LOG_FILE_PATH"
}

function pc_create_log_file() {
    local date="$(pc_get_time_for_record)"
    local first_line="0,0,$date,$date"
    echo "$first_line" > "$PC_LOG_FILE_PATH"
    return 0
}

function pc_check_log_file_integrity() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    IFS=','  read -r v1 v2 v3 v4 <<< $(sed '1 p' "$PC_LOG_FILE_PATH")
    if [[ "$v2" != "$(pc_resolve_hash)" ]]; then return 1; fi
    return 0
}

function pc_on_warn_corrupted_record() {
    local date_applied name backup_dir
    date_applied="$1"
    name="$2"
    backup_dir="$3"
    say "Found the compromised record:" -w
    echo "  Patch: $name"
    echo "  Applied: $date_applied"
    echo "  Path to the backup: $3"
    unset choice
    say "This record was probably edited manually by someone.
What to do with it?" -i
    echo "   1) Skip it. (the record won't be removed)"
    echo "   2) Restore the record and continue. (cheksum of the record will be re-calculated)"
    read -p "Choose an action (default: 1): " choice 
    case "$choice" in 
            1) return 1 ;;
            2)          ;;
            *)  say "Default is '1'" -w 
                return 1
                ;;
        esac
    return 0
}

function pc_read_log_file() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    local counter=0
    local need_restore=0
    while IFS= read -r line <&4; do
        need_restore=0
        IFS=','  read -r v1 v2 v3 v4 <<< "$line"
        if [[ $v1 != 0 ]]; then 
            # Checking checksum of a record
            local record=""
            local data=$(printf ",$v2,$v3,$v4")
            local hash=$(echo "$data" | $PC_HASH_SOLVER | cut -f1 -d" ")
            if [[ "$v1" != "$hash" ]]; then 
                if pc_on_warn_corrupted_record $v2 $v3 $v4; then need_restore=1; else continue; fi
            fi
            if [[ $need_restore -eq 1 ]]; then
                say "Restoring the record." -i
                record="$hash,$v2,$v3,$v4"
                PC_NEED_REWRITE_HISTORY=1
            else
                record="$line"
            fi
            pc_patch_history[$counter]="$record"
            counter=$((counter+1))
        fi
    done 4< "$PC_LOG_FILE_PATH"
    return 0
}

function pc_find_record_by() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    [[ $# == 2 ]] || return 1
    local key date name
    case "$1" in
        --key)  key=1   ;;
        --date) date=1  ;;
        --name) name=1  ;;
        *) ;;
    esac
    local pattern="$2"
    local result=""
    for entry in ${pc_patch_history[@]}; do
        if   [[ $key -eq 1 ]]; then
            result="$(printf $entry | grep "$pattern,.*,.*,.*")$result"
        elif [[ $date -eq 1 ]]; then
            result="$(printf $entry | grep ".*,$pattern,.*,.*")$result"
        elif [[ $name -eq 1 ]]; then
            result="$(printf $entry | grep ".*,.*,$pattern,.*")$result"
        fi
    done
    if [ ! -z "$result" -a "$result" != " "  ]; then
        return 0
    fi
    return 1
}

function pc_erase_record_by() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    [[ $# == 2 ]] || return 1
    local key date name
    case "$1" in
        --key)  key=1   ;;
        --date) date=1  ;;
        --name) name=1  ;;
        *) ;;
    esac
    local pattern="$2"
    local result=""
    local counter=0
    for entry in ${pc_patch_history[@]}; do
        result=""
        if   [[ $key -eq 1 ]]; then
            result="$(printf $entry | grep "$pattern,.*,.*,.*")"
        elif [[ $date -eq 1 ]]; then
            result="$(printf $entry | grep ".*,$pattern,.*,.*")"
        elif [[ $name -eq 1 ]]; then
            result="$(printf $entry | grep ".*,.*,$pattern,.*")"
        fi
        if [ ! -z "$result" -a "$result" != " "  ]; then
            pc_patch_history[$counter]=""
            PC_NEED_REWRITE_HISTORY=1
        fi
        counter=$((counter+1))
    done
    return 0
}

function pc_append_record() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    local record_length=3
    [[ $# == $record_length ]] || return 1
    local data=$(printf ",$1,$2,$3")
    local hash=$(echo "$data" | $PC_HASH_SOLVER | cut -f1 -d" ")
    echo $hash$data >> "$PC_LOG_FILE_PATH"
    pc_replace_hash
    return 0
}

function pc_rewrite_log_file() {
    [[ -f "$PC_LOG_FILE_PATH" ]] || return $ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST
    [[ $PC_NEED_REWRITE_HISTORY -ne 0 ]] || return 1
    say "History was changed. Rewrite '$PC_LOG_FILE_PATH'." -i
    local new_file="${PC_LOG_FILE_PATH}.new"
    sed -n '1 p' "$PC_LOG_FILE_PATH" > "$new_file"
    for entry in ${pc_patch_history[@]}; do
        echo "$entry" >> "$new_file"
    done
    cat "$new_file" > "$PC_LOG_FILE_PATH"
    pc_replace_hash
    rm -f "$new_file"
    return 0
}

function pc_print_history() {
    [[ ${#pc_patch_history[@]} -ne 0 ]] || return 0
    say "Time prints in UTC format." -i
    IFS=','  read -r v1 v2 v3 v4 <<< "${pc_patch_history[0]}"

    local test=$(printf "%s | %s | %s\n" "$(pc_get_utc_time $v2 | tr 'T-' ' /')" "$v3" "$v4")
    local headline_length=${#test}
    local head=$(printf "%-${headline_length}s" "*")
    echo "${head// /*}"
    for entry in ${pc_patch_history[@]}; do
        IFS=','  read -r v1 v2 v3 v4 <<< "$entry"
        printf "%s | %s | %s\n" "$(pc_get_utc_time $v2 | tr 'T-' ' /')" "$v3" "$v4"
    done
    echo "${head// /*}"
    return 0
}

function pc_init() {
    # Check existence of log file
    [[ $VERBOSE == 0 ]] || say "Checking the history of patching." -i
    if [[ ! -f $PC_LOG_FILE_PATH ]]; then say "Cannot find history. Creating new one ..." -w; pc_create_log_file; fi
    if [[ ! $? == 0 ]]; then say "Cannot create file." -e; return 1; fi
    # Check integrity
    local records_number=$(sed -n '2,$ { /^$/ d ; p }' "$PC_LOG_FILE_PATH" | wc -l)
    if [[ -n $records_number && $records_number -gt 0 ]]; then 
        [[ $VERBOSE == 0 ]] || say "Found $records_number record(s)." -i
        pc_check_log_file_integrity
        if [[ $? -ne 0 ]]; then
            say "   Checking the patch history file showed that it was compromised or corrupted.
            This file cannot be trusted as a reliable source. You can continue applying
            the current patch, then the fact of checking will be ignored. After applying
            the patch, the checksum will be recalculated." -w

            unset choice
            read -p "Continue (N/[y])?: " choice
            case "$choice" in 
                y|Y) ;;
                n|N) say "Operation was aborted." -w
                    say "$MSG_REMOVE_AND_TRY_AGAIN" -i
                    exit $ERR_CORRUPTED_FILE
                    ;;
                *) say "Default is 'N'" -w
                    say "$MSG_REMOVE_AND_TRY_AGAIN" -i
                    exit $ERR_CORRUPTED_FILE
                    ;;
            esac
            pc_replace_hash
        fi
        pc_read_log_file
    else [[ $VERBOSE == 0 ]] || say "There are not records in history." -i; fi
    return 0
}

function pc_post_process() {
    pc_rewrite_log_file
    return 0
}
# <<pclib_end>>

function lib_append() {
	[[ $# == 3 ]] || return 1
	local prefix='# <<'
    local start_suffix='_start>>'
    local stop_suffix='_end>>'
    sed -n '/'"$prefix""$3""$start_suffix"'/,/'"$prefix""$3""$stop_suffix"'/ p' "$1" >> "$2"
	return 0
}

function init_content() {
	local CURRENT_DIR=$($getCurDir)
	CONTENT_DIR="$CURRENT_DIR/$CONTENT_DIR"
	assert_directory_must_exist "$CONTENT_DIR"
	find "$CONTENT_DIR" -name "*.sh" -exec chmod a+x {} \;
}

function test_env() {
	init_content
	# Creating the lib
	if [[ ! -f "$LIB" ]]; then
		lib_append "$0" "$LIB" 'includable_vars'
		lib_append "$0" "$LIB" 'global_consts'
		lib_append "$0" "$LIB" 'common'
		lib_append "$0" "$LIB" 'pclib'
	fi
	# Checking Callback Assist components
	assert_directory_must_exist "$TARGET_DIR"
	assert_file_must_exist "$VERSION_FILE"
	while IFS='= ' read -r lhs rhs; do
		lhs=${lhs//[$'\r']}; rhs=${rhs//[$'\r']}
		if [[ ! $lhs =~ ^\ *# && -n $lhs ]]; then
			rhs="${rhs%%\#*}"
			rhs="${rhs%%*( )}"
			rhs="${rhs%\"*}"
			rhs="${rhs#\"*}"
			CONFIGURATION="$lhs=$rhs;$CONFIGURATION"
		fi
	done < "$VERSION_FILE"
	echo $CONFIGURATION > $STORAGE
	# Init package checker
	pc_init
}

function find_backups() {
	say "LIST OF BACKUPS"
	find $ROOT_DIR -maxdepth 1 -type d -wholename "$ROOT_DIR/${BACKUP_DIR_PREFIX}*"
}

function show_backups() {
	test_env
	find_backups
}

function describe_patch() {
	init_content
	say "$PATCH_NAME\n"
	say "$DESCRIPTION\n"
	say "CONTENTS:"
	local SUBNAME=""
	for patch in $(ls -d $CONTENT_DIR/* | $VERSION_SORTER_ASC); do
		SUBNAME=${patch##*/}
		say "    - $SUBNAME"
	done
	say "\nDETAILS:"
	for patch in $(find "$CONTENT_DIR" -type f -iname "$PATCH_README_TEMPLATE" | $VERSION_SORTER_ASC); do
		getPatchName SUBNAME $patch
		say "\n=== $SUBNAME ===\n"
		cat $patch
		echo
	done
}

function check_patches() {
	test_env
	pc_print_history
}

function known_normal() {
    [[ $# == 1 ]] || return 1
    local set_of_known_errors="
        $USER_MESSAGE_WARN
        $ERR_PATCH_CONFLICT
        $ERR_PATCH_ALREADY_APLIED
        $ERR_PATCH_NOT_APLIED
    "
    for err in ${set_of_known_errors[*]}; do
        if [[ $err -eq $1 ]]; then return 0; fi
    done
    return 1
}

function apply() {
	test_env
	local SUBNAME=""
	local rc=$SUCCESS
	local patch_list=""
    if [[ $# != 0 ]]; then 
        for entry in "$@"; do
            local pattern="*$entry/$PATCH_SH_TEMPLATE"
            patch_list="$(find "$CONTENT_DIR" -type f -wholename "$pattern") $patch_list"
        done
        patch_list=$(echo $patch_list | $VERSION_SORTER_ASC)
    else
        patch_list=$(find "$CONTENT_DIR" -type f -name "$PATCH_SH_TEMPLATE" | $VERSION_SORTER_ASC)
    fi
    if [[ ${#patch_list} == 0 ]]; then say "Nothing found to apply." -i; return 0; fi
    say "Begin applying." -i
    for patch in ${patch_list[*]}; do
		getPatchName SUBNAME $patch
		$patch --apply	--patch-name "$SUBNAME" \
						--backup-path "$ROOT_DIR/${BACKUP_DIR_PREFIX}_${PATCH_NAME}_$SUBNAME" \
						--target-path "$TARGET_DIR" \
						--patch-path "${patch%/*.*}" \
						-c "$($getCurDir)/$STORAGE"
		rc=$?
		if [[ $rc -eq 0 ]]; then
            pc_append_record "$(pc_get_time_for_record)" "$SUBNAME" "$ROOT_DIR/${BACKUP_DIR_PREFIX}_${PATCH_NAME}_$SUBNAME"
			print_status --success
        else
            if ! known_normal $rc; then
                print_status --failure-msg "Operation status"
            else
                ${ECHO}; print_status -w $USER_MESSAGE_WARN "Applying of the $SUBNAME aborted."
            fi            
		fi
		if [[ $rc -gt 0 && $STOP_AT_FIRST_FAILURE -gt 0 ]]; then
            if ! known_normal $rc; then
                pc_post_process
                die "Stopped at failure. Last code = $rc."
            fi
		fi
	done
	echo
	find_backups
}

function rollback() {
	test_env
	local SUBNAME=""
	local rc=$SUCCESS
	local patch_list=""
    if [[ $# != 0 ]]; then 
        for entry in "$@"; do
            local pattern="*$entry/$PATCH_SH_TEMPLATE"
            patch_list="$(find "$CONTENT_DIR" -type f -wholename "$pattern") $patch_list"
        done
        patch_list=$(echo $patch_list | $VERSION_SORTER_DSC)   
    else
        patch_list=$(find "$CONTENT_DIR" -type f -name "$PATCH_SH_TEMPLATE" | $VERSION_SORTER_DSC)
    fi
    if [[ ${#patch_list} == 0 ]]; then say "Nothing found to rollback." -i; return 0; fi
    say "Begin rolling back." -i
    for patch in ${patch_list[*]}; do
        getPatchName SUBNAME $patch
		$patch --rollback --patch-name "$SUBNAME" \
						  --backup-path "$ROOT_DIR/${BACKUP_DIR_PREFIX}_${PATCH_NAME}_$SUBNAME" \
						  --target-path "$TARGET_DIR" \
						  --patch-path "${patch%/*.*}" \
						  -c "$($getCurDir)/$STORAGE"
		rc=$?
		if [[ $rc -eq 0 ]]; then
			pc_erase_record_by --name "$SUBNAME"
            print_status --success
        else
            if ! known_normal $rc; then
                print_status --failure-msg "Operation status"
            else
                ${ECHO}; print_status -w $USER_MESSAGE_WARN "Operation for the $SUBNAME aborted."
            fi
		fi
		if [[ $rc -gt 0 && $STOP_AT_FIRST_FAILURE -gt 0 ]]; then
            if ! known_normal $rc; then
                pc_post_process
                die "Stopped at failure. Last code = $rc."
            fi
		fi
	done
}

function packme() {
	local TOOL='tar'
	local OPTIONS='cavf'
	$TOOL --version >/dev/null 2>&1 || { die "Need $TOOL for this action." 255; }
	local README_FILE=""
	local found=$(find . -maxdepth 1 -type f -iname $PATCH_README_TEMPLATE -print -quit)
	if [[ ! -n found ]]; then
		die "You forgot to write a main README.txt file. Aborted." 254
	else
		README_FILE=${found##*/}
	fi
	init_content
	found=$(find "$CONTENT_DIR" -type f -print -quit | wc -l)
	if [[ found -eq 0 ]]; then
		die "$CONTENT_DIR is empty or does not exist. Aborted." 253
	fi
	$TOOL $OPTIONS "CBA-${PATCH_NAME}.tar.gz" ${0##*/} $README_FILE ${CONTENT_DIR##*/}
}

#-------------------------------------------------------------------------------
# Main
#-------------------------------------------------------------------------------
# WARNING
#   Please do not correct anything below this line.
#-------------------------------------------------------------------------------

if [[ -z "$*" ]]; then
	usage
	exit $ERR_NOPARAMS
fi

while [[ -n "$1" ]]; do
	case "$1" in
		--self-pack)
			packme; break;;
		-t|--test) test_env; break ;;
		--stop-at-first-failure)
			say "\"Stop at a first failure\" feature is on." --info
			STOP_AT_FIRST_FAILURE=1
			;;
		--check-patches)
			check_patches; break ;;
		--find-backups)
			show_backups; break ;;
		--describe)
			describe_patch; break ;;
		-v|--version)
			say $PATCH_NAME; break ;;
		-h|--help)
			usage; break ;;
		-a|--apply)
			MISSES=0; shift; apply $*; break ;;
		-r|--rollback)
			MISSES=0; shift; rollback $*; break ;;
		*) 
			say "Unknown option: $1 – Skipped" --warning;  MISSES=$((MISSES+1)) ;;
	esac
    shift
done

pc_post_process

if [[ $MISSES -gt 0 ]]; then
	usage
	exit $ERR_HAS_UNKNOWN_PARAMS
fi

exit $SUCCESS

#-------------------------------------------------------------------------------
# Ending
#-------------------------------------------------------------------------------
