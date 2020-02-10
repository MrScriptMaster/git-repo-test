################################################################################
## progress-bar_v1 ${duration-in-secs}
##
## Preview:
##    ▇▇▇▇▇| 100%
##
################################################################################
progress-bar_v1() {
  local duration=${1}
  
  already_done() { for ((done=0; done<$elapsed; done++)); do printf "▇"; done }
  remaining() { for ((remain=$elapsed; remain<$duration; remain++)); do printf " "; done }
  percentage() { printf "| %s%%" $(( (($elapsed)*100)/($duration)*100/100 )); }
  clean_line() { printf "\r"; }

  for (( elapsed=1; elapsed<=$duration; elapsed++ )); do
      already_done; remaining; percentage
      sleep 1
      clean_line
  done
  clean_line
}; #progress-bar_v1 5

################################################################################
## progress-bar_v2 ${current-state} ${total-state}
## 
## Example:
##   Progress : [########################################] 100%
##
################################################################################
progress-bar_v2() {
    let _progress=(${1}*100/${2}*100)/100
    let _done=(${_progress}*4)/10
    let _left=40-$_done

    _fill=$(printf "%${_done}s")
    _empty=$(printf "%${_left}s")
	printf "\rProgress : [${_fill// /#}${_empty// /-}] ${_progress}%%"
};
demo2() {
	__start=1;
	__end=100;
	for number in $(seq ${__start} ${__end})
	do
		sleep 0.1
		progress-bar_v2 ${number} ${__end}
	done
	echo
	echo "Completed"
}; #demo2
################################################################################
## progress-bar_v3 ${duration}
##
## Example:
##   ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇| 100%
##
################################################################################
progress-bar_v3() {
	local duration=${1}
	local barsize=$((`tput cols` - 7))
	local unity=$(($barsize / $duration))
	local increment=$(($barsize%$duration))
	local skip=$(($duration/($duration-$increment)))
	local curr_bar=0
	local prev_bar=
	for (( elapsed=1; elapsed<=$duration; elapsed++ ))
	do
	  # Elapsed
	  prev_bar=$curr_bar
	  let curr_bar+=$unity
	  [[ $increment -eq 0 ]] || {  
	    [[ $skip -eq 1 ]] &&
	      { [[ $(($elapsed%($duration/$increment))) -eq 0 ]] && let curr_bar++; } ||
		{ [[ $(($elapsed%$skip)) -ne 0 ]] && let curr_bar++; }
	  }
	  [[ $elapsed -eq 1 && $increment -eq 1 && $skip -ne 1 ]] && let curr_bar++
	  [[ $(($barsize-$curr_bar)) -eq 1 ]] && let curr_bar++
	  [[ $curr_bar -lt $barsize ]] || curr_bar=$barsize
	  for (( filled=0; filled<=$curr_bar; filled++ )); do
	    printf "▇"
	  done

	  # Remaining
	  for (( remain=$curr_bar; remain<$barsize; remain++ )); do
	    printf " "
	  done

	  # Percentage
	  printf "| %s%%" $(( ($elapsed*100)/$duration))

	  # Return
	  sleep 1
	  printf "\r"
	done
	printf "\n"
}; #progress-bar_v3 5

