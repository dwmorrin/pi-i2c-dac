#include "dac.h"

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
    int handle;
    handle = i2cOpen(1, ADDRESS, 0);
    if (handle < 0) i2cError(handle);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char buf[3];
    char data[3] = {WRITE_BYTE,0xFF,0xFF};

    while (1) {
        printf("> ");
        nread = getline(&line, &len, stdin);
        if (nread < 0) fatal("bad input");
        if (strstr(line, "quit")) {
            exit(EXIT_SUCCESS);
        }
        else if (strstr(line, "read")) {
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
            puts("write OK");
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
    printf("\ngoodbye\n");
}

// ctrl+c handler
void onInterrupt(int signum) {
    exit(EXIT_SUCCESS);
}

