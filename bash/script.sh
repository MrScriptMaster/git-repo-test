#!/bin/bash

###############################################################
# Script version: 1.0
###############################################################

PATCH_NAME="enter-patch-version"
CBA_VERSION="5.x.x.x"
DESCRIPTION="The script apply $PATCH_NAME for Callback Assist $CBA_VERSION."
CONTENT_DIR="content"
ROOT_DIR='/opt/Avaya'
TARGET_DIR="$ROOT_DIR/callbackassist"
VERSION_FILE="$ROOT_DIR/callbackassist/version.txt"
PATCH_SH_TEMPLATE='patch.sh'
PATCH_README_TEMPLATE='readme.txt'
STORAGE='storage.data'
BACKUP_DIR_PREFIX='backup'
CONFIGURATION=""
STOP_AT_FIRST_FAILURE=0
MISSES=0

###############################################################
# Constants
###############################################################
VERSION_SORTER_ASC='sort -V'
VERSION_SORTER_DSC='sort -rV'
getCurDir="dirname "$(readlink -f ${BASH_SOURCE[0]})""
SUCCESS=0
ERR_NOPARAMS=1
ERR_HAS_UNKNOWN_PARAMS=2
ERR_UNKNOWN_PROBLEM=3
ERR_DIRECTORY_NOT_EXIST=4
ERR_FILE_NOT_EXIST=5

function usage () {
	echo $0; echo
	echo $DESCRIPTION;
	printf "
Syntax:
	$0 -h|--help
		Show this message.
		
	$0 [--stop-at-first-failure] (-a|--apply)
		Apply patch $PATCH_NAME.
		
	$0 [--stop-at-first-failure] (-r|--rollback)
		Rollback patch $PATCH_NAME.

Options:
	--stop-at-first-failure
		Stops the procedure on any first failure. By default,
		the script ignores any errors that occur in the process.
		The option must be entered first on the command line.

Options to show information:
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

function init_content() {
	local CURRENT_DIR=$($getCurDir)
	CONTENT_DIR="$CURRENT_DIR/$CONTENT_DIR"
	assert_directory_must_exist "$CONTENT_DIR"
	find "$CONTENT_DIR" -name "*.sh" -exec chmod +x {} \;
}

function test_env() {
	init_content
	#assert_directory_must_exist "$TARGET_DIR"
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

function apply() {
	test_env
	local SUBNAME=""
	local rc=$SUCCESS
	for patch in $(find "$CONTENT_DIR" -type f -name "$PATCH_SH_TEMPLATE" | $VERSION_SORTER_ASC); do
		getPatchName SUBNAME $patch
		$patch --apply	--patch-name "$SUBNAME" \
						--backup-path "$ROOT_DIR/${BACKUP_DIR_PREFIX}_${PATCH_NAME}_$SUBNAME" \
						--target-path "$TARGET_DIR" \
						--patch-path "${patch%/*.*}" \
						-c "$($getCurDir)/$STORAGE"
		rc=$?
		if [[ $rc -gt 0 && $STOP_AT_FIRST_FAILURE -gt 0 ]]; then
			die "Stopped at failure. Last code = $rc."
		fi
	done
	echo
	find_backups
}

function rollback() {
	test_env
	local SUBNAME=""
	local rc=$SUCCESS
	for patch in $(find "$CONTENT_DIR" -type f -name "$PATCH_SH_TEMPLATE" | $VERSION_SORTER_DSC); do
		getPatchName SUBNAME $patch
		$patch --rollback --patch-name "$SUBNAME" \
						  --backup-path "$ROOT_DIR/${BACKUP_DIR_PREFIX}_${PATCH_NAME}_$SUBNAME" \
						  --target-path "$TARGET_DIR" \
						  --patch-path "${patch%/*.*}" \
						  -c "$($getCurDir)/$STORAGE"
		rc=$?
		if [[ $rc -gt 0 && $STOP_AT_FIRST_FAILURE -gt 0 ]]; then
			die "Stopped at a failure. Last code = $rc."
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

###############################################################
# Main
###############################################################

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
		--find-backups)
			show_backups; break ;;
		--describe)
			describe_patch; break ;;
		-v|--version)
			say $PATCH_NAME; break ;;
		-h|--help)
			usage; break ;;
		-a|--apply)
			MISSES=0; apply; break ;;
		-r|--rollback)
			MISSES=0; rollback; break ;;
		*) 
			say "Unknown option: $1 – Skipped" --warning;  MISSES=$((MISSES+1)) ;;
	esac
    shift
done

if [[ $MISSES -gt 0 ]]; then
	usage
	exit $ERR_HAS_UNKNOWN_PARAMS
fi

exit $SUCCESS
