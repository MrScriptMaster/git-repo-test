#!/bin/bash


if [[ $(id -u) != "0" ]]; then
	echo -e "\n*** Пользователь не обладает привилегиями суперпользователя ***\n"
	#exit 1
fi
