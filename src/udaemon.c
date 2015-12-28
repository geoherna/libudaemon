/*! \file udaemon.c
	libμdaemon implementation
*/
#include <udaemon.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

static int _udaemonlockfd = 0;
static lud_opt* _dopt = NULL;
static lud_status* _ludstatus = NULL;


static void _udaemonsighndl(int sig) {
	switch(sig) {

	}
}
int lud_daemonize(lud_opt* dopt) {
	if((_dopt = dopt) == NULL) return EUDNOOPT;

	pid_t pid, sid, parent;
	int pidfd;
	if(_dopt->use_syslog)
		openlog(_dopt->name, LOG_PID, LOG_LOCAL5);
	// Already Daemonized
	if(getppid() == 1) return EUDNOERR;
	if(_dopt->lockfile && _dopt->lockfile[0]) {
		_udaemonlockfd = open(_dopt->lockfile, O_RDWR | O_CREAT, 0640);
		if(_udaemonlockfd < 0) {
			if(_dopt->use_syslog)
				closelog();
			return EUDNOLOCK;
		}
	}


	// Setup the initial status
	_ludstatus = (lud_status*)malloc(sizeof(lud_status));

	kill(parent, SIGUSR1);
}

void lud_cleanup() {
	if(_ludstatus != NULL)
		free((void*)_ludstatus);
	if(_udaemonlockfd)
		close(_udaemonlockfd);
	closelog();
}

lud_status* lud_querystatus(void) {
	return _ludstatus;
}

char* lud_strerror(int lud_errno) {
	switch(lud_errno) {
		case EUDNOERR:
			return "Assumed daemonization success";
		case EUDNOOPT:
			return "No daemon options provided";
		case EUDNOLOCK:
			return "Unable to obtain lock";
		case EUDNOUSER:
			return "Unable to switch user";
		case EUDFORK:
			return "Unable to fork off";
		case EUDSID:
			return "Unable to set SID";
		case EUDCHDIR:
			return "Unable to change working directory";
		case EUDNOPID:
			return "Unable to create PID file";
		default:
			return "Unknown Error, shit failed";
	}
}
