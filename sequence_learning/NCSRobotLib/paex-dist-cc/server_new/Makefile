CC=gcc
CFLAGS =  -I

servermon_multiclient: comm/aerctl.c servermon_multiclient.c
	$(CC)  -DDEVICE_MN256R1 comm/aerctl.c -o servermon_multiclient servermon_multiclient.c -lpthread -I.

serversec_multiclient: comm/aerctl.c serversec_multiclient.c
	$(CC)  -DDEVICE_MN256R1 comm/aerctl.c -o serversec_multiclient serversec_multiclient.c -lpthread -I.
