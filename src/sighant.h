/* Handles singals */
void signal_handler(int signum);

/* Needed for compiling sighant.c  on solaris, code from www.apuebook.com */
#if defined(SOLARIS)
#define _XOPEN_SOURCE	500	/* Single UNIX Specification, Version 2  for Solaris 9 */
#define CMSG_LEN(x)	_CMSG_DATA_ALIGN(sizeof(struct cmsghdr)+(x))
#elif !defined(BSD)
#define _XOPEN_SOURCE	600	/* Single UNIX Specification, Version 3 */
#endif

/* Error safe signal handling from http://www.apuebook.com */
typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc* signal(int signo, Sigfunc *func);
