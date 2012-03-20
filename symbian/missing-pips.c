#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <pthreadtypes.h>

char **environ = 0;

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

int kill(pid_t pid, int sig);
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
int execl(const char *path, const char *arg0, ... /*, (char *)0 */);
int execv(const char *path, char *const argv[]);
int pthread_kill(pthread_t thread, int sig);

int kill(pid_t pid, int sig)
{
	return 0;
}

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset)
{
	return -1;
}

int execl(const char *path, const char *arg0, ...)
{
	return 0;
}

int execv(const char *path, char *const argv[])
{
	return 0;
}

int pthread_kill(pthread_t thread, int sig)
{
	return -1;
}


sighandler_t posix_signal(int signum, sighandler_t handler)
{
    return signal((signum),(handler));
}

int getrlimit(int resource, struct rlimit *rlp)
{
    return 0;
}

int setrlimit(int resource, const struct rlimit *rlp)
{
    return 0;
}

int getrusage(int who, struct rusage *r_usage)
{
    return 0;
}