################################################################################
## progress-bar_v4 ${new_value_to_display}
##
##  Example:
##    0014 sec  ################################################# 100%
##
################################################################################
progress-bar_v4() {
	local LR='\033[1;31m'
    local LG='\033[1;32m'
    local LY='\033[1;33m'
    local LC='\033[1;36m'
    local LW='\033[1;37m'
    local NC='\033[0m'
    if [ "${1}" = "0" ]; then TME=$(date +"%s"); fi
    local SEC=`printf "%04d\n" $(($(date +"%s")-${TME}))`; SEC="$SEC sec"
    local PRC=`printf "%.0f" ${1}`
    local SHW=`printf "%3d\n" ${PRC}`
    local LNE=`printf "%.0f" $((${PRC}/2))`
    local LRR=`printf "%.0f" $((${PRC}/2-12))`; if [ ${LRR} -le 0 ]; then LRR=0; fi;
    local LYY=`printf "%.0f" $((${PRC}/2-24))`; if [ ${LYY} -le 0 ]; then LYY=0; fi;
    local LCC=`printf "%.0f" $((${PRC}/2-36))`; if [ ${LCC} -le 0 ]; then LCC=0; fi;
    local LGG=`printf "%.0f" $((${PRC}/2-48))`; if [ ${LGG} -le 0 ]; then LGG=0; fi;
    local LRR_=""
    local LYY_=""
    local LCC_=""
    local LGG_=""
    for ((i=1;i<=13;i++))
    do
    	DOTS=""; for ((ii=${i};ii<13;ii++)); do DOTS="${DOTS}."; done
    	if [ ${i} -le ${LNE} ]; then LRR_="${LRR_}#"; else LRR_="${LRR_}."; fi
    	echo -ne "  ${LW}${SEC}  ${LR}${LRR_}${DOTS}${LY}............${LC}............${LG}............ ${SHW}%${NC}\r"
    	if [ ${LNE} -ge 1 ]; then sleep .05; fi
    done
    for ((i=14;i<=25;i++))
    do
    	DOTS=""; for ((ii=${i};ii<25;ii++)); do DOTS="${DOTS}."; done
    	if [ ${i} -le ${LNE} ]; then LYY_="${LYY_}#"; else LYY_="${LYY_}."; fi
    	echo -ne "  ${LW}${SEC}  ${LR}${LRR_}${LY}${LYY_}${DOTS}${LC}............${LG}............ ${SHW}%${NC}\r"
    	if [ ${LNE} -ge 14 ]; then sleep .05; fi
    done
    for ((i=26;i<=37;i++))
    do
    	DOTS=""; for ((ii=${i};ii<37;ii++)); do DOTS="${DOTS}."; done
    	if [ ${i} -le ${LNE} ]; then LCC_="${LCC_}#"; else LCC_="${LCC_}."; fi
    	echo -ne "  ${LW}${SEC}  ${LR}${LRR_}${LY}${LYY_}${LC}${LCC_}${DOTS}${LG}............ ${SHW}%${NC}\r"
    	if [ ${LNE} -ge 26 ]; then sleep .05; fi
    done
    for ((i=38;i<=49;i++))
    do
    	DOTS=""; for ((ii=${i};ii<49;ii++)); do DOTS="${DOTS}."; done
    	if [ ${i} -le ${LNE} ]; then LGG_="${LGG_}#"; else LGG_="${LGG_}."; fi
    	echo -ne "  ${LW}${SEC}  ${LR}${LRR_}${LY}${LYY_}${LC}${LCC_}${LG}${LGG_}${DOTS} ${SHW}%${NC}\r"
    	if [ ${LNE} -ge 38 ]; then sleep .05; fi
    done
}
demo3() {
	progress-bar_v4 0
	sleep 1
	progress-bar_v4 10
	sleep 1
	progress-bar_v4 20
	sleep 1
	progress-bar_v4 30
	sleep 1
	progress-bar_v4 80
	sleep 1
	progress-bar_v4 90
	sleep 1
	progress-bar_v4 100
	echo
}; #demo3

################################################################################
## progress-bar_v5
##
## APT-style progress bar
##
## Example:
##   Progress 90% [#####################################.........]
##
################################################################################
demo4() {
	#consts
	local CODE_SAVE_CURSOR="\033[s"
	local CODE_RESTORE_CURSOR="\033[u"
	local CODE_CURSOR_IN_SCROLL_AREA="\033[1A"
	local COLOR_FG="\e[30m"
	local COLOR_BG="\e[42m"
	local COLOR_BG_BLOCKED="\e[43m"
	local RESTORE_FG="\e[39m"
	local RESTORE_BG="\e[49m"
	#vars
	local PROGRESS_BLOCKED="false"
    local TRAPPING_ENABLED="false"
    local TRAP_SET="false"
	setup_scroll_area() {
    	# If trapping is enabled, we will want to activate it whenever we setup the scroll area and remove it when we break the scroll area
    	if [ "$TRAPPING_ENABLED" = "true" ]; then
    	    trap_on_interrupt
    	fi
    	lines=$(tput lines)
    	let lines=$lines-1
    	# Scroll down a bit to avoid visual glitch when the screen area shrinks by one row
    	echo -en "\n"
    	# Save cursor
    	echo -en "$CODE_SAVE_CURSOR"
    	# Set scroll region (this will place the cursor in the top left)
    	echo -en "\033[0;${lines}r"
    	# Restore cursor but ensure its inside the scrolling area
    	echo -en "$CODE_RESTORE_CURSOR"
    	echo -en "$CODE_CURSOR_IN_SCROLL_AREA"
    	# Start empty progress bar
    	draw_progress_bar 0
	}
	destroy_scroll_area() {
    	lines=$(tput lines)
    	# Save cursor
    	echo -en "$CODE_SAVE_CURSOR"
    	# Set scroll region (this will place the cursor in the top left)
    	echo -en "\033[0;${lines}r"
    	# Restore cursor but ensure its inside the scrolling area
    	echo -en "$CODE_RESTORE_CURSOR"
    	echo -en "$CODE_CURSOR_IN_SCROLL_AREA"
    	# We are done so clear the scroll bar
    	clear_progress_bar
    	# Scroll down a bit to avoid visual glitch when the screen area grows by one row
    	echo -en "\n\n"
    	# Once the scroll area is cleared, we want to remove any trap previously set. Otherwise, ctrl+c will exit our shell
    	if [ "$TRAP_SET" = "true" ]; then
    	    trap - INT
    	fi
	}
	draw_progress_bar() {
    	percentage=$1
    	lines=$(tput lines)
    	let lines=$lines
    	# Save cursor
    	echo -en "$CODE_SAVE_CURSOR"
    	# Move cursor position to last row
    	echo -en "\033[${lines};0f"
    	# Clear progress bar
    	tput el
    	# Draw progress bar
    	PROGRESS_BLOCKED="false"
    	print_bar_text $percentage
    	# Restore cursor position
    	echo -en "$CODE_RESTORE_CURSOR"
	}
	block_progress_bar() {
    	percentage=$1
    	lines=$(tput lines)
    	let lines=$lines
    	# Save cursor
    	echo -en "$CODE_SAVE_CURSOR"
    	# Move cursor position to last row
    	echo -en "\033[${lines};0f"
    	# Clear progress bar
    	tput el
    	# Draw progress bar
    	PROGRESS_BLOCKED="true"
    	print_bar_text $percentage
    	# Restore cursor position
    	echo -en "$CODE_RESTORE_CURSOR"
	}
	clear_progress_bar() {
    	lines=$(tput lines)
    	let lines=$lines
    	# Save cursor
    	echo -en "$CODE_SAVE_CURSOR"
    	# Move cursor position to last row
    	echo -en "\033[${lines};0f"
    	# clear progress bar
    	tput el
    	# Restore cursor position
    	echo -en "$CODE_RESTORE_CURSOR"
	}
	print_bar_text() {
    	local percentage=$1
    	local cols=$(tput cols)
    	let bar_size=$cols-17
    	local color="${COLOR_FG}${COLOR_BG}"
    	if [ "$PROGRESS_BLOCKED" = "true" ]; then
    	    color="${COLOR_FG}${COLOR_BG_BLOCKED}"
    	fi
    	# Prepare progress bar
		let complete_size=($bar_size*$percentage)/100
		let remainder_size=$bar_size-$complete_size
    	progress_bar=$(echo -ne "["; echo -en "${color}"; printf_new "#" $complete_size; echo -en "${RESTORE_FG}${RESTORE_BG}"; printf_new "." $remainder_size; echo -ne "]");
    	# Print progress bar
    	echo -ne " Progress ${percentage}% ${progress_bar}"
	}
	enable_trapping() {
    	TRAPPING_ENABLED="true"
	}
	trap_on_interrupt() {
    	# If this function is called, we setup an interrupt handler to cleanup the progress bar
    	TRAP_SET="true"
    	trap cleanup_on_interrupt INT
	}
	cleanup_on_interrupt() {
    	destroy_scroll_area
    	exit
	}
	printf_new() {
		str=$1
    	num=$2
    	[[ ${num} -eq 0 ]] && return;
		v=$(printf "%-${num}s" "$str")
    	echo -ne "${v// /$str}"
	}

	# Start demo
	enable_trapping
	setup_scroll_area
	draw_progress_bar 0
	sleep 2
	draw_progress_bar 10
	sleep 1
	draw_progress_bar 40
	sleep 1
	draw_progress_bar 60
	sleep 1
	draw_progress_bar 90
	sleep 1
	draw_progress_bar 100
	sleep 5
	destroy_scroll_area
}; #demo4

