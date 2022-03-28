#!/bin/bash

connect () {
	nc -c localhost 5555
}

(
./ft_irc 5555
)

RET=1
while [ $RET -ne 0 ]
do
	echo "Trying connection..."
	# connect
	RET="$?"
	sleep 1
done
