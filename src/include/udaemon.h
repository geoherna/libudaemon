/*! \file udaemon.h
	Main header for libμdaemon
*/
#pragma once
#if !defined(_udaemon_h_)

	#define _udaemon_h_

	#if defined(__cplusplus)
		#define lud_linkage extern "C"
	#else /* __cplusplus */
		#define lud_linkage extern
	#endif /* c */

	#include <stdbool.h>

	#define EUDNOERR	 0
	#define EUDNOOPT 	-1
	#define EUDNOLOCK 	-2
	#define EUDNOUSER 	-3
	#define EUDFORK 	-4
	#define EUDSID		-5
	#define EUDCHDIR	-6
	#define EUDNOPID	-7
	#define EUDNOSIG	-8

	#if defined(LUD_AUTO_DTOR)
		#define LUD_DTOR()					\
			__attribute__((destructor(101)))\
			void __lud_dtor_w(void) {		\
				lud_cleanup();				\
			}
	#endif /* LUD_AUTO_DTOR */

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

	/*! \brief Deaemonize the current process
		Daemonizes the current calling process using the proved options passed
		by \p dopt

		If daemonization is successful then this method will return 0, otherwise
		it will return an error code that can be looked up with lud_strerror

		\param dopt the pointer to the lud_opt struct
		\returns 0 if daemon is successful or an error code
	*/
	lud_linkage int lud_daemonize(lud_opt* dopt);
	/*! \brief Resource reclamation
		Cleans up after the lud_daemonize call
	*/
	lud_linkage void lud_cleanup();
	/*! \brief Returns the error message for the lud_daemonize result

		This method will return a human readable error message from the
		return value of the lud_daemonize call

		\param lud_errno The error to lookup
		\returns Human readable error message
	*/
	lud_linkage char* lud_strerror(int lud_errno);
	/*! \brief Get current running daemon info

		Returns the current PID, PPID and SID for the running daemon

		\returns a lus_status struct pointer
	*/
	lud_linkage lud_status* lud_querystatus(void);

	/*! \brief used to signal a running daemon
		This can be used to automatically signal the running
		instance of this daemon process from any other instance

		This allows you to do things like notify the daemon of a
		configuration change or to kill it.

		\returns 0 on success or an lud_error code
	*/
	lud_linkage	int lud_signaldaemon(int sig, lud_opt* dopt);

#endif /* _udaemon_h_ */
