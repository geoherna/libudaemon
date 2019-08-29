/*! \file udaemon.c
        libμdaemon implementation
*/
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <udaemon.h>
#include <unistd.h>

static int _udaemonlockfd = 0;
static int _udaemonlockrc = 0;
static lud_opt *_dopt = NULL;
static lud_status *_ludstatus = NULL;

static void _udaemonsighndl(int sig) {
  switch (sig) {
  case SIGPIPE:
  case SIGTERM: {
    if (_ludstatus != NULL)
      free((void *)_ludstatus);
    exit(0);
  }
  case SIGALRM:
  case SIGCHLD: {
    if (_ludstatus != NULL)
      free((void *)_ludstatus);
    exit(-1);
  }
  default: {}
  }
}

int lud_daemonize(lud_opt *dopt) {
  if ((_dopt = dopt) == NULL)
    return EUDNOOPT;

  pid_t pid, sid, parent;
  int pidfd;

  if (_dopt->use_syslog == 1)
    openlog(_dopt->name, LOG_PID, LOG_LOCAL5);
  // Already Daemonized
  if (getppid() == 1)
    return EUDNOERR;
  // Lockfile
  if (_dopt->lockfile && _dopt->lockfile[0]) {
    _udaemonlockfd = open(_dopt->lockfile, O_RDWR | O_CREAT, 0666);
    if (_udaemonlockfd < 0) {
      if (_dopt->use_syslog == 1) {
        syslog(LOG_ERR, "Unable to obtain lock on %s! %d (%s)", _dopt->lockfile,
               errno, strerror(errno));
        closelog();
      }
            
      return EUDNOLOCK;
    }
    _udaemonlockrc = flock(_udaemonlockfd, LOCK_EX | LOCK_NB);
    if (_udaemonlockrc < 0) {
      if (_dopt->use_syslog == 1) {
        syslog(LOG_ERR, "Unable to obtain lock on %s! %d (%s)", _dopt->lockfile,
               errno, strerror(errno));
        closelog();
      }
            
      return EUDNOLOCK;
    }
  }
  // Change user
  if (_dopt->user && _dopt->user[0]) {
    if (getuid() == 0 || geteuid() == 0) {
      struct passwd *pw = getpwnam(_dopt->user);
      if (pw) {
        if (_dopt->use_syslog == 1)
          syslog(LOG_NOTICE, "Setting user to %s(%d)", _dopt->user, pw->pw_uid);
        setuid(pw->pw_uid);
      } else {
        if (_dopt->use_syslog == 1) {
          syslog(LOG_ERR, "Unable to switch to user %s! %d (%s)", _dopt->user,
                 errno, strerror(errno));
          closelog();
        }
              
        return EUDNOUSER;
      }
    }
  }

  // Register signals
  signal(SIGALRM, _udaemonsighndl);
  signal(SIGTERM, _udaemonsighndl);
  signal(SIGHUP, _udaemonsighndl);
  signal(SIGCHLD, _udaemonsighndl);
  signal(SIGUSR1, _udaemonsighndl);

  // Fork off~
  pid = fork();
  if (pid < 0) {
    if (_dopt->use_syslog == 1) {
      syslog(LOG_ERR, "Unable to fork process! %d (%s)", errno,
             strerror(errno));
      closelog();
    }
          
    return EUDFORK;
  }

  // Wait for the noose
  if (pid > 0) {
    alarm(2);
    pause();
    exit(-1);
  }

  // Get the parent PID
  parent = getppid();
  // Reset some signals
  signal(SIGCHLD, SIG_DFL); /* A child process dies */
  signal(SIGTSTP, SIG_IGN); /* Various TTY signals */
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  // Set the file mask
  umask(0);

  // Attempt to switch sessions
  sid = setsid();
  if (sid < 0) {
    if (_dopt->use_syslog == 1) {
      syslog(LOG_ERR, "Unable to switch session! %d (%s)", errno,
             strerror(errno));
      closelog();
    }
          
    return EUDSID;
  }

  // Switch working directory
  if (_dopt->wdir && _dopt->wdir[0]) {
    if (chdir(_dopt->wdir) < 0) {
      if (_dopt->use_syslog == 1) {
        syslog(LOG_ERR, "Unable to switch working directory to %s! %d (%s)",
               _dopt->wdir, errno, strerror(errno));
        closelog();
      }
            
      return EUDCHDIR;
    }
  }

  // Attempt to create pid file
  if (_dopt->pidfile && _dopt->pidfile[0]) {
    if ((pidfd = open(_dopt->pidfile, O_CREAT | O_RDWR, 0640)) < 0) {
      if (_dopt->use_syslog == 1) {
        syslog(LOG_ERR, "Unable to create PID file %s! %d (%s)", _dopt->pidfile,
               errno, strerror(errno));
        closelog();
      }
            
      return EUDNOPID;
    }
    char buff[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // Null out the PID file
    write(pidfd, buff, strlen(buff));
    snprintf(buff, sizeof(buff), "%d", getpid());
    write(pidfd, buff, strlen(buff));
    close(pidfd);
  }

  // Setup the initial status
  _ludstatus = (lud_status *)malloc(sizeof(lud_status));

  _ludstatus->pid = getpid();
  _ludstatus->sid = getsid(_ludstatus->pid);
  _ludstatus->ppid = getppid();

  // Redirect IO
  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);

  // Asphyxiate the parent process
  kill(parent, SIGTERM);
  if (_dopt->use_syslog == 1)
    syslog(LOG_NOTICE, "Process daemonized to pid %d", _ludstatus->pid);
        
  return EUDNOERR;
}

