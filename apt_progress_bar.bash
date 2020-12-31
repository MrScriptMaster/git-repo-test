#!/bin/bash

ECHO='echo'

#consts
declare -r GPB_CODE_SAVE_CURSOR="\033[s"
declare -r GPB_CODE_RESTORE_CURSOR="\033[u"
declare -r GPB_CODE_CURSOR_IN_SCROLL_AREA="\033[1A"
declare -r GPB_COLOR_FG="\e[30m"
declare -r GPB_COLOR_BG="\e[42m"
declare -r GPB_COLOR_BG_BLOCKED="\e[43m"
declare -r GPB_RESTORE_FG="\e[39m"
declare -r GPB_RESTORE_BG="\e[49m"
#vars
GPB_IS_PROGRESS_BLOCKED="false"
GPB_IS_TRAPPING_ENABLED="false"
GPB_IS_TRAP_SET="false"

function gpb_setup_scroll_area() {
  if [[ "$GPB_IS_TRAPPING_ENABLED" == "true" ]]; then
      gpb_trap_on_interrupt
  fi
  local lines=$(tput lines)
  let lines=$lines-1
  ${ECHO} -en "\n"
  ${ECHO} -en "$GPB_CODE_SAVE_CURSOR"
  ${ECHO} -en "\033[0;${lines}r"
  ${ECHO} -en "$GPB_CODE_RESTORE_CURSOR"
  ${ECHO} -en "$GPB_CODE_CURSOR_IN_SCROLL_AREA"
  gpb_draw_progress_bar 0
}

function gpb_destroy_scroll_area() {
  lines=$(tput lines)
  ${ECHO} -en "$GPB_CODE_SAVE_CURSOR"
  ${ECHO} -en "\033[0;${lines}r"
  ${ECHO} -en "$GPB_CODE_RESTORE_CURSOR"
  ${ECHO} -en "$GPB_CODE_CURSOR_IN_SCROLL_AREA"
  gpb_clear_progress_bar
  ${ECHO} -en "\n\n"
  if [[ "$GPB_IS_TRAP_SET" == "true" ]]; then
      trap - INT
  fi
}

function gpb_draw_progress_bar() {
  local percentage=$1
  local lines=$(tput lines)
  let lines=$lines
  ${ECHO} -en "$GPB_CODE_SAVE_CURSOR"
  ${ECHO} -en "\033[${lines};0f"
  tput el
  GPB_IS_PROGRESS_BLOCKED="false"
  gpb_print_bar_text $percentage
  ${ECHO} -en "$GPB_CODE_RESTORE_CURSOR"
}

function gpb_block_progress_bar() {
  local percentage=$1
  local lines=$(tput lines)
  let lines=$lines
  ${ECHO} -en "$GPB_CODE_SAVE_CURSOR"
  ${ECHO} -en "\033[${lines};0f"
  tput el
  GPB_IS_PROGRESS_BLOCKED="true"
  gpb_print_bar_text $percentage
  ${ECHO} -en "$GPB_CODE_RESTORE_CURSOR"
}

function gpb_clear_progress_bar() {
  local lines=$(tput lines)
  let lines=$lines
  ${ECHO} -en "$GPB_CODE_SAVE_CURSOR"
  ${ECHO} -en "\033[${lines};0f"
  tput el
  ${ECHO} -en "$GPB_CODE_RESTORE_CURSOR"
}

function gpb_print_bar_text() {
  local percentage=$1
  local cols=$(tput cols)
  let bar_size=$cols-17
  local color="${GPB_COLOR_FG}${GPB_COLOR_BG}"
  if [[ "$GPB_IS_PROGRESS_BLOCKED" == "true" ]]; then
      color="${GPB_COLOR_FG}${GPB_COLOR_BG_BLOCKED}"
  fi
	let complete_size=($bar_size*$percentage)/100
	let remainder_size=$bar_size-$complete_size
  local progress_bar=$(${ECHO} -ne "["; ${ECHO} -en "${color}"; gpb_printf_new "#" $complete_size; \
      ${ECHO} -en "${GPB_RESTORE_FG}${GPB_RESTORE_BG}"; gpb_printf_new "." $remainder_size; ${ECHO} -ne "]");
  ${ECHO} -ne " Progress ${percentage}% ${progress_bar}"
}

function gpb_enable_trapping() {
  GPB_IS_TRAPPING_ENABLED="true"
}

function gpb_trap_on_interrupt() {
  GPB_IS_TRAP_SET="true"
  trap gpb_cleanup_on_interrupt INT
}

function gpb_cleanup_on_interrupt() {
  gpb_destroy_scroll_area
  exit
}

function gpb_printf_new() {
  local str=$1
  local num=$2
  [[ ${num} -eq 0 ]] && return;
  local v=$(printf "%-${num}s" "$str")
  ${ECHO} -ne "${v// /$str}"
}


gpb_enable_trapping
gpb_setup_scroll_area
gpb_draw_progress_bar 0
sleep 2
gpb_draw_progress_bar 10
echo hello
sleep 1
gpb_draw_progress_bar 40
sleep 1
echo hello
gpb_draw_progress_bar 60
sleep 1
echo hello
gpb_draw_progress_bar 90
sleep 1
echo hello
echo hello
echo hello
echo hello
echo hello
echo hello
echo hello
echo hello
gpb_draw_progress_bar 100
sleep 5
gpb_destroy_scroll_area
