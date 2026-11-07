/* Test-only syslog interceptor; no host syslog service is required. */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static void capture_syslog(const char *format, va_list args)
{
    const char *path = getenv("PHP_FPM_TEST_SYSLOG_CAPTURE");
    if (path == NULL || *path == '\0') {
        return;
    }

    char message[8192];
    int len = vsnprintf(message, sizeof(message), format, args);
    if (len < 0) {
        return;
    }

    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0600);
    if (fd < 0) {
        return;
    }
    size_t count = (size_t) len < sizeof(message) ? (size_t) len : sizeof(message) - 1;
    (void) write(fd, message, count);
    (void) write(fd, "\n", 1);
    close(fd);
}

void syslog(int priority, const char *format, ...)
{
    (void) priority;
    va_list args;
    va_start(args, format);
    capture_syslog(format, args);
    va_end(args);
}

void vsyslog(int priority, const char *format, va_list args)
{
    (void) priority;
    capture_syslog(format, args);
}

/* glibc uses these symbols when _FORTIFY_SOURCE is enabled. */
void __syslog_chk(int priority, int flag, const char *format, ...)
{
    (void) priority;
    (void) flag;
    va_list args;
    va_start(args, format);
    capture_syslog(format, args);
    va_end(args);
}

void __vsyslog_chk(int priority, int flag, const char *format, va_list args)
{
    (void) priority;
    (void) flag;
    capture_syslog(format, args);
}
