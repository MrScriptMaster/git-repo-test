#!/bin/bash

function usage() {
    printf "$0 (--key <string>) (--value <string>|--file <path>) /path/to/out-file ...
    Description:
        Replaces all '--key' lines in the output file (out-file) with
        the '--value line, or the contents of the '--file'.
    
    Flags:
        --key <string>
            The string in the target file to be replaced.
        
        --value <string>
            The replacement string.

        --file </path/to/file>
            The file, the contents of which will be taken for replacement.

    Args:
        /path/to/out-file ...
            The file(s) to replace.
"
    case "$1" in
        --exit)
            shift
            exit $1
            ;;
    esac
}

function say() {
    case $2 in
        -e|--error)
            printf "ERROR: $1\n"
        ;;
        -w|--warning)
            printf "WARNING: $1\n"
        ;;
        -i|--info)
            printf "INFO: $1\n"
        ;;
        *)
            printf "$1\n"
        ;;
    esac
}

__FROM_FILE=false
__VERBOSE=false

# --- Process options ---
while [[ -n "$1" ]]; do
    case "$1" in
        -v|--verbose)
            __VERBOSE=true
            ;;
        --key)
            shift
            KEY="$1"
            ;;
        --value)
            shift
            SOURCE="$1"
            ;;
        --file)
            shift
            SOURCE="$1"
            __FROM_FILE=true
            ;;
        *) break ;;
    esac
    shift
done
# --- Some validations ---
[[ ! -z $KEY ]] && [[ ! -z $SOURCE ]] || usage --exit 1
[[ $__VERBOSE == false ]] || say "key=$KEY" -i
[[ $__VERBOSE == false ]] || say "source=$SOURCE" -i
[[ $__VERBOSE == false ]] || say "is_source_path=$__FROM_FILE" -i

if [[ $__FROM_FILE == true && ! -f "$SOURCE" ]]; then
    say "\"$SOURCE\" is not exist. Operation aborted." -e
    exit 1
fi

# --- Start processing ---
for file in $@; do
    [[ $__VERBOSE == false ]] || say "Processing $file ..." -i
    if [[ ! -f $file ]]; then
        say "\"$file\" is not exist. Skipped." -w
        continue
    fi
    if [[ $__FROM_FILE == false ]]; then
        [[ $__VERBOSE == false ]] || say "\"$KEY\" -> \"$SOURCE\" in \"$file\"" -i
        sed -i -e 's/'"$KEY"'/'"$SOURCE"'/' "$file"        
    else    
        [[ $__VERBOSE == false ]] || say "\"$KEY\" -> 'content of \"$SOURCE\"' in \"$file\"" -i
        sed -i -e '\_'"$KEY"'_ {
            r '"$SOURCE"'
            d
        }' "$file"
    fi
done

exit 0
# --- End of the script ---
