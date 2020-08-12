#!/bin/bash

PATCH_NAME="enter-patch-version"
CBA_VERSION="5.x.x.x"
DESCRIPTION="The script apply $PATCH_NAME for $CBA_VERSION."
MISSES=0

###############################################################
# Constants
###############################################################
ERR_NOPARAMS=1
ERR_HAS_MISSES=2

function usage () {
	echo $0; echo
	echo $DESCRIPTION;
printf "
Syntax:
	$0 -h|--help
		Show this message.
		
	$0 -a|--apply
		Apply patch $PATCH_NAME.
		
	$0 -r|--rollback
		Rollback patch $PATCH_NAME.
"
}

function test_env() {
	echo "test_environment"
}

function apply() {
	test_env
}

function rollback() {
	test_env
}

###############################################################
# Main
###############################################################

if [[ -z "$*" ]]; then
	usage
	exit $ERR_NOPARAMS
fi

while [[ -n "$1" ]]; do
	case $1 in
		-v|--version)
			echo $PATCH_NAME; break ;;
		-h|--help)
			usage; break ;;
		-a|--apply)
			MISSES=0; apply; break ;;
		-r|--rollback)
			MISSES=0; rollback; break ;;
		*) 
			echo "Unknown option: $1 – Skipped";  MISSES=$((MISSES+1)) ;;
	esac
    shift
done

if [[ $MISSES -gt 0 ]]; then
	usage
	exit $ERR_HAS_MISSES
fi

exit 0
