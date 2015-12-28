# libμdaemon
A small C/C++ library to make writing Unix daemons simple

## Building
libμdaemon is standalone, you don't need to link it with anything strange or download a tar file and stick it in the source directory, if you have a decent unix-like system, FreeBSD, Arch Linux, for example, CMake, and a decent compiler then you can build the library.

```
$ git clone https://github.com/xampp/libudaemon.git
$ cd libudaemon
$ mkdir build
$ cd !$ && cmake ../
$ make
``` 

That's all there is to it.

## Example
```{.c}
#include <stdio.h>
#include <stdlib.h>
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
```

## License
libμdaemon is licensed under the BSD 3-clause license, see [LICENSE.md](https://github.com/XAMPP/lib-daemon/blob/master/LICENSE.md)
