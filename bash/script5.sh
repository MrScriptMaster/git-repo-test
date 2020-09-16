#!/bin/bash
# !!! IMPORTANT !!!                                                            #
# DO NOT EDIT THIS SCRIPT IF YOU ARE NOT SURE OF WHAT A PARTICULAR PART OF IT  #
# DOES                                                                         #
################################################################################
# WARNING                                                                      #
#                                                                              #
#  - DO NOT FILL any declared variables and its names manually without         #
#    agreement.                                                                #
#  - DO NOT CHANGE constant's values without agreement.                        #
#  - DO NOT CHANGE input keys of this script.                                  #
#                                                                              #
# Violation of the conventions may lead to loss of compatibility with          #
# the script calling this script.                                              #
#                                                                              #
################################################################################

TARGET_DIR=""
BACKUP_DIR=""
PATCH_DIR=""
PART_NAME=""
MESSAGE_CODE=0
MESSAGE_ADDITIONAL_STRING=""

if [[ -f '.toolkit.include' ]]; then
	source '.toolkit.include'
	if [[ ! -z $VERBOSE ]]; then VERBOSE=0; fi
else
	echo "Fatal error: Oops, you shouldn't have seen this message. \
You are doing something wrong."
	exit 250
fi

################################################################################
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
				PC_LOG_FILE_PATH="${TARGET_DIR}${PC_LOG_FILE_PATH_SUFFIX}"
				pc_init
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

# Usage:
# 	print [-i|-w <code>] "Your Message"
function print() {
	[[ $# != 0 ]] || return 0
	case "$1" in
		-i)
			print_status -i "$2"
			;;
		-w) 
			${ECHO}; print_status -w "$2" "$3"
			;;
		*)
			print_status -i "$1"
			;;
	esac
}

# args:
#  $1   Message code. Use 0 if it is not error.
#  $2   Your message.
function setWarningMessage() {
	[[ $# != 0 ]] || return 0
	[[ -z "$1" ]] || MESSAGE_CODE=$1
	[[ -z "$2" ]] || MESSAGE_ADDITIONAL_STRING="$2"
}

function basic_checks_for_apply() {
	function has_been_applied() {
		local my_name=${PART_NAME}
		if pc_find_record_by --name "$my_name"; then 
			MESSAGE_ADDITIONAL_STRING=""
			return 0;
		fi
		return 1
	}
	# Note: Add conflicts if you need.
	function have_conflicts() {
		# List all the patches that the current one conflicts with.
		# The separator can be a space or a line break.
		# Examples:
		#  0.0.0.1-Patch1 - valid
		#  .*Patch1       - valid
		#  .*Patch[1-3]   - valid
		#  .*Patch[1456]  - valid
		local conflicts="
			
		"
		# DO NOT EDIT code below this line
		for entry in ${conflicts[*]}; do
			local concrete=$entry
			if pc_find_record_by --name "$entry"; then
				MESSAGE_ADDITIONAL_STRING="$concrete. Full list:"
				for entr in ${conflicts[*]}; do
					MESSAGE_ADDITIONAL_STRING="${MESSAGE_ADDITIONAL_STRING} $entr"
				done
				return 0
			fi
		done
		return 1
	}
	#------------------------------------------------------------------------------------
	if has_been_applied; then setWarningMessage ${ERR_PATCH_ALREADY_APLIED}; return 1; fi
	if have_conflicts; then setWarningMessage ${ERR_PATCH_CONFLICT}; return 1; fi
	return 0
}

function basic_checks_for_rollback() {
	function has_not_been_applied() {
		local my_name=${PART_NAME}
		if ! pc_find_record_by --name "$my_name"; then 
			MESSAGE_ADDITIONAL_STRING=""
			return 0;
		fi
		return 1
	}
	function not_exist_backup() {
		MESSAGE_ADDITIONAL_STRING=""
		[[ -d ${BACKUP_DIR} && $(ls -A "${BACKUP_DIR}" | wc -l) != 0 ]] || return 0
		return 1
	}
	#------------------------------------------------------------------------------------
	if has_not_been_applied; then setWarningMessage ${ERR_PATCH_NOT_APLIED}; return 1; fi
	if not_exist_backup; then setWarningMessage ${ERR_DIRECTORY_NOT_EXIST} "$BACKUP_DIR"; return 1; fi
	return 0
}

function debug() {
	[[ ! -z $VERBOSE && $VERBOSE == 1 ]] || return
	print "Patch name=$PART_NAME"
	print "Patch location=$PATCH_DIR"
	print "Backup directory for the patch=$BACKUP_DIR"
	print "release=$(getValue release)"
	print "build=$(getValue build)"
	print "buildTimestamp=$(getValue buildTimestamp)"
	print "deploymentType=$(getValue deploymentType)"
	print "component=$(getValue component)"
	print "installTimestamp=$(getValue installTimestamp)"
	print "platform=$(getValue platform)"
}

#########################################################################################

function apply() {
	#VERBOSE=1
	args $*; print_status --patch "Starting to apply the patch $PART_NAME ..."; debug
	if ! basic_checks_for_apply; then return; fi
	## Note: Write your code below this line.
	########## Applying process below ##########
	#
	# .... Write your code here ....
	#
}

function rollback() {
	#VERBOSE=1
	args $*; print_status --rollback "Starting to rolling back the patch $PART_NAME ..."; debug 
	if ! basic_checks_for_rollback; then return; fi
	## Note: Write your code below this line.
	########## Rollbacking process below ##########
	#
	# .... Write your code here ....
	#
}

#-------------------------------------------------------------------------------
# Main
#-------------------------------------------------------------------------------
# WARNING
#   Please do not correct anything below this line.
#-------------------------------------------------------------------------------

case $1 in
	-a|--apply) shift; apply $*       ;;
	-r|--rollback) shift; rollback $* ;;
	*) ;;
