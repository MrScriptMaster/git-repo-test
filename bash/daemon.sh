#!/bin/bash
###############################################################################
# Version: 1.0.1                                                              #
#                                                                             #  
# Author:                                                                     #
#   Grigory Okhmak                                                            #
#                                                                             #
# Description:                                                                #
#   The daemon is following some file. If it is not empty, it will send       #
#   PTI request for Audiotune.                                                #
#                                                                             #
#   To start the daemon, type:                                                #
#                                                                             #
#       $ <filename>.sh start                                                 #
#                                                                             #
#   To stop the daemon, type:                                                 #
#                                                                             #
#       $ <filename>.sh stop                                                  #
#                                                                             #
#   The daemon will not allow itself to be started twice and will clean up    #
#   all the files it creates except logs. Logging is disabled by redirecting  #
#   the output stream and the error stream to /dev/null (see                  #
#   "exec" commands below in "start" procedure ).                             #
#                                                                             #
###############################################################################
## DAEMON PARAMETERS
DAEMON_NAME="pti_helper"
TMP_PATH="/tmp"
PID_FILE="${TMP_PATH}/${DAEMON_NAME}_daemon.pid"
PERIOD=10
DEBUG=1
TARGET_FILE="${TMP_PATH}/audiotune.request.pti"

## SET LOGGING
## If DEBUG=1 then daemon will create log-file to logging.
if [ $DEBUG -eq 1 ]; then
    LOG="${TMP_PATH}/${DAEMON_NAME}_daemon.log"
    ERR_LOG="${TMP_PATH}/${DAEMON_NAME}_daemon_err.log"
else
    rm -f "${TMP_PATH}/${DAEMON_NAME}_daemon.log" "${TMP_PATH}/${DAEMON_NAME}_daemon_err.log"
    LOG="/dev/null"
    ERR_LOG="/dev/null"
fi

###############################################################################
##  Function library                                                         ##                         
###############################################################################
# Help
usage() {
    echo "Usage: $0 (start|stop)"; echo
    echo "  start"
    echo "      Run daemon"
    echo "  stop"
    echo "      Stop running daemon"
}

# Logger
_log() {
    #TIMEPRINT=$(printf '\e[0;36m%s \e[0;32m%s \e[0m\n' $(date '+%T'))
    TIMEPRINT=$(date '+%T:%N')
    process=$1
    shift
    echo "$TIMEPRINT ${process} [$$]: $*"
}

# Procedure for stopping
stop() {
    # remove artefacts
    if [ -e ${PID_FILE} ]; then
        _pid=$(cat ${PID_FILE})
        # kill daemon
        kill $_pid
        rc=$?
        if [ $rc -eq 0 ]; then
            echo "Daemon is stopped"
        else
            echo "Error: can't stop daemon"
        fi
    else
        echo "Daemon was not been run"
    fi
}

# Procedure for running
start() {
    # dup-protection
    if [ -e $PID_FILE ]; then
        _pid=$(cat ${PID_FILE})
        if [ -e /proc/${_pid} ]; then
            echo "Daemon already running (pid = $_pid)"
            exit 0
        fi
    fi
    
    # create logs
    touch ${LOG}; touch ${ERR_LOG}
    
    cd /
    exec > $LOG
    exec 2> $ERR_LOG
    exec < /dev/null
    
    _log "Fork"
    # fork
    (
        # garbage collection
        # ${TMP_PATH}/*.lock
        trap "{ rm -f ${PID_FILE}; echo \"$(date '+%T:%N') Daemon closing\"; exit 254; }"  EXIT
        trap "{ rm -f ${PID_FILE}; echo \"$(date '+%T:%N') Daemon received SIGINT\"; exit 254; }"  SIGINT
        trap "{ rm -f ${PID_FILE}; echo \"$(date '+%T:%N') Daemon received SIGTERM\"; exit 254; }"  SIGTERM
        trap "{ rm -f ${PID_FILE}; echo \"$(date '+%T:%N') Daemon received SIGKILL\"; exit 255; }"  SIGKILL
        
        _log "Preparing jobs"
        arrJobs=("lock")
        arrLock=("${TMP_PATH}/${DAEMON_NAME}.lock")
        
        # main cycle
        while [ 1 ]
        do
            #_log "Array elems in arrJobs is ${!arrJobs[*]}"
            for idx in ${!arrJobs[*]}
            do
                rerun ${arrJobs[$idx]} ${arrLock[$idx]}
            done
            sleep ${PERIOD}    # period
        done
        exit 0
    )&
    
    # save pid
    echo $! > ${PID_FILE}
}

# 
# Usage: rerun $1 $2
#   $1 -- path to an executable file
#   $2 -- lock file path
#
rerun () {
    (
        _log "rerun($1, $2)"
        # garbage collection
        unset arrJobs
        unset arrLock
        RESULT=0
        if [ ! -e $2 ]; then
            echo "$$" > $2
            trap "{ rm -f $2; echo \"$(date '+%T:%N') Сhild $1 received SIGKILL\"; exit 255; }" SIGKILL 
            trap "{ rm -f $2; echo \"$(date '+%T:%N') Сhild $1 received SIGTERM\"; exit 254; }" SIGTERM 
            trap "{ rm -f $2; echo \"$(date '+%T:%N') Сhild $1 received SIGINT\"; exit 254; }" SIGINT 
            trap "{ rm -f $2; echo \"$(date '+%T:%N') Сhild $1 closing\"; exit 254; }" EXIT

            _log "Running $1"
            # main()
            if [ -e  "${TARGET_FILE}" ]; then
                if [ -f "${TARGET_FILE}" ]; then
                    if [ -s "${TARGET_FILE}" ]; then
                        _log "Execute program"
                        #exec 3<> ${OUTPUT}
                        #cat /dev/null >&3
                        #${CMD} >&3
                        #exec 3>&-
                        # Clean file
                        cat /dev/null > "${TARGET_FILE}"
                    else
                        _log "${TARGET_FILE} file is empty"
                        RESULT=-3
                    fi
                else
                    _log "${TARGET_FILE} is not a file"
                    RESULT=-2
                fi
            else
                _log "File ${TARGET_FILE} is not exist"
                RESULT=-1
            fi
            # end main
            #_log "Task $1 returns $?"
            _log "Task $1 returns ${RESULT}"
        else
            _log "Task is running yet"
        fi
        exit 0
    )&
}

###############################################################################
##  Mode selection                                                           ##                         
###############################################################################
case $1 in
    "start")
        start
        ;;
    "stop")
        stop
        ;;
    *)
        usage
        ;;
esac
exit 0
