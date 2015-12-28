/*! \file test.c
	libμdaemon test
*/
#include <udaemon.h>

int main(int argc, char* argv[]) {
	// We need options damn it!~
	lud_opt opts = {
		true,				// Use syslog
		"udaemonTest",		// The daemon name
		"/tmp/udtest.lwk",	// The lock file location
		"/tmp/udtest.pid",	// The PID file location
		NULL,				// The user to switch to
		NULL				// The working dir to switch to
	};
	// Ensure we daemonize correctly
	int err;
	if((err = lud_daemonize(&opts)) < 0) {
		printf("libudaemon error: %d (%s)", err, lud_strerror(err));
		exit(err);
	}

	// DO WORK
	printf("Rarw! I'm a scawwy daemon!");

	// Job's done
	lud_cleanup();
	return err;
}
