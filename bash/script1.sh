#!/bin/bash

######################################################################
#
# Template for a subcomponent of a complex patch.
#
# Version: 1.0
#
# Syntax:
#  To apply the patch: <script.sh> --apply "args"
#  To rollback the patch: <script.sh> --rollback "args"
#
# Supported args for --apply option:
#   -n|--patch-name <string>
#		The name of the subcomponent.
#
#	-b|--backup-path <path>
#		Path to a directory where are stored backup files for
#       rollbacking.
#
# Supported args for --rollback option:
#   -n|--patch-name <string>
#		The name of the subcomponent.
#
#	-b|--backup-path <path>
#		Path to a directory where are stored backup files for
#       rollbacking.
#
######################################################################

TARGET_DIR=""
BACKUP_DIR=""
PATCH_DIR=""
PART_NAME=""
FAILS=0

######################################################################
# Constants
######################################################################
SUCCESS=0
ERR_UNKNOWN_PROBLEM=16
ERR_DIRECTORY_NOT_EXIST=17
ERR_FILE_NOT_EXIST=18

######################################################################
declare -A configuration

# Note:
#   Input should be in format
#     "parameter11=value1;parameter12=value2; ...\nparameter21=..."
#
function append_configuration() {
	[[ $# == 1 ]] || [[ -f $1 ]] || return
	while IFS= read -r storage_line; do
		IFS=';' read -ra params <<< "$storage_line"
		for line in "${params[@]}"; do
			configuration[${line%%=*}]=${line##*=}
		done
	done < "$1"
}

function getValue() {
	[[ $# == 1 ]] || return
	printf "${configuration[$1]}"
}

function args() {
	for arg in $*; do
		case "$1" in
			-c|--append-configuration)
				shift
				append_configuration "$1"
				;;
			-b|--backup-path)
				shift
				BACKUP_DIR="$1"
				;;
			-n|--patch-name)
				shift
				PART_NAME="$1"
				;;
			-t|--target-path)
				shift
				TARGET_DIR="$1"
				;;
			-s|--patch-path)
				shift
				PATCH_DIR="$1"
				;;
			*)  ;;
		esac
		shift
	done
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
        die "Directory \"$1\" must exist" $ERR_DIRECTORY_NOT_EXIST
    fi
}

function assert_file_must_exist() {
    if [[ ! -f "$1" ]]; then
        die "File \"$1\" must exist." $ERR_FILE_NOT_EXIST
    fi
}

function apply() {
	args $*
	echo  "-> Applying $PART_NAME ..."
	## Applying process below
	say "Information"
	say "Patch name=$PART_NAME" -i
	say "Patch location=$PATCH_DIR" -i
	say "Backup directory for the patch=$BACKUP_DIR" -i
	say "Params:"
	say "release=$(getValue release)" -i
	say "build=$(getValue build)" -i
	say "buildTimestamp=$(getValue buildTimestamp)" -i
	say "deploymentType=$(getValue deploymentType)" -i
	say "component=$(getValue component)" -i
	say "installTimestamp=$(getValue installTimestamp)" -i
	say "platform=$(getValue platform)" -i
}

function rollback() {
	args $*
	echo  "-> Rollback for $PART_NAME ... "
	## Rollbacking process below
	say "Information"
	say "Patch name=$PART_NAME" -i
	say "Patch location=$PATCH_DIR" -i
	say "Backup directory for the patch=$BACKUP_DIR" -i
	say "Params:"
	say "release=$(getValue release)" -i
	say "build=$(getValue build)" -i
	say "buildTimestamp=$(getValue buildTimestamp)" -i
	say "deploymentType=$(getValue deploymentType)" -i
	say "component=$(getValue component)" -i
	say "installTimestamp=$(getValue installTimestamp)" -i
	say "platform=$(getValue platform)" -i
}

case $1 in
	-a|--apply) shift; apply $*       ;;
	-r|--rollback) shift; rollback $* ;;
	*) ;;
esac

if [[ $FAILS -gt 0 ]]; then
	echo "<- failed"
	exit $FAILS
else
	echo "<- done"
fi

exit $SUCCESS
