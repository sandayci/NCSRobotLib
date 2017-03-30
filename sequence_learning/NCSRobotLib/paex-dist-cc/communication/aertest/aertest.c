#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/aerctl/aerctl.h"

#define STIMSYN 255

void resetall() {
	printf("resetting nonplastic connections...\n");
	aerResetAll(NONPLASTIC);
	printf("resetting plastic connections...\n");
	aerResetAll(PLASTIC);
}

void setconn(int target, int source, int mode, int recurrent, int wgt1,int wgt2) {
	printf("connecting %d -> %d (%d)\n", source, target, 2*mode-1);
	Synapse synapse = {
		.type = NONPLASTIC,
		.recurrent = recurrent,
		.mode = mode,
		.source = source,
		.target = target,
		.wgt1 = wgt1,
		.wgt2 = wgt2,
	};
	aerSetConn(&synapse);
}

void connExc0(int target, int source) {
	setconn(target, source, EXC, ENABLE, DISABLE, DISABLE);
}
void connExc1(int target, int source) {
	setconn(target, source, EXC, ENABLE, ENABLE, DISABLE);
}
void connExc2(int target, int source) {
	setconn(target, source, EXC, ENABLE, DISABLE, ENABLE);
}
void connExcB(int target, int source) {
	setconn(target, source, EXC, ENABLE, ENABLE, ENABLE);
}

void connInh0(int target, int source) {
	setconn(target, source, INH, ENABLE, DISABLE, DISABLE);
}
void connInh1(int target, int source) {
	setconn(target, source, INH, ENABLE, ENABLE, DISABLE);
}
void connInh2(int target, int source) {
	setconn(target, source, INH, ENABLE, DISABLE, ENABLE);
}
void connInhB(int target, int source) {
	setconn(target, source, INH, ENABLE, ENABLE, ENABLE);
}

void connIn(int target, int source) {
	setconn(target, source, EXC, DISABLE, DISABLE, DISABLE);
}

void resetbiassel() {
	aerWriteBits(1249282);
	usleep(100);
	aerWriteBits(1249282);
	usleep(100);
	aerWriteBits(1249283);
	usleep(100);
	aerWriteBits(1249283);
}

void buildWTA(int start, int size, int *cells) {
	int i;
	for (i = 0; i<size; i++) {
		connExcB(start, start+i+1);
		connInhB(start+i+1, start);
		cells[i] = start+i+1;
	}
}

void buildTestNet() {
	int wta1[10];
	int wta2[10];
	int i,j;
	buildWTA(32, 10, wta1);
	buildWTA(64, 10, wta2);
	for (i=0; i<10; i++) {
		for (j=0; j<10; j++) {
			if (j == i) {
				connExc1(wta2[j], wta1[i]);
				connExc1(wta1[j], wta2[i]);
			}
			else {
				connInh1(wta2[j], wta1[i]);
				connInh1(wta1[j], wta2[i]);
			}
		}
	}
}

int main(int argc, char **argv) {
	int c,i,n,r;
	aerOpenDevice();
	while ((c = getopt (argc, argv, "acim:n:prs:t")) != -1)
		switch (c) {

			case 'r':
				//reset all connections
				resetbiassel();
				resetall();
				return;

			case 'm':
				//switch on monitor for neuron n
				n = atoi(optarg);
				if (n<0 || n>255) {
					printf("invalid neuron id: %d\n", n);
					return -1;
				}
				printf("monitoring soma %d...\n", n);
				aerMonSoma(n);
				return;

			case 'n':
				//inject noise at given frequency
				printf("stimulating...\n");
				r = atoi(optarg) * 256;
				while (1) {
					usleep(1000000/r);
					n = rand() % 256;
					aerStimSyn(NONPLASTIC, n, STIMSYN);
				}

			case 's':
				//stimulate non-plastic synapse
				n = atoi(optarg);
				if (n<0 || n>255) {
					printf("invalid neuron id\n");
					return -1;
				}
				printf("stimulating...\n");
				while (1) {
					aerStimSyn(NONPLASTIC, n, STIMSYN);
					usleep(1000);
				}

			case 'i':
				//set input connections
				for (i=0; i<256; i++) {
					if (i != STIMSYN)
						connIn(i,STIMSYN);
				}
				return;

			case 'c':
				//random connectivity
				for (i=200; i<250; i++) { 
					for (n=0; n<10; n++) {
						r = 200 + (rand() % 50);
						connExc0(i,r);
					}
					for (n=0; n<5; n++) {
						r = 200 + (rand() % 50);
						connInh0(i,r);
					}
				}
				for (i=0; i<10*16; i++) { 
					r = 200 + (rand() % 50);
					connExc0(i,r);
				}
				return;

			case 'p':
				//read events from buffer
				i = aerFindHead();
				while(1) {
					r = aerReadMem(2*i);
					if (r != 0xffffffff) {
						i = (i+1)%AER_BUFFER_SIZE;
						printf("Received event: %03x\n", r);
					}
				}

			case 't':
				printf("generating test network...\n");
				buildTestNet();
				return;

			default:
				return 0;
		}
	printf("no arguments provided. aborting.\n");
}
