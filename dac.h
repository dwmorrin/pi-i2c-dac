#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pigpio.h>

// look at https://github.com/adafruit/Adafruit_MCP4725/blob/master/Adafruit_MCP4725.h
// for where these and the reading/writing come from
#define ADDRESS 0x62
#define DAC_MAX_PLUS_1 4096
#define FIRST_BYTE(n) (n / 16)
#define SECOND_BYTE(n) ((n % 16) << 4)
#define WRITE_BYTE 0x40

void cycle(void);
void fatal(const char*);
void i2cError(int);
void onExit(void);
void onInterrupt(int);
