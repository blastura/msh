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

/* Error safe signal handling from http://www.apuebook.com */
/* Reliable version of signal(), using POSIX sigaction().  */
Sigfunc* signal(int signo, Sigfunc *func) {
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
       act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef	SA_RESTART
       act.sa_flags |= SA_RESTART;
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
       return(SIG_ERR);
	return(oact.sa_handler);
}
