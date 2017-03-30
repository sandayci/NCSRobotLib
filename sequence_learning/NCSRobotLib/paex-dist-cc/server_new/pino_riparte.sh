#!/bin/bash
while [ 1 ]
do
	if pgrep "serversec_multiclient" > /dev/null
	then
		echo "" > /dev/null
	else
		echo "Stopped, but is better if we run it again"
		/home/parallella/paex-dist-cc/server_new/serversec_multiclient &
	fi
done