################################################################################
## progress-bar_v5 ${value} ${max_value} ${unit} ${title}
## 
## Example:
##  [###################################] 100% (137921 / 137921 bytes)
##
################################################################################
progress-bar_v5() {
	local PROGRESS_BAR_WIDTH=50  # progress bar length in characters
	local __value=$1
  	local __max=$2
  	local __unit=${3:-""}  # if unit is not supplied, do not display it
	local __title=${4:-""}
  	# Calculate percentage
  	if (( $__max < 1 )); then __max=1; fi  # anti zero division protection
  	local __percentage=$(( 100 - ($__max*100 - $__value*100) / $__max ))
  	# Rescale the bar according to the progress bar width
  	local __num_bar=$(( $__percentage * $PROGRESS_BAR_WIDTH / 100 ))
  	# Draw progress bar
  	printf "${__title} ["
  	for b in $(seq 1 $__num_bar); do printf "#"; done
  	for s in $(seq 1 $(( $PROGRESS_BAR_WIDTH - $__num_bar ))); do printf " "; done
  	printf "] $__percentage%% ($__value / $__max $__unit)\r"
}
demo5 () {
	local file_size=137921
	local uploaded_bytes=0
	while true; do
	  # Check if we reached 100%
	  if [[ $uploaded_bytes -gt $file_size ]]; then break; fi
	  # Get current value of uploaded bytes
	  let uploaded_bytes=uploaded_bytes+10000
	  # Draw a progress bar
	  progress-bar_v5 $uploaded_bytes $file_size "bytes" "file-1.txt"
	  sleep 1  # Wait before redrawing
	done
	# Go to the newline at the end of upload
	printf "\n"
}; #demo5

################################################################################
## spinner-v1 ${duration-in-secs}
##
################################################################################
spinner-v1() {
	local duration=${1}
	local sp='/-\|'
	local sc=0

	if [[ ${duration} -lt 0 ]]; then duration=0; fi
	
	spin() {
		printf "\bIn progress ... (${sp:sc++:1}) %s\r" $(date '+%T')
   		((sc==${#sp})) && sc=0
	}
	endspin() {
   		printf "\r%s\n" "$@"
	}
	
	until [[ ${duration} -eq 0 ]]
	do
		spin
		let duration--
		if [[ ${duration} -eq 0 ]]; then work_done=1; fi
		sleep 0.5
	done
	echo "Completed                        "
}; #spinner-v1 5