void lud_cleanup() {
  if (_dopt->use_syslog == 1)
    syslog(LOG_NOTICE, "Cleaning up...");
  if (_ludstatus != NULL)
    free((void *)_ludstatus);
  if (_udaemonlockfd)
    close(_udaemonlockfd);
  closelog();
}

lud_status *lud_querystatus(void) { return _ludstatus; }

char *lud_strerror(int lud_errno) {
  switch (lud_errno) {
  case EUDNOERR:
    return "Assumed daemonization success";
  case EUDNOOPT:
    return "No daemon options provided";
  case EUDNOLOCK:
    return "Unable to obtain lock (am I already running?)";
  case EUDNOUSER:
    return "Unable to switch user";
  case EUDFORK:
    return "Unable to fork off";
  case EUDSID:
    return "Unable to set SID";
  case EUDCHDIR:
    return "Unable to change working directory";
  case EUDNOPID:
    return "Unable to poke PID file (is it even set?)";
  case EUDNOSIG:
    return "Unable to send signal to daemon (is it running?)";
  default:
    return "Unknown Error, shit failed";
  }
}

int lud_signaldaemon(int sig, lud_opt *dopt) {
  if (dopt == NULL)
    return EUDNOOPT;
  if (dopt->pidfile && dopt->pidfile[0]) {
    int pidfd;
    if ((pidfd = open(dopt->pidfile, O_RDWR, 0640)) < 0) {
      if (dopt->use_syslog) {
        syslog(LOG_ERR, "Unable to open PID file %s! %d (%s)", dopt->pidfile,
               errno, strerror(errno));
        closelog();
      }
            
      return EUDNOPID;
    }

    char buff[10];
    read(pidfd, buff, sizeof(buff));
    int pid = atoi(buff);
    close(pidfd);

    if (kill(pid, sig) < 0) {
      if (dopt->use_syslog) {
        syslog(LOG_ERR, "Unable to send signal %d to %d! %d (%s)", sig, pid,
               errno, strerror(errno));
        closelog();
      }
      return EUDNOSIG;
    }
  } else {
    return EUDNOPID;
  }
        
  return EUDNOERR;
}

// void lud_registersighandle(int sig, lud_sighandle handle) {

// }
