#!/bin/bash

function say() {
	case $2 in
		-e|--error)
			printf "Error: $1\n"
		;;
		-w|--warning)
			printf "Warning: $1\n"
		;;
		-i|--info)
			printf "Info: $1\n"
		;;
		*)
			printf "$1\n"
		;;
	esac
}

# pclib_start
# Patch checker variables
PC_LOG_FILE_PATH='storage.log'
PC_HASH_SOLVER='md5sum'
PC_NEED_REWRITE_HISTORY=0

MSG_REMOVE_AND_TRY_AGAIN="You can delete '$PC_LOG_FILE_PATH' and try to patch again."

ERR_CANCEL_BECAUSE_HISTORY_NOT_EXIST=16
ERR_CORRUPTED_FILE=8

declare -a pc_patch_history

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

## args:
##   $1   column
##   $2   value
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

## args:
##   $1   column
##   $2   value
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

## args:
##   $1   Applied date
##   $2   Name of the patch
##   $3   Backup files
## Example:
##   pc_append_record "$(pc_get_time_for_record)" 0.0.0.1-Patch1 /path/to/backup
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
    say "History was changed. Rewrite \'$PC_LOG_FILE_PATH\'." -i
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
    say "Checking the history of patching." -i
    if [[ ! -f $PC_LOG_FILE_PATH ]]; then say "Cannot find history. Creating new one ..." -w; pc_create_log_file; fi
    if [[ ! $? == 0 ]]; then say "Cannot create file." -e; return 1; fi
    # Check integrity
    local records_number=$(sed -n '2,$ { /^$/ d ; p }' "$PC_LOG_FILE_PATH" | wc -l)
    if [[ -n $records_number && $records_number -gt 0 ]]; then 
        say "Found $records_number record(s)." -i
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
    else say "There are not records in history." -i; fi
    return 0
}

function pc_post_process() {
    pc_rewrite_log_file
    return 0
}
# pclib_end

#pc_append_record "$(pc_get_time_for_record)" 0.0.0.1-Patch3 /path/to/backup
pc_init
#pc_erase_record_by --name '.*Patch3'
pc_print_history
pc_post_process

exit 0
