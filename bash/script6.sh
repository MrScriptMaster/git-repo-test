#!/bin/bash
LOCK=/var/lock/`basename $0`
(
  flock -nx 99
  if [ $? -ne 0 ]; then
    echo "Multiple running detected. Aborting"
    exit 1
  fi
  echo "Hello"
) 99>$LOCK
read -p "Press [Enter] key to reboot system..., ^c to abort reboot"
exit 0
