#include "dac.h"

char data[3] = {WRITE_BYTE,0xFF,0xFF};
int handle;

int main(void) {
    if (gpioInitialise() < 0) {
        fatal("could not initialize gpio\n");
    }
    if (atexit(onExit)) {
        gpioTerminate();
        fatal("could not set exit function\n");
    }
    signal(SIGINT, onInterrupt);

    // get handle for the chip
    handle = i2cOpen(1, ADDRESS, 0);
    if (handle < 0) i2cError(handle);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1) {
        nread = getline(&line, &len, stdin);
        if (nread < 0) exit(errno);
        if (strstr(line, "quit")) {
            exit(EXIT_SUCCESS);
        }
        else if (strstr(line, "read")) {
            char buf[3];
            int err = i2cReadDevice(handle, buf, 3);
            if (err <= 0) i2cError(err);
            for (int i = 0; i < 3; ++i)
                printf("[%d]", buf[i]);
            puts("");
        }
        else if (strstr(line, "write")) {
            unsigned value = 0;
            sscanf(line, "write %u", &value);
            if (value < 0 || value > 4095) fatal("bad value");
            data[1] = FIRST_BYTE(value);
            data[2] = SECOND_BYTE(value);
            int err = i2cWriteDevice(handle, data, 3);
            if (err) i2cError(err);
        }
        else if (strstr(line, "cycle")) {
            int err = gpioSetTimerFunc(0, 20, cycle);
            if (err) fatal("Setting timer failed");
            // exit cycle with a keypress
            getchar();
            err = gpioSetTimerFunc(0, 20, NULL);
            if (err) fatal("Setting timer failed");
        }
        else {
            puts(
                "Commands: \n"
                "read\n"
                "write\n"
                "quit\n"
            );
        }
    }
    
    exit(EXIT_FAILURE); // unreachable
}

void cycle(void) {
    static unsigned value = 0;
    value = (value + 10) % 4096;
    data[1] = FIRST_BYTE(value);
    data[2] = SECOND_BYTE(value);
    int err = i2cWriteDevice(handle, data, 3);
    if (err) i2cError(err);
}

void fatal(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void i2cError(int error) {
    switch (error) {
        case PI_BAD_I2C_BUS:
            fatal("bad i2c bus\n");
            break;
        case PI_BAD_I2C_ADDR:
            fatal("bad i2c address\n");
            break;
        case PI_BAD_FLAGS:
            fatal("bad flags\n");
            break;
        case PI_BAD_HANDLE:
            fatal("bad handld\n");
            break;
        case PI_I2C_READ_FAILED:
            fatal("read failed\n");
            break;
        case PI_NO_HANDLE:
            fatal("no handle\n");
            break;
        case PI_I2C_OPEN_FAILED:
            fatal("i2c open failed\n");
            break;
        default:
            fatal("unknown i2c error");
    }
}

// runs after a call to exit()
void onExit(void) {
    gpioTerminate();
}

// ctrl+c handler
void onInterrupt(int signum) {
    exit(EXIT_SUCCESS);
}

