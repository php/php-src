
	/* $Id: fpm_status.c 312262 2011-06-18 17:41:56Z felipe $ */
	/* (c) 2009 Jerome Loyet */

#include "php.h"
#include "SAPI.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_TIMES
#include <sys/times.h>
#endif

#include "fpm_config.h"
#include "fpm_log.h"
#include "fpm_clock.h"
#include "fpm_process_ctl.h"
#include "fpm_signals.h"
#include "fpm_scoreboard.h"
#include "fastcgi.h"
#include "zlog.h"

#ifdef MAX_LINE_LENGTH
# define FPM_LOG_BUFFER MAX_LINE_LENGTH
#else
# define FPM_LOG_BUFFER 1024
#endif

static char *fpm_log_format = NULL;
static int fpm_log_fd = -1;

int fpm_log_open(int reopen) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	int ret = 1;
	
	int fd;
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config->access_log) {
			continue;
		}
		ret = 0;
		
		fd = open(wp->config->access_log, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		if (0 > fd) {
			zlog(ZLOG_SYSERROR, "failed to open access log (%s)", wp->config->access_log);
			return -1;
		}

		if (reopen) {
			dup2(fd, wp->log_fd);
			close(fd);
			fd = wp->log_fd;
			fpm_pctl_kill_all(SIGQUIT);
		} else {
			wp->log_fd = fd;
		}

		fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
	}

	return ret;
}
/* }}} */

/* }}} */
int fpm_log_init_child(struct fpm_worker_pool_s *wp)  /* {{{ */
{
	if (!wp || !wp->config) {
		return -1;
	}

	if (wp->config->access_log && *wp->config->access_log) {
		if (wp->config->access_format) {
			fpm_log_format = strdup(wp->config->access_format);
		}
	}

	if (fpm_log_fd == -1) {
		fpm_log_fd = wp->log_fd;
	}


	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (wp->log_fd > -1 && wp->log_fd != fpm_log_fd) {
			close(wp->log_fd);
			wp->log_fd = -1;
		}
	}

	return 0;
}
/* }}} */

