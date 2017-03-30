#!/bin/bash
while [ 1 ]
do
	if pgrep "servermon_multiclient" > /dev/null
	then
		echo "" > /dev/null
	else
		echo "Stopped, but is better if we run it again"
		/home/parallella/paex-dist-cc/server_new/servermon_multiclient &
	fi
done
