/*================================================================
*
*  文件名称：log.h
*  创 建 者: mongia
*  创建日期：2021年04月01日
*
*  copy from st
*
================================================================*/

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#define plog_debug(x) std::cout << "[debug] " << x << std::endl;
#define plog_info(x) std::cout << "[info ] " << x << std::endl;
#define plog_warn(x) std::cout << "[warn ] " << x << std::endl;
#define plog_error(x) std::cout << "[error] " << x << std::endl;
#define plog_fatal(x)                \
    {                                \
        std::cout << "[fatal] " << x << std::endl; \
        exit(1);                     \
    }

/*
 * Simple error reporting functions.
 * Suggested in W. Richard Stevens' "Advanced Programming in UNIX
 * Environment".
 */

#define MAXLINE 4096 /* max line length */

static int stout = 1;
static int logfd = stout;
static void log_init(const char *logname);
static void err_doit(int, int, const char *, va_list);

/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void err_sys_report(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(logfd, 1, fmt, ap);
    va_end(ap);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys_quit(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(logfd, 1, fmt, ap);
    va_end(ap);
    exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void err_sys_dump(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(logfd, 1, fmt, ap);
    va_end(ap);
    abort(); /* dump core and terminate */
    exit(1); /* shouldn't get here */
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void err_report(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(logfd, 0, fmt, ap);
    va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(logfd, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

/*
 * Return a pointer to a string containing current time.
 */
char *err_tstamp(void) {
    static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static char str[32];
    static time_t lastt = 0;
    struct tm *tmp;
    time_t currt = time(0);

    if (currt == lastt) return str;

    tmp = localtime(&currt);
    sprintf(str, "[%02d/%s/%d:%02d:%02d:%02d] ", tmp->tm_mday,
            months[tmp->tm_mon], 1900 + tmp->tm_year, tmp->tm_hour, tmp->tm_min,
            tmp->tm_sec);
    lastt = currt;

    return str;
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void err_doit(int fd, int errnoflag, const char *fmt, va_list ap) {
    int errno_save;
    char buf[MAXLINE];

    errno_save = errno;        /* value caller might want printed   */
    strcpy(buf, err_tstamp()); /* prepend a message with time stamp */
    vsprintf(buf + strlen(buf), fmt, ap);
    if (errnoflag)
        sprintf(buf + strlen(buf), ": %s\n", strerror(errno_save));
    else
        strcat(buf, "\n");
    write(fd, buf, strlen(buf));
    errno = errno_save;
}

static void log_init(const char *logname) {
    int fd;
    if ((fd = open(logname, O_CREAT | O_WRONLY | O_APPEND, 0644)) < 0)
        err_sys_quit("ERROR: can't open error log file: open");
    logfd = fd;
}