esac

if [[ ${MESSAGE_CODE} -gt 0 ]]; then
	${ECHO}; print_status --warning $MESSAGE_CODE "$MESSAGE_ADDITIONAL_STRING"
	exit $MESSAGE_CODE
fi

exit $SUCCESS
#-------------------------------------------------------------------------------
# Ending
#-------------------------------------------------------------------------------
# (AFTER READING YOU CAN DELETE THIS PART)
#
# Template for a subcomponent of a complex patch.
#
# Version: 2.0
#
# Instructions:
#   1. You need to program the apply() and rollback() functions (see
#      them at the bottom of the file) that take into account the
#      specifics of applying and rolling back, respectively, for this
#      subpatch.
#   2. This script is a template and does nothing by itself. You can
#      supplement it with the necessary code required for
#      the procedures.
#   3. You can use the functions and variables that this script
#      introduces. The script ensures that all the variables it enters
#      will be initialized before calling apply() or rollback(). Please
#      do not change their values, just copy.
#   4. To distribute the configuration between patches, a centralized
#      storage is used, which is read by this script. The script saves
#      all values from the storage in the associative array
#      'configuration'. If you know the key, then use the getValue()
#      function to request a value from the array.
#
#      Example:
#      
#      component=$(getValue component) # take the component name from
#                                      # the storage
#      Note: see examples below in the apply() and rollback()
#            functions.
#   5. Do not modify the existing part of the script code, as this
#      script will be called in automated mode.
#   6. This script should work regardless of its position in the file
#      system. Therefore, for copy/move/delete operations, use
#      the following prefixes to form absolute paths:
#        * $PATCH_DIR stores the absolute path of the directory where
#          this script locates.
#        * $TARGET_DIR stores the absolute path of the directory where
#          the patchable product locates.
#        * $BACKUP_DIR stores the absolute path of the directory where
#          backup files locate.
#
# Syntax:
#  To apply the patch: <script.sh> --apply "args"
#  To rollback the patch: <script.sh> --rollback "args"
#
# Supported args for --apply and --rollback options:
#   -n|--patch-name <string>
#		The name of the subcomponent.
#
#	-b|--backup-path <path>
#		Path (absolute) to a directory where are stored backup files for
#       rollbacking.
#
#	-t|--target-path <path>
#		Path (absolute) to the product root directory.
#
#	-s|--patch-path <path>
#		Path (absolute) to the directory of current patch.
#
#	-c|--append-configuration <file path>
#		Path to the configuration storage that is used by this script
#		for taking technological information. In particular, to obtain
#       the characteristics of the product being patched.
#
################################################################################
