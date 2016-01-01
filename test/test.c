/*! \file test.c
	libμdaemon test
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <udaemon.h>
#include <signal.h>

static void sig(int s) {
	if(s == SIGHUP) {
		syslog(LOG_INFO, "HI MOM!");
	}
}

int main(int argc, char* argv[]) {
	// We need options damn it!~
	lud_opt opts = {
		1,					// Use syslog
		"udaemonTest",		// The daemon name
		"/tmp/udtest.lwk",	// The lock file location
		"/tmp/udtest.pid",	// The PID file location
		NULL,				// The user to switch to
		NULL				// The working dir to switch to
	};
	int err;
	if (argc > 1) {
		if(strncmp(argv[1], "kill", 4) == 0) {
			if((err = lud_signaldaemon(SIGTERM, &opts)) < 0) {
				printf("libudaemon error: %d (%s)\n", err, lud_strerror(err));
				exit(err);
			}
		}
		if(strncmp(argv[1], "hup", 3) == 0) {
			if((err = lud_signaldaemon(SIGHUP, &opts)) < 0) {
				printf("libudaemon error: %d (%s)\n", err, lud_strerror(err));
				exit(err);
			}
		}
		exit(0);
	}

	// Ensure we daemonize correctly
	if((err = lud_daemonize(&opts)) < 0) {
		printf("libudaemon error: %d (%s)\n", err, lud_strerror(err));
		exit(err);
	}

	signal(SIGHUP, sig);

	// DO WORK
	syslog(LOG_INFO, "Rarw! I'm a scawwy daemon!");

	lud_status* stat = lud_querystatus();
	syslog(LOG_INFO, "PID: %d PPID: %d SID: %d", stat->pid, stat->ppid, stat->sid);

	for(;;) {

	}

	// Job's done
	lud_cleanup();
	return err;
}
