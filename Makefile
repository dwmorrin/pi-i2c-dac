dac: dac.c dac.h
	gcc -g -Wall -pthread -o dac dac.c -lpigpio -lrt
