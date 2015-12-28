/*! \file udaemon.h
	Main header for libμdaemon
*/
#pragma once
#if !defined(_udaemon_h_)
	#define _udaemon_h_
	#if defined(__cplusplus)
		#define linkage extern "C"
	#else
		#define linkage extern
	#endif
	#include <stdbool.h>

	#define EUDNOERR	 0
	#define EUDNOOPT 	-1
	#define EUDNOLOCK 	-2
	#define EUDNOUSER 	-3
	#define EUDFORK 	-4
	#define EUDSID		-5
	#define EUDCHDIR	-6
	#define EUDNOPID	-7

	/*! \brief libμdaemon configuration
		This contains all the options for configuring libμdaemon.
	*/
	typedef struct daemon_opt {
		bool use_syslog;		/*!< Use syslog logging for internal log messages */
		const char* name;		/*!< The name of the daemon to use for log messages */
		const char* lockfile;	/*!< The lock file to use (optional) */
		const char* pidfile;	/*!< The PID file to use (optional) */
		const char* user;		/*!< The name of the user to switch to (optional) */
		const char* wdir;		/*!< The working directory to switch to (optional) */
	} lud_opt;

	/*! \brief libμdaemon status result
		This struct contains various bits of information
	that are relevant to the running daemon process.
	*/
	typedef struct deamon_status {
		int pid;	/*!< The daemons PID */
		int sid;	/*!< The current session ID */
		int ppid;	/*!< The parent PID */
	} lud_status;

	linkage int lud_daemonize(lud_opt* dopt);
	linkage void lud_cleanup();
	linkage char* lud_strerror(int lud_errno);
	linkage lud_status* lud_querystatus(void);

#endif /* _udaemon_h_ */