int fpm_log_write(char *log_format TSRMLS_DC) /* {{{ */
{
	char *s, *b;
	char buffer[FPM_LOG_BUFFER+1];
	int token, test;
	size_t len, len2;
	struct fpm_scoreboard_proc_s proc, *proc_p;
	struct fpm_scoreboard_s *scoreboard;
	char tmp[129];
	char format[129];
	time_t now_epoch;
#ifdef HAVE_TIMES
	clock_t tms_total;
#endif

	if (!log_format && (!fpm_log_format || fpm_log_fd == -1)) {
		return -1;
	}

	if (!log_format) {
		log_format = fpm_log_format;
		test = 0;
	} else {
		test = 1;
	}

	now_epoch = time(NULL);

	if (!test) {
		scoreboard = fpm_scoreboard_get();
		if (!scoreboard) {
			zlog(ZLOG_WARNING, "unable to get scoreboard while preparing the access log");
			return -1;
		}
		proc_p = fpm_scoreboard_proc_acquire(NULL, -1, 0);
		if (!proc_p) {
			zlog(ZLOG_WARNING, "[pool %s] Unable to acquire shm slot while preparing the access log", scoreboard->pool);
			return -1;
		}
		proc = *proc_p;
		fpm_scoreboard_proc_release(proc_p);
	}

	token = 0;

	memset(buffer, '\0', sizeof(buffer));
	b = buffer;
	len = 0;


	s = log_format;

	while (*s != '\0') {
		/* Test is we have place for 1 more char. */
		if (len >= FPM_LOG_BUFFER) {
			zlog(ZLOG_NOTICE, "the log buffer is full (%d). The access log request has been truncated.", FPM_LOG_BUFFER);
			len = FPM_LOG_BUFFER;
			break;
		}

		if (!token && *s == '%') {
			token = 1;
			memset(format, '\0', sizeof(format)); /* reset format */
			s++;
			continue;
		}

		if (token) {
			token = 0;
			len2 = 0;
			switch (*s) {

				case '%': /* '%' */
					*b = '%';
					len2 = 1;
					break;

#ifdef HAVE_TIMES
				case 'C': /* %CPU */
					if (format[0] == '\0' || !strcasecmp(format, "total")) {
						if (!test) {
							tms_total = proc.last_request_cpu.tms_utime + proc.last_request_cpu.tms_stime + proc.last_request_cpu.tms_cutime + proc.last_request_cpu.tms_cstime;
						}
					} else if (!strcasecmp(format, "user")) {
						if (!test) {
							tms_total = proc.last_request_cpu.tms_utime + proc.last_request_cpu.tms_cutime;
						}
					} else if (!strcasecmp(format, "system")) {
						if (!test) {
							tms_total = proc.last_request_cpu.tms_stime + proc.last_request_cpu.tms_cstime;
						}
					} else {
						zlog(ZLOG_WARNING, "only 'total', 'user' or 'system' are allowed as a modifier for %%%c ('%s')", *s, format);
						return -1;
					}

					format[0] = '\0';
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%.2f", tms_total / fpm_scoreboard_get_tick() / (proc.cpu_duration.tv_sec + proc.cpu_duration.tv_usec / 1000000.) * 100.);
					}
					break;
#endif

				case 'd': /* duration µs */
					/* seconds */
					if (format[0] == '\0' || !strcasecmp(format, "seconds")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%.3f", proc.duration.tv_sec + proc.duration.tv_usec / 1000000.);
						}

					/* miliseconds */
					} else if (!strcasecmp(format, "miliseconds") || !strcasecmp(format, "mili")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%.3f", proc.duration.tv_sec * 1000. + proc.duration.tv_usec / 1000.);
						}

					/* microseconds */
					} else if (!strcasecmp(format, "microseconds") || !strcasecmp(format, "micro")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%lu", proc.duration.tv_sec * 1000000UL + proc.duration.tv_usec);
						}

					} else {
						zlog(ZLOG_WARNING, "only 'seconds', 'mili', 'miliseconds', 'micro' or 'microseconds' are allowed as a modifier for %%%c ('%s')", *s, format);
						return -1;
					}
					format[0] = '\0';
					break;

				case 'e': /* fastcgi env  */
					if (format[0] == '\0') {
						zlog(ZLOG_WARNING, "the name of the environment variable must be set between embraces for %%%c", *s);
						return -1;
					}

					if (!test) {
						char *env = fcgi_getenv((fcgi_request*) SG(server_context), format, strlen(format));
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", env ? env : "-");
					}
					format[0] = '\0';
					break;

				case 'f': /* script */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s",  proc.script_filename && *proc.script_filename ? proc.script_filename : "-");
					}
					break;

				case 'l': /* content length */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%zu", proc.content_length);
					}
					break;

				case 'm': /* method */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", proc.request_method && *proc.request_method ? proc.request_method : "-");
					}
					break;

				case 'M': /* memory */
					/* seconds */
					if (format[0] == '\0' || !strcasecmp(format, "bytes")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%zu", proc.memory);
						}

					/* kilobytes */
					} else if (!strcasecmp(format, "kilobytes") || !strcasecmp(format, "kilo")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%lu", proc.memory / 1024);
						}

					/* megabytes */
					} else if (!strcasecmp(format, "megabytes") || !strcasecmp(format, "mega")) {
						if (!test) {
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%lu", proc.memory / 1024 / 1024);
						}

					} else {
						zlog(ZLOG_WARNING, "only 'bytes', 'kilo', 'kilobytes', 'mega' or 'megabytes' are allowed as a modifier for %%%c ('%s')", *s, format);
						return -1;
					}
					format[0] = '\0';
					break;

				case 'n': /* pool name */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", scoreboard->pool[0] ? scoreboard->pool : "-");
					}
					break;

				case 'o': /* header output  */
					if (format[0] == '\0') {
						zlog(ZLOG_WARNING, "the name of the header must be set between embraces for %%%c", *s);
						return -1;
					}
					if (!test) {
						sapi_header_struct *h;
						zend_llist_position pos;
						sapi_headers_struct *sapi_headers = &SG(sapi_headers);
						size_t format_len = strlen(format);

						h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
						while (h) {
							char *header;
							if (!h->header_len) {
								h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
								continue;
							}
							if (!strstr(h->header, format)) {
								h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
								continue;
							}

							/* test if enought char after the header name + ': ' */
							if (h->header_len <= format_len + 2) {
								h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
								continue;
							}

							if (h->header[format_len] != ':' || h->header[format_len + 1] != ' ') {
								h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
								continue;
							}

							header = h->header + format_len + 2;
							len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", header && *header ? header : "-");

							/* found, done */
							break;
						}
						if (!len2) {
							len2 = 1;
							*b = '-';
						}
					}
					format[0] = '\0';
					break;

				case 'p': /* PID */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%ld", (long)getpid());
					}
					break;

				case 'P': /* PID */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%ld", (long)getppid());
					}
					break;

				case 'q': /* query_string */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", proc.query_string ? proc.query_string : "");
					}
					break;

				case 'Q': /* '?' */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", proc.query_string && *proc.query_string  ? "?" : "");
					}
					break;

				case 'r': /* request URI */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", proc.request_uri ? proc.request_uri : "-");
					}
					break;

				case 'R': /* remote IP address */
					if (!test) {
						char *tmp = fcgi_get_last_client_ip();
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", tmp ? tmp : "-");
					}
					break;

				case 's': /* status */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%d", SG(sapi_headers).http_response_code);
					}
					break;

				case 'T':
				case 't': /* time */
					if (!test) {
						time_t *t;
						if (*s == 't') {
							t = &proc.accepted_epoch;
						} else {
							t = &now_epoch;
						}
						if (format[0] == '\0') {
							strftime(tmp, sizeof(tmp) - 1, "%d/%b/%Y:%H:%M:%S %z", localtime(t));
						} else {
							strftime(tmp, sizeof(tmp) - 1, format, localtime(t));
						}
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", tmp);
					}
					format[0] = '\0';
					break;

				case 'u': /* remote user */
					if (!test) {
						len2 = snprintf(b, FPM_LOG_BUFFER - len, "%s", proc.auth_user ? proc.auth_user : "-");
					}
					break;

				case '{': /* complex var */
					token = 1;
					{
						char *start;
						size_t l;
						
						start = ++s;

						while (*s != '\0') {
							if (*s == '}') {
								l = s - start;

								if (l >= sizeof(format) - 1) {
									l = sizeof(format) - 1;
								}

								memcpy(format, start, l);
								format[l] = '\0';
								break;
							}
							s++;
						}
						if (s[1] == '\0') {
							zlog(ZLOG_WARNING, "missing closing embrace in the access.format");
							return -1;
						}
					}
					break;

				default:
					zlog(ZLOG_WARNING, "Invalid token in the access.format (%%%c)", *s);
					return -1;
			}

			if (*s != '}' && format[0] != '\0') {
				zlog(ZLOG_WARNING, "embrace is not allowed for modifier %%%c", *s);
				return -1;
			}
			s++;
			if (!test) {
				b += len2;
				len += len2;
			}
			continue;
		}

		if (!test) {
			// push the normal char to the output buffer
			*b = *s;
			b++;
			len++;
		}
		s++;
	}

	if (!test && strlen(buffer) > 0) {
		buffer[len] = '\n';
		write(fpm_log_fd, buffer, len + 1);
	}

	return 0;
}
/* }}} */
