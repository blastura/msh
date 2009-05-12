/* Handles singals */
void signal_handler(int signum);

/* Error safe signal handling from http://www.apuebook.com */
typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc* signal(int signo, Sigfunc *func);
