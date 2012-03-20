void Init_golf(void);
#define ruby_options goruby_options
#define ruby_run_node goruby_run_node
#include "main.c"
#undef ruby_options
#undef ruby_run_node

#if defined _WIN32
#include <io.h>
#include <fcntl.h>
#define pipe(p) _pipe(p, 32L, _O_NOINHERIT)
#elif defined HAVE_UNISTD_H
#include <unistd.h>
#endif

RUBY_EXTERN void *ruby_options(int argc, char **argv);
RUBY_EXTERN int ruby_run_node(void*);
RUBY_EXTERN void ruby_init_ext(const char *name, void (*init)(void));

static VALUE
init_golf(VALUE arg)
{
    ruby_init_ext("golf", Init_golf);
    return arg;
}

void *
goruby_options(int argc, char **argv)
{
    static const char cmd[] = "require 'irb'\nIRB.start";
    int rw[2], infd;
    void *ret;

    if ((isatty(0) && isatty(1) && isatty(2)) && (pipe(rw) == 0)) {
	infd = dup(0);
	dup2(rw[0], 0);
	close(rw[0]);
	write(rw[1], cmd, sizeof(cmd) - 1);
	close(rw[1]);
	ret = ruby_options(argc, argv);
	dup2(infd, 0);
	close(infd);
	return ret;
    }
    else {
	return ruby_options(argc, argv);
    }
}

int
goruby_run_node(void *arg)
{
    int state;
    if (NIL_P(rb_protect(init_golf, Qtrue, &state))) {
	return state == EXIT_SUCCESS ? EXIT_FAILURE : state;
    }
    return ruby_run_node(arg);
}
