cmake_minimum_required(VERSION 2.8.0)
project(libudaemon)

include_directories(include)

include(CheckIncludeFiles)

option(BUILD_STATIC_LIBRARY " Build the static libudaemon library" ON)
option(BUILD_SHARED_LIBRARY " Build the shared libudaemon library" ON)

check_include_files("stdio.h" HAVE_STDIO_H)
check_include_files("stdlib.h" HAVE_STDLIB_H)
check_include_files("unistd.h" HAVE_UNISTD_H)
check_include_files("fcntl.h" HAVE_FCNTL_H)
check_include_files("signal.h" HAVE_SIGNAL_H)
check_include_files("errno.h" HAVE_ERRNO_H)
check_include_files("pwd.h" HAVE_PWD_H)
check_include_files("string.h" HAVE_STRING_H)
check_include_files("syslog.h" HAVE_SYSLOG_H)
check_include_files("sys/types.h" HAVE_TYPES_H)
check_include_files("sys/stat.h" HAVE_STAT_H)
check_include_files("sys/file.h" HAVE_FILE_H)

# check_prototype_exists(openlog "syslog.h" HAVE_OPENLOG)
# check_prototype_exists(closelog "syslog.h" HAVE_CLOSELOG)
# check_prototype_exists(syslog "syslog.h" HAVE_SYSLOG)

# check_prototype_exists(getppid "sys/types.h;unistd.h" HAVE_GETPPID)
# check_prototype_exists(getpid "sys/types.h;unistd.h" HAVE_GETPID)
# check_prototype_exists(getuid "sys/types.h;unistd.h" HAVE_GETUID)
# check_prototype_exists(geteuid "sys/types.h;unistd.h" HAVE_GETEUID)
# check_prototype_exists(setuid "sys/types.h;unistd.h" HAVE_SETUID)

# check_prototype_exists(freopen "stdio.h" HAVE_FREOPEN)
# check_prototype_exists(snprintf "stdio.h" HAVE_SNPRINTF)

# check_prototype_exists(fork "unistd.h" HAVE_FORK)
# check_prototype_exists(alarm "unistd.h" HAVE_ALARM)
# check_prototype_exists(pause "unistd.h" HAVE_PAUSE)
# check_prototype_exists(setsid "unistd.h" HAVE_SETSID)
# check_prototype_exists(close "unistd.h" HAVE_CLOSE)
# check_prototype_exists(write "unistd.h" HAVE_WRITE)


# check_prototype_exists(signal "signal.h" HAVE_SIGNAL)
# check_prototype_exists(kill "signal.h;sys/types.h" HAVE_KILL)

# check_prototype_exists(umask "sys/types.h;sys/stat.h" HAVE_UMASK)

# check_prototype_exists(open "sys/types.h;sys/stat.h;fcntl.h" HAVE_OPEN)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CC_FLAGS} -g")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CC_FLAGS} -Werror -fno-builtin")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-overflow=1 -Wformat=2 -Wuninitialized -Werror=return-type  -Werror=shadow -fstack-protector -Wformat-security")

file(GLOB SOURCES "./*.c")

if(BUILD_SHARED_LIBRARY)
	add_library(udaemon SHARED ${SOURCES})
endif()

if(BUILD_STATIC_LIBRARY)
	add_library(udaemonst STATIC ${SOURCES})
	set_target_properties(udaemonst PROPERTIES OUTPUT_NAME udaemon)
endif()
