#include "sighant.h"

#include <stdio.h>
#include <signal.h>

/* Signal handler */
void signal_handler(int signum) {
    if (signum == SIGINT) {
        // DO what? printf("signal: %d\n", signum);
        // Close all open processes
    }
}
