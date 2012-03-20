/**********************************************************************

  ruby.c -

  $Author$
  created at: Tue Aug 10 12:47:31 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#ifdef __CYGWIN__
#include <windows.h>
#include <sys/cygwin.h>
#endif
#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "internal.h"
#include "eval_intern.h"
#include "dln.h"
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>

#ifdef __hpux
#include <sys/pstat.h>
#endif
#if defined(LOAD_RELATIVE) && defined(HAVE_DLADDR)
#include <dlfcn.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#elif defined(HAVE_SYS_FCNTL_H)
#include <sys/fcntl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#include "ruby/util.h"

#ifndef HAVE_STDLIB_H
char *getenv();
#endif

#define DISABLE_BIT(bit) (1U << disable_##bit)
enum disable_flag_bits {
    disable_gems,
    disable_rubyopt,
    disable_flag_count
};

#define DUMP_BIT(bit) (1U << dump_##bit)
enum dump_flag_bits {
    dump_version,
    dump_copyright,
    dump_usage,
    dump_yydebug,
    dump_syntax,
    dump_parsetree,
    dump_parsetree_with_comment,
    dump_insns,
    dump_flag_count
};

struct cmdline_options {
    int sflag, xflag;
    int do_loop, do_print;
    int do_line, do_split;
    int do_search;
    unsigned int disable;
    int verbose;
    int safe_level;
    unsigned int setids;
    unsigned int dump;
    const char *script;
    VALUE script_name;
    VALUE e_script;
    struct {
	struct {
	    VALUE name;
	    int index;
	} enc;
    } src, ext, intern;
    VALUE req_list;
};

static void init_ids(struct cmdline_options *);

#define src_encoding_index GET_VM()->src_encoding_index

static struct cmdline_options *
cmdline_options_init(struct cmdline_options *opt)
{
    MEMZERO(opt, *opt, 1);
    init_ids(opt);
    opt->src.enc.index = src_encoding_index;
    opt->ext.enc.index = -1;
    opt->intern.enc.index = -1;
#if defined DISABLE_RUBYGEMS && DISABLE_RUBYGEMS
    opt->disable |= DISABLE_BIT(gems);
#endif
    return opt;
}

static NODE *load_file(VALUE, VALUE, int, struct cmdline_options *);
static void forbid_setid(const char *, struct cmdline_options *);
#define forbid_setid(s) forbid_setid((s), opt)

static struct {
    int argc;
    char **argv;
} origarg;

static void
usage(const char *name)
{
    /* This message really ought to be max 23 lines.
     * Removed -h because the user already knows that option. Others? */

    static const char *const usage_msg[] = {
	"-0[octal]       specify record separator (\\0, if no argument)",
	"-a              autosplit mode with -n or -p (splits $_ into $F)",
	"-c              check syntax only",
	"-Cdirectory     cd to directory, before executing your script",
	"-d              set debugging flags (set $DEBUG to true)",
	"-e 'command'    one line of script. Several -e's allowed. Omit [programfile]",
	"-Eex[:in]       specify the default external and internal character encodings",
	"-Fpattern       split() pattern for autosplit (-a)",
	"-i[extension]   edit ARGV files in place (make backup if extension supplied)",
	"-Idirectory     specify $LOAD_PATH directory (may be used more than once)",
	"-l              enable line ending processing",
	"-n              assume 'while gets(); ... end' loop around your script",
	"-p              assume loop like -n but print line also like sed",
	"-rlibrary       require the library, before executing your script",
	"-s              enable some switch parsing for switches after script name",
	"-S              look for the script using PATH environment variable",
	"-T[level=1]     turn on tainting checks",
	"-v              print version number, then turn on verbose mode",
	"-w              turn warnings on for your script",
	"-W[level=2]     set warning level; 0=silence, 1=medium, 2=verbose",
	"-x[directory]   strip off text before #!ruby line and perhaps cd to directory",
	"--copyright     print the copyright",
	"--version       print the version",
	NULL
    };
    const char *const *p = usage_msg;

    printf("Usage: %s [switches] [--] [programfile] [arguments]\n", name);
    while (*p)
	printf("  %s\n", *p++);
}

#ifdef MANGLED_PATH
static VALUE
rubylib_mangled_path(const char *s, unsigned int l)
{
    static char *newp, *oldp;
    static int newl, oldl, notfound;
    char *ptr;
    VALUE ret;

    if (!newp && !notfound) {
	newp = getenv("RUBYLIB_PREFIX");
	if (newp) {
	    oldp = newp = strdup(newp);
	    while (*newp && !ISSPACE(*newp) && *newp != ';') {
		newp = CharNext(newp);	/* Skip digits. */
	    }
	    oldl = newp - oldp;
	    while (*newp && (ISSPACE(*newp) || *newp == ';')) {
		newp = CharNext(newp);	/* Skip whitespace. */
	    }
	    newl = strlen(newp);
	    if (newl == 0 || oldl == 0) {
		rb_fatal("malformed RUBYLIB_PREFIX");
	    }
	    translit_char(newp, '\\', '/');
	}
	else {
	    notfound = 1;
	}
    }
    if (!newp || l < oldl || STRNCASECMP(oldp, s, oldl) != 0) {
	return rb_str_new(s, l);
    }
    ret = rb_str_new(0, l + newl - oldl);
    ptr = RSTRING_PTR(ret);
    memcpy(ptr, newp, newl);
    memcpy(ptr + newl, s + oldl, l - oldl);
    ptr[l + newl - oldl] = 0;
    return ret;
}
#else
#define rubylib_mangled_path rb_str_new
#endif

static void
push_include(const char *path, VALUE (*filter)(VALUE))
{
    const char sep = PATH_SEP_CHAR;
    const char *p, *s;
    VALUE load_path = GET_VM()->load_path;

    p = path;
    while (*p) {
	while (*p == sep)
	    p++;
	if (!*p) break;
	for (s = p; *s && *s != sep; s = CharNext(s));
	rb_ary_push(load_path, (*filter)(rubylib_mangled_path(p, s - p)));
	p = s;
    }
}

#ifdef __CYGWIN__
static void
push_include_cygwin(const char *path, VALUE (*filter)(VALUE))
{
    const char *p, *s;
    char rubylib[FILENAME_MAX];
    VALUE buf = 0;

    p = path;
    while (*p) {
	unsigned int len;
	while (*p == ';')
	    p++;
	if (!*p) break;
	for (s = p; *s && *s != ';'; s = CharNext(s));
	len = s - p;
	if (*s) {
	    if (!buf) {
		buf = rb_str_new(p, len);
		p = RSTRING_PTR(buf);
	    }
	    else {
		rb_str_resize(buf, len);
		p = strncpy(RSTRING_PTR(buf), p, len);
	    }
	}
#ifdef HAVE_CYGWIN_CONV_PATH
#define CONV_TO_POSIX_PATH(p, lib) \
	cygwin_conv_path(CCP_WIN_A_TO_POSIX|CCP_RELATIVE, (p), (lib), sizeof(lib))
#else
#define CONV_TO_POSIX_PATH(p, lib) \
	cygwin_conv_to_posix_path((p), (lib))
#endif
	if (CONV_TO_POSIX_PATH(p, rubylib) == 0)
	    p = rubylib;
	push_include(p, filter);
	if (!*s) break;
	p = s + 1;
    }
}

#define push_include push_include_cygwin
#endif

void
ruby_push_include(const char *path, VALUE (*filter)(VALUE))
{
    if (path == 0)
	return;
    push_include(path, filter);
}

static VALUE
identical_path(VALUE path)
{
    return path;
}
static VALUE
locale_path(VALUE path)
{
    rb_enc_associate(path, rb_locale_encoding());
    return path;
}

void
ruby_incpush(const char *path)
{
    ruby_push_include(path, locale_path);
}

static VALUE
expand_include_path(VALUE path)
{
    char *p = RSTRING_PTR(path);
    if (!p)
	return path;
    if (*p == '.' && p[1] == '/')
	return path;
    return rb_file_expand_path(path, Qnil);
}

void
ruby_incpush_expand(const char *path)
{
    ruby_push_include(path, expand_include_path);
}

#if defined _WIN32 || defined __CYGWIN__
static HMODULE libruby;

BOOL WINAPI
DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
	libruby = dll;
    return TRUE;
}

HANDLE
rb_libruby_handle(void)
{
    return libruby;
}
#endif

void ruby_init_loadpath_safe(int safe_level);

void
ruby_init_loadpath(void)
{
    ruby_init_loadpath_safe(0);
}

void
ruby_init_loadpath_safe(int safe_level)
{
    VALUE load_path;
    ID id_initial_load_path_mark;
    extern const char ruby_initial_load_paths[];
    const char *paths = ruby_initial_load_paths;
#if defined LOAD_RELATIVE
# if defined HAVE_DLADDR || defined HAVE_CYGWIN_CONV_PATH
#   define VARIABLE_LIBPATH 1
# else
#   define VARIABLE_LIBPATH 0
# endif
# if VARIABLE_LIBPATH
    char *libpath;
    VALUE sopath;
# else
    char libpath[MAXPATHLEN + 1];
# endif
    size_t baselen;
    char *p;

#if defined _WIN32 || defined __CYGWIN__
# if VARIABLE_LIBPATH
    sopath = rb_str_new(0, MAXPATHLEN);
    libpath = RSTRING_PTR(sopath);
    GetModuleFileName(libruby, libpath, MAXPATHLEN);
# else
    GetModuleFileName(libruby, libpath, sizeof libpath);
# endif
#elif defined(__EMX__)
    _execname(libpath, sizeof(libpath) - 1);
#elif defined(HAVE_DLADDR)
    Dl_info dli;
    if (dladdr((void *)(VALUE)expand_include_path, &dli)) {
	char fbuf[MAXPATHLEN];
	char *f = dln_find_file_r(dli.dli_fname, getenv(PATH_ENV), fbuf, sizeof(fbuf));
	VALUE fname = rb_str_new_cstr(f ? f : dli.dli_fname);
	rb_str_freeze(fname);
	sopath = rb_realpath_internal(Qnil, fname, 1);
    }
    else {
	sopath = rb_str_new(0, 0);
    }
    libpath = RSTRING_PTR(sopath);
#endif

#if !VARIABLE_LIBPATH
    libpath[sizeof(libpath) - 1] = '\0';
#endif
#if defined DOSISH
    translit_char(libpath, '\\', '/');
#elif defined __CYGWIN__
    {
# if VARIABLE_LIBPATH
	const int win_to_posix = CCP_WIN_A_TO_POSIX | CCP_RELATIVE;
	size_t newsize = cygwin_conv_path(win_to_posix, libpath, 0, 0);
	if (newsize > 0) {
	    VALUE rubylib = rb_str_new(0, newsize);
	    p = RSTRING_PTR(rubylib);
	    if (cygwin_conv_path(win_to_posix, libpath, p, newsize) == 0) {
		rb_str_resize(sopath, 0);
		sopath = rubylib;
		libpath = p;
	    }
	}
# else
	char rubylib[FILENAME_MAX];
	cygwin_conv_to_posix_path(libpath, rubylib);
	strncpy(libpath, rubylib, sizeof(libpath));
# endif
    }
#endif
    p = strrchr(libpath, '/');
    if (p) {
	*p = 0;
	if (p - libpath > 3 && !(STRCASECMP(p - 4, "/bin") && strcmp(p - 4, "/lib"))) {
	    p -= 4;
	    *p = 0;
	}
    }
#if !VARIABLE_LIBPATH
    else {
	strlcpy(libpath, ".", sizeof(libpath));
	p = libpath + 1;
    }
    baselen = p - libpath;
#define PREFIX_PATH() rb_str_new(libpath, baselen)
#else
    baselen = p - libpath;
    rb_str_resize(sopath, baselen);
    libpath = RSTRING_PTR(sopath);
#define PREFIX_PATH() sopath
#endif

#define BASEPATH() rb_str_buf_cat(rb_str_buf_new(baselen+len), libpath, baselen)

#define RUBY_RELATIVE(path, len) rb_str_buf_cat(BASEPATH(), (path), (len))
#else
    static const char exec_prefix[] = RUBY_EXEC_PREFIX;
#define RUBY_RELATIVE(path, len) rubylib_mangled_path((path), (len))
#define PREFIX_PATH() RUBY_RELATIVE(exec_prefix, sizeof(exec_prefix)-1)
#endif
    load_path = GET_VM()->load_path;

    if (safe_level == 0) {
#ifdef MANGLED_PATH
	rubylib_mangled_path("", 0);
#endif
	ruby_push_include(getenv("RUBYLIB"), identical_path);
    }

    id_initial_load_path_mark = rb_intern_const("@gem_prelude_index");
    while (*paths) {
	size_t len = strlen(paths);
	VALUE path = RUBY_RELATIVE(paths, len);
	rb_ivar_set(path, id_initial_load_path_mark, path);
	rb_ary_push(load_path, path);
	paths += len + 1;
    }

    rb_const_set(rb_cObject, rb_intern_const("TMP_RUBY_PREFIX"), rb_obj_freeze(PREFIX_PATH()));
}


static void
add_modules(VALUE *req_list, const char *mod)
{
    VALUE list = *req_list;

    if (!list) {
	*req_list = list = rb_ary_new();
	RBASIC(list)->klass = 0;
    }
    rb_ary_push(list, rb_obj_freeze(rb_str_new2(mod)));
}

static void
require_libraries(VALUE *req_list)
{
    VALUE list = *req_list;
    VALUE self = rb_vm_top_self();
    ID require;
    rb_thread_t *th = GET_THREAD();
    rb_block_t *prev_base_block = th->base_block;
    int prev_parse_in_eval = th->parse_in_eval;
    th->base_block = 0;
    th->parse_in_eval = 0;

    Init_ext();		/* should be called here for some reason :-( */
    CONST_ID(require, "require");
    while (list && RARRAY_LEN(list) > 0) {
	VALUE feature = rb_ary_shift(list);
	rb_funcall2(self, require, 1, &feature);
    }
    *req_list = 0;

    th->parse_in_eval = prev_parse_in_eval;
    th->base_block = prev_base_block;
}

static void
process_sflag(int *sflag)
{
    if (*sflag > 0) {
	long n;
	VALUE *args;
	VALUE argv = rb_argv;

	n = RARRAY_LEN(argv);
	args = RARRAY_PTR(argv);
	while (n > 0) {
	    VALUE v = *args++;
	    char *s = StringValuePtr(v);
	    char *p;
	    int hyphen = FALSE;

	    if (s[0] != '-')
		break;
	    n--;
	    if (s[1] == '-' && s[2] == '\0')
		break;

	    v = Qtrue;
	    /* check if valid name before replacing - with _ */
	    for (p = s + 1; *p; p++) {
		if (*p == '=') {
		    *p++ = '\0';
		    v = rb_str_new2(p);
		    break;
		}
		if (*p == '-') {
		    hyphen = TRUE;
		}
		else if (*p != '_' && !ISALNUM(*p)) {
		    VALUE name_error[2];
		    name_error[0] =
			rb_str_new2("invalid name for global variable - ");
		    if (!(p = strchr(p, '='))) {
			rb_str_cat2(name_error[0], s);
		    }
		    else {
			rb_str_cat(name_error[0], s, p - s);
		    }
		    name_error[1] = args[-1];
		    rb_exc_raise(rb_class_new_instance(2, name_error, rb_eNameError));
		}
	    }
	    s[0] = '$';
	    if (hyphen) {
		for (p = s + 1; *p; ++p) {
		    if (*p == '-')
			*p = '_';
		}
	    }
	    rb_gv_set(s, v);
	}
	n = RARRAY_LEN(argv) - n;
	while (n--) {
	    rb_ary_shift(argv);
	}
	*sflag = -1;
    }
}

static long proc_options(long argc, char **argv, struct cmdline_options *opt, int envopt);

static void
moreswitches(const char *s, struct cmdline_options *opt, int envopt)
{
    long argc, i, len;
    char **argv, *p;
    const char *ap = 0;
    VALUE argstr, argary;

    while (ISSPACE(*s)) s++;
    if (!*s) return;
    argstr = rb_str_tmp_new((len = strlen(s)) + 2);
    argary = rb_str_tmp_new(0);

    p = RSTRING_PTR(argstr);
    *p++ = ' ';
    memcpy(p, s, len + 1);
    ap = 0;
    rb_str_cat(argary, (char *)&ap, sizeof(ap));
    while (*p) {
	ap = p;
	rb_str_cat(argary, (char *)&ap, sizeof(ap));
	while (*p && !ISSPACE(*p)) ++p;
	if (!*p) break;
	*p++ = '\0';
	while (ISSPACE(*p)) ++p;
    }
    argc = RSTRING_LEN(argary) / sizeof(ap);
    ap = 0;
    rb_str_cat(argary, (char *)&ap, sizeof(ap));
    argv = (char **)RSTRING_PTR(argary);

    while ((i = proc_options(argc, argv, opt, envopt)) > 1 && (argc -= i) > 0) {
	argv += i;
	if (**argv != '-') {
	    *--*argv = '-';
	}
	if ((*argv)[1]) {
	    ++argc;
	    --argv;
	}
    }

    /* get rid of GC */
    rb_str_resize(argary, 0);
    rb_str_resize(argstr, 0);
}

#define NAME_MATCH_P(name, str, len) \
    ((len) < (int)sizeof(name) && strncmp((str), (name), (len)) == 0)

#define UNSET_WHEN(name, bit, str, len)	\
    if (NAME_MATCH_P((name), (str), (len))) { \
	*(unsigned int *)arg &= ~(bit); \
	return;				\
    }

#define SET_WHEN(name, bit, str, len)	\
    if (NAME_MATCH_P((name), (str), (len))) { \
	*(unsigned int *)arg |= (bit);	\
	return;				\
    }

static void
enable_option(const char *str, int len, void *arg)
{
#define UNSET_WHEN_DISABLE(bit) UNSET_WHEN(#bit, DISABLE_BIT(bit), str, len)
    UNSET_WHEN_DISABLE(gems);
    UNSET_WHEN_DISABLE(rubyopt);
    if (NAME_MATCH_P("all", str, len)) {
	*(unsigned int *)arg = 0U;
	return;
    }
    rb_warn("unknown argument for --enable: `%.*s'", len, str);
}

static void
disable_option(const char *str, int len, void *arg)
{
#define SET_WHEN_DISABLE(bit) SET_WHEN(#bit, DISABLE_BIT(bit), str, len)
    SET_WHEN_DISABLE(gems);
    SET_WHEN_DISABLE(rubyopt);
    if (NAME_MATCH_P("all", str, len)) {
	*(unsigned int *)arg = ~0U;
	return;
    }
    rb_warn("unknown argument for --disable: `%.*s'", len, str);
}

static void
dump_option(const char *str, int len, void *arg)
{
#define SET_WHEN_DUMP(bit) SET_WHEN(#bit, DUMP_BIT(bit), str, len)
    SET_WHEN_DUMP(version);
    SET_WHEN_DUMP(copyright);
    SET_WHEN_DUMP(usage);
    SET_WHEN_DUMP(yydebug);
    SET_WHEN_DUMP(syntax);
    SET_WHEN_DUMP(parsetree);
    SET_WHEN_DUMP(parsetree_with_comment);
    SET_WHEN_DUMP(insns);
    rb_warn("don't know how to dump `%.*s',", len, str);
    rb_warn("but only [version, copyright, usage, yydebug, syntax, parsetree, parsetree_with_comment, insns].");
}

static void
set_option_encoding_once(const char *type, VALUE *name, const char *e, long elen)
{
    VALUE ename;

    if (!elen) elen = strlen(e);
    ename = rb_str_new(e, elen);

    if (*name &&
	rb_funcall(ename, rb_intern("casecmp"), 1, *name) != INT2FIX(0)) {
	rb_raise(rb_eRuntimeError,
		 "%s already set to %s", type, RSTRING_PTR(*name));
    }
    *name = ename;
}

#define set_internal_encoding_once(opt, e, elen) \
    set_option_encoding_once("default_internal", &(opt)->intern.enc.name, (e), (elen))
#define set_external_encoding_once(opt, e, elen) \
    set_option_encoding_once("default_external", &(opt)->ext.enc.name, (e), (elen))
#define set_source_encoding_once(opt, e, elen) \
    set_option_encoding_once("source", &(opt)->src.enc.name, (e), (elen))

static long
proc_options(long argc, char **argv, struct cmdline_options *opt, int envopt)
{
    long n, argc0 = argc;
    const char *s;

    if (argc == 0)
	return 0;

    for (argc--, argv++; argc > 0; argc--, argv++) {
	const char *const arg = argv[0];
	if (arg[0] != '-' || !arg[1])
	    break;

	s = arg + 1;
      reswitch:
	switch (*s) {
	  case 'a':
	    if (envopt) goto noenvopt;
	    opt->do_split = TRUE;
	    s++;
	    goto reswitch;

	  case 'p':
	    if (envopt) goto noenvopt;
	    opt->do_print = TRUE;
	    /* through */
	  case 'n':
	    if (envopt) goto noenvopt;
	    opt->do_loop = TRUE;
	    s++;
	    goto reswitch;

	  case 'd':
	    ruby_debug = Qtrue;
	    ruby_verbose = Qtrue;
	    s++;
	    goto reswitch;

	  case 'y':
	    if (envopt) goto noenvopt;
	    opt->dump |= DUMP_BIT(yydebug);
	    s++;
	    goto reswitch;

	  case 'v':
	    if (opt->verbose) {
		s++;
		goto reswitch;
	    }
	    ruby_show_version();
	    opt->verbose = 1;
	  case 'w':
	    ruby_verbose = Qtrue;
	    s++;
	    goto reswitch;

	  case 'W':
	    {
		size_t numlen;
		int v = 2;	/* -W as -W2 */

		if (*++s) {
		    v = scan_oct(s, 1, &numlen);
		    if (numlen == 0)
			v = 1;
		    s += numlen;
		}
		switch (v) {
		  case 0:
		    ruby_verbose = Qnil;
		    break;
		  case 1:
		    ruby_verbose = Qfalse;
		    break;
		  default:
		    ruby_verbose = Qtrue;
		    break;
		}
	    }
	    goto reswitch;

	  case 'c':
	    if (envopt) goto noenvopt;
	    opt->dump |= DUMP_BIT(syntax);
	    s++;
	    goto reswitch;

	  case 's':
	    if (envopt) goto noenvopt;
	    forbid_setid("-s");
	    if (!opt->sflag) opt->sflag = 1;
	    s++;
	    goto reswitch;

	  case 'h':
	    if (envopt) goto noenvopt;
	    opt->dump |= DUMP_BIT(usage);
	    goto switch_end;

	  case 'l':
	    if (envopt) goto noenvopt;
	    opt->do_line = TRUE;
	    rb_output_rs = rb_rs;
	    s++;
	    goto reswitch;

	  case 'S':
	    if (envopt) goto noenvopt;
	    forbid_setid("-S");
	    opt->do_search = TRUE;
	    s++;
	    goto reswitch;

	  case 'e':
	    if (envopt) goto noenvopt;
	    forbid_setid("-e");
	    if (!*++s) {
		s = argv[1];
		argc--, argv++;
	    }
	    if (!s) {
		rb_raise(rb_eRuntimeError, "no code specified for -e");
	    }
	    if (!opt->e_script) {
		opt->e_script = rb_str_new(0, 0);
		if (opt->script == 0)
		    opt->script = "-e";
	    }
	    rb_str_cat2(opt->e_script, s);
	    rb_str_cat2(opt->e_script, "\n");
	    break;

	  case 'r':
	    forbid_setid("-r");
	    if (*++s) {
		add_modules(&opt->req_list, s);
	    }
	    else if (argv[1]) {
		add_modules(&opt->req_list, argv[1]);
		argc--, argv++;
	    }
	    break;

	  case 'i':
	    if (envopt) goto noenvopt;
	    forbid_setid("-i");
	    ruby_set_inplace_mode(s + 1);
	    break;

	  case 'x':
	    if (envopt) goto noenvopt;
	    opt->xflag = TRUE;
	    s++;
	    if (*s && chdir(s) < 0) {
		rb_fatal("Can't chdir to %s", s);
	    }
	    break;

	  case 'C':
	  case 'X':
	    if (envopt) goto noenvopt;
	    s++;
	    if (!*s) {
		s = argv[1];
		argc--, argv++;
	    }
	    if (!s || !*s) {
		rb_fatal("Can't chdir");
	    }
	    if (chdir(s) < 0) {
		rb_fatal("Can't chdir to %s", s);
	    }
	    break;

	  case 'F':
	    if (envopt) goto noenvopt;
	    if (*++s) {
		rb_fs = rb_reg_new(s, strlen(s), 0);
	    }
	    break;

	  case 'E':
	    if (!*++s && (!--argc || !(s = *++argv))) {
		rb_raise(rb_eRuntimeError, "missing argument for -E");
	    }
	    goto encoding;

	  case 'U':
	    set_internal_encoding_once(opt, "UTF-8", 0);
	    ++s;
	    goto reswitch;

	  case 'K':
	    if (*++s) {
		const char *enc_name = 0;
		switch (*s) {
		  case 'E': case 'e':
		    enc_name = "EUC-JP";
		    break;
		  case 'S': case 's':
		    enc_name = "Windows-31J";
		    break;
		  case 'U': case 'u':
		    enc_name = "UTF-8";
		    break;
		  case 'N': case 'n': case 'A': case 'a':
		    enc_name = "ASCII-8BIT";
		    break;
		}
		if (enc_name) {
		    opt->src.enc.name = rb_str_new2(enc_name);
		    if (!opt->ext.enc.name)
			opt->ext.enc.name = opt->src.enc.name;
		}
		s++;
	    }
	    goto reswitch;

	  case 'T':
	    {
		size_t numlen;
		int v = 1;

		if (*++s) {
		    v = scan_oct(s, 2, &numlen);
		    if (numlen == 0)
			v = 1;
		    s += numlen;
		}
		if (v > opt->safe_level) opt->safe_level = v;
	    }
	    goto reswitch;

	  case 'I':
	    forbid_setid("-I");
	    if (*++s)
		ruby_incpush_expand(s);
	    else if (argv[1]) {
		ruby_incpush_expand(argv[1]);
		argc--, argv++;
	    }
	    break;

	  case '0':
	    if (envopt) goto noenvopt;
	    {
		size_t numlen;
		int v;
		char c;

		v = scan_oct(s, 4, &numlen);
		s += numlen;
		if (v > 0377)
		    rb_rs = Qnil;
		else if (v == 0 && numlen >= 2) {
		    rb_rs = rb_str_new2("\n\n");
		}
		else {
		    c = v & 0xff;
		    rb_rs = rb_str_new(&c, 1);
		}
	    }
	    goto reswitch;

	  case '-':
	    if (!s[1] || (s[1] == '\r' && !s[2])) {
		argc--, argv++;
		goto switch_end;
	    }
	    s++;

#	define is_option_end(c, allow_hyphen) \
	    (!(c) || ((allow_hyphen) && (c) == '-') || (c) == '=')
#	define check_envopt(name, allow_envopt) \
	    (((allow_envopt) || !envopt) ? (void)0 : \
	     rb_raise(rb_eRuntimeError, "invalid switch in RUBYOPT: --" name))
#	define need_argument(name, s) \
	    ((*(s)++ ? !*(s) : (!--argc || !((s) = *++argv))) ?		\
	     rb_raise(rb_eRuntimeError, "missing argument for --" name) \
	     : (void)0)
#	define is_option_with_arg(name, allow_hyphen, allow_envopt) \
	    (strncmp((name), s, n = sizeof(name) - 1) == 0 && is_option_end(s[n], (allow_hyphen)) ? \
	     (check_envopt(name, (allow_envopt)), s += n, need_argument(name, s), 1) : 0)

	    if (strcmp("copyright", s) == 0) {
		if (envopt) goto noenvopt_long;
		opt->dump |= DUMP_BIT(copyright);
	    }
	    else if (strcmp("debug", s) == 0) {
		ruby_debug = Qtrue;
                ruby_verbose = Qtrue;
            }
	    else if (is_option_with_arg("enable", Qtrue, Qtrue)) {
		ruby_each_words(s, enable_option, &opt->disable);
	    }
	    else if (is_option_with_arg("disable", Qtrue, Qtrue)) {
		ruby_each_words(s, disable_option, &opt->disable);
	    }
	    else if (is_option_with_arg("encoding", Qfalse, Qtrue)) {
		char *p;
	      encoding:
		do {
#	define set_encoding_part(type) \
		    if (!(p = strchr(s, ':'))) { \
			set_##type##_encoding_once(opt, s, 0); \
			break; \
		    } \
		    else if (p > s) { \
			set_##type##_encoding_once(opt, s, p-s); \
		    }
		    set_encoding_part(external);
		    if (!*(s = ++p)) break;
		    set_encoding_part(internal);
		    if (!*(s = ++p)) break;
#if defined ALLOW_DEFAULT_SOURCE_ENCODING && ALLOW_DEFAULT_SOURCE_ENCODING
		    set_encoding_part(source);
		    if (!*(s = ++p)) break;
#endif
		    rb_raise(rb_eRuntimeError, "extra argument for %s: %s",
			     (arg[1] == '-' ? "--encoding" : "-E"), s);
#	undef set_encoding_part
		} while (0);
	    }
	    else if (is_option_with_arg("internal-encoding", Qfalse, Qtrue)) {
		set_internal_encoding_once(opt, s, 0);
	    }
	    else if (is_option_with_arg("external-encoding", Qfalse, Qtrue)) {
		set_external_encoding_once(opt, s, 0);
	    }
#if defined ALLOW_DEFAULT_SOURCE_ENCODING && ALLOW_DEFAULT_SOURCE_ENCODING
	    else if (is_option_with_arg("source-encoding", Qfalse, Qtrue)) {
		set_source_encoding_once(opt, s, 0);
	    }
#endif
	    else if (strcmp("version", s) == 0) {
		if (envopt) goto noenvopt_long;
		opt->dump |= DUMP_BIT(version);
	    }
	    else if (strcmp("verbose", s) == 0) {
		opt->verbose = 1;
		ruby_verbose = Qtrue;
	    }
	    else if (strcmp("yydebug", s) == 0) {
		if (envopt) goto noenvopt_long;
		opt->dump |= DUMP_BIT(yydebug);
	    }
	    else if (is_option_with_arg("dump", Qfalse, Qfalse)) {
		ruby_each_words(s, dump_option, &opt->dump);
	    }
	    else if (strcmp("help", s) == 0) {
		if (envopt) goto noenvopt_long;
		opt->dump |= DUMP_BIT(usage);
		goto switch_end;
	    }
	    else {
		rb_raise(rb_eRuntimeError,
			 "invalid option --%s  (-h will show valid options)", s);
	    }
	    break;

	  case '\r':
	    if (!s[1])
		break;

	  default:
	    {
		if (ISPRINT(*s)) {
                    rb_raise(rb_eRuntimeError,
			"invalid option -%c  (-h will show valid options)",
                        (int)(unsigned char)*s);
		}
		else {
                    rb_raise(rb_eRuntimeError,
			"invalid option -\\x%02X  (-h will show valid options)",
                        (int)(unsigned char)*s);
		}
	    }
	    goto switch_end;

	  noenvopt:
	    /* "EIdvwWrKU" only */
	    rb_raise(rb_eRuntimeError, "invalid switch in RUBYOPT: -%c", *s);
	    break;

	  noenvopt_long:
	    rb_raise(rb_eRuntimeError, "invalid switch in RUBYOPT: --%s", s);
	    break;

	  case 0:
	    break;
#	undef is_option_end
#	undef check_envopt
#	undef need_argument
#	undef is_option_with_arg
	}
    }

  switch_end:
    return argc0 - argc;
}

static void
ruby_init_prelude(void)
{
    Init_prelude();
    rb_const_remove(rb_cObject, rb_intern_const("TMP_RUBY_PREFIX"));
}

static int
opt_enc_index(VALUE enc_name)
{
    const char *s = RSTRING_PTR(enc_name);
    int i = rb_enc_find_index(s);

    if (i < 0) {
	rb_raise(rb_eRuntimeError, "unknown encoding name - %s", s);
    }
    else if (rb_enc_dummy_p(rb_enc_from_index(i))) {
	rb_raise(rb_eRuntimeError, "dummy encoding is not acceptable - %s ", s);
    }
    return i;
}

#define rb_progname (GET_VM()->progname)
VALUE rb_argv0;

static VALUE
false_value(void)
{
    return Qfalse;
}

static VALUE
true_value(void)
{
    return Qtrue;
}

#define rb_define_readonly_boolean(name, val) \
    rb_define_virtual_variable((name), (val) ? true_value : false_value, 0)

static VALUE
uscore_get(void)
{
    VALUE line;

    line = rb_lastline_get();
    if (!RB_TYPE_P(line, T_STRING)) {
	rb_raise(rb_eTypeError, "$_ value need to be String (%s given)",
		 NIL_P(line) ? "nil" : rb_obj_classname(line));
    }
    return line;
}

/*
 *  call-seq:
 *     sub(pattern, replacement)   -> $_
 *     sub(pattern) { block }      -> $_
 *
 *  Equivalent to <code>$_.sub(<i>args</i>)</code>, except that
 *  <code>$_</code> will be updated if substitution occurs.
 *  Available only when -p/-n command line option specified.
 */

static VALUE
rb_f_sub(argc, argv)
    int argc;
    VALUE *argv;
{
    VALUE str = rb_funcall3(uscore_get(), rb_intern("sub"), argc, argv);
    rb_lastline_set(str);
    return str;
}

/*
 *  call-seq:
 *     gsub(pattern, replacement)    -> string
 *     gsub(pattern) {|...| block }  -> string
 *
 *  Equivalent to <code>$_.gsub...</code>, except that <code>$_</code>
 *  receives the modified result.
 *  Available only when -p/-n command line option specified.
 *
 */

static VALUE
rb_f_gsub(argc, argv)
    int argc;
    VALUE *argv;
{
    VALUE str = rb_funcall3(uscore_get(), rb_intern("gsub"), argc, argv);
    rb_lastline_set(str);
    return str;
}

/*
 *  call-seq:
 *     chop   -> string
 *
 *  Equivalent to <code>($_.dup).chop!</code>, except <code>nil</code>
 *  is never returned. See <code>String#chop!</code>.
 *  Available only when -p/-n command line option specified.
 *
 */

static VALUE
rb_f_chop(void)
{
    VALUE str = rb_funcall3(uscore_get(), rb_intern("chop"), 0, 0);
    rb_lastline_set(str);
    return str;
}


/*
 *  call-seq:
 *     chomp            -> $_
 *     chomp(string)    -> $_
 *
 *  Equivalent to <code>$_ = $_.chomp(<em>string</em>)</code>. See
 *  <code>String#chomp</code>.
 *  Available only when -p/-n command line option specified.
 *
 */

static VALUE
rb_f_chomp(argc, argv)
    int argc;
    VALUE *argv;
{
    VALUE str = rb_funcall3(uscore_get(), rb_intern("chomp"), argc, argv);
    rb_lastline_set(str);
    return str;
}

static VALUE
process_options(int argc, char **argv, struct cmdline_options *opt)
{
    NODE *tree = 0;
    VALUE parser;
    VALUE iseq;
    rb_encoding *enc, *lenc;
    const char *s;
    char fbuf[MAXPATHLEN];
    int i = (int)proc_options(argc, argv, opt, 0);
    rb_thread_t *th = GET_THREAD();
    rb_env_t *env = 0;

    argc -= i;
    argv += i;

    if (opt->dump & DUMP_BIT(usage)) {
	usage(origarg.argv[0]);
	return Qtrue;
    }

    if (!(opt->disable & DISABLE_BIT(rubyopt)) &&
	opt->safe_level == 0 && (s = getenv("RUBYOPT"))) {
	VALUE src_enc_name = opt->src.enc.name;
	VALUE ext_enc_name = opt->ext.enc.name;
	VALUE int_enc_name = opt->intern.enc.name;

	opt->src.enc.name = opt->ext.enc.name = opt->intern.enc.name = 0;
	moreswitches(s, opt, 1);
	if (src_enc_name)
	    opt->src.enc.name = src_enc_name;
	if (ext_enc_name)
	    opt->ext.enc.name = ext_enc_name;
	if (int_enc_name)
	    opt->intern.enc.name = int_enc_name;
    }

    if (opt->dump & DUMP_BIT(version)) {
	ruby_show_version();
	return Qtrue;
    }
    if (opt->dump & DUMP_BIT(copyright)) {
	ruby_show_copyright();
    }

    if (opt->safe_level >= 4) {
	OBJ_TAINT(rb_argv);
	OBJ_TAINT(GET_VM()->load_path);
    }

    if (!opt->e_script) {
	if (argc == 0) {	/* no more args */
	    if (opt->verbose)
		return Qtrue;
	    opt->script = "-";
	}
	else {
	    opt->script = argv[0];
	    if (opt->script[0] == '\0') {
		opt->script = "-";
	    }
	    else if (opt->do_search) {
		char *path = getenv("RUBYPATH");

		opt->script = 0;
		if (path) {
		    opt->script = dln_find_file_r(argv[0], path, fbuf, sizeof(fbuf));
		}
		if (!opt->script) {
		    opt->script = dln_find_file_r(argv[0], getenv(PATH_ENV), fbuf, sizeof(fbuf));
		}
		if (!opt->script)
		    opt->script = argv[0];
	    }
	    argc--;
	    argv++;
	}
    }

    opt->script_name = rb_str_new_cstr(opt->script);
    opt->script = RSTRING_PTR(opt->script_name);
#if defined DOSISH || defined __CYGWIN__
    translit_char(RSTRING_PTR(opt->script_name), '\\', '/');
#endif

    ruby_init_loadpath_safe(opt->safe_level);
    rb_enc_find_index("encdb");
    lenc = rb_locale_encoding();
    rb_enc_associate(rb_progname, lenc);
    rb_obj_freeze(rb_progname);
    parser = rb_parser_new();
    if (opt->dump & DUMP_BIT(yydebug)) {
	rb_parser_set_yydebug(parser, Qtrue);
    }
    if (opt->ext.enc.name != 0) {
	opt->ext.enc.index = opt_enc_index(opt->ext.enc.name);
    }
    if (opt->intern.enc.name != 0) {
	opt->intern.enc.index = opt_enc_index(opt->intern.enc.name);
    }
    if (opt->src.enc.name != 0) {
	opt->src.enc.index = opt_enc_index(opt->src.enc.name);
	src_encoding_index = opt->src.enc.index;
    }
    if (opt->ext.enc.index >= 0) {
	enc = rb_enc_from_index(opt->ext.enc.index);
    }
    else {
	enc = lenc;
    }
    rb_enc_set_default_external(rb_enc_from_encoding(enc));
    if (opt->intern.enc.index >= 0) {
	enc = rb_enc_from_index(opt->intern.enc.index);
	rb_enc_set_default_internal(rb_enc_from_encoding(enc));
	opt->intern.enc.index = -1;
    }
    rb_enc_associate(opt->script_name, lenc);
    rb_obj_freeze(opt->script_name);
    {
	long i;
	VALUE load_path = GET_VM()->load_path;
	for (i = 0; i < RARRAY_LEN(load_path); ++i) {
	    rb_enc_associate(RARRAY_PTR(load_path)[i], lenc);
	}
    }
    if (!(opt->disable & DISABLE_BIT(gems))) {
#if defined DISABLE_RUBYGEMS && DISABLE_RUBYGEMS
	rb_require("rubygems");
#else
	rb_define_module("Gem");
#endif
    }
    ruby_init_prelude();
    ruby_set_argv(argc, argv);
    process_sflag(&opt->sflag);

    {
	/* set eval context */
	VALUE toplevel_binding = rb_const_get(rb_cObject, rb_intern("TOPLEVEL_BINDING"));
	rb_binding_t *bind;

	GetBindingPtr(toplevel_binding, bind);
	GetEnvPtr(bind->env, env);
    }

#define PREPARE_PARSE_MAIN(expr) do { \
    th->parse_in_eval--; \
    th->base_block = &env->block; \
    expr; \
    th->parse_in_eval++; \
    th->base_block = 0; \
} while (0)

    if (opt->e_script) {
	VALUE progname = rb_progname;
	rb_encoding *eenc;
	if (opt->src.enc.index >= 0) {
	    eenc = rb_enc_from_index(opt->src.enc.index);
	}
	else {
	    eenc = lenc;
	}
	rb_enc_associate(opt->e_script, eenc);
	rb_vm_set_progname(rb_progname = opt->script_name);
	require_libraries(&opt->req_list);
	rb_vm_set_progname(rb_progname = progname);

	PREPARE_PARSE_MAIN({
	    tree = rb_parser_compile_string(parser, opt->script, opt->e_script, 1);
	});
    }
    else {
	if (opt->script[0] == '-' && !opt->script[1]) {
	    forbid_setid("program input from stdin");
	}

	PREPARE_PARSE_MAIN({
	    tree = load_file(parser, opt->script_name, 1, opt);
	});
    }
    rb_progname = opt->script_name;
    rb_vm_set_progname(rb_progname);
    if (opt->dump & DUMP_BIT(yydebug)) return Qtrue;

    if (opt->ext.enc.index >= 0) {
	enc = rb_enc_from_index(opt->ext.enc.index);
    }
    else {
	enc = lenc;
    }
    rb_enc_set_default_external(rb_enc_from_encoding(enc));
    if (opt->intern.enc.index >= 0) {
	/* Set in the shebang line */
	enc = rb_enc_from_index(opt->intern.enc.index);
	rb_enc_set_default_internal(rb_enc_from_encoding(enc));
    }
    else if (!rb_default_internal_encoding())
	/* Freeze default_internal */
	rb_enc_set_default_internal(Qnil);
    rb_stdio_set_default_encoding();

    if (!tree) return Qfalse;

    process_sflag(&opt->sflag);
    opt->xflag = 0;

    if (opt->safe_level >= 4) {
	FL_UNSET(rb_argv, FL_TAINT);
	FL_UNSET(GET_VM()->load_path, FL_TAINT);
    }

    if (opt->dump & DUMP_BIT(syntax)) {
	printf("Syntax OK\n");
	return Qtrue;
    }

    if (opt->do_print) {
	PREPARE_PARSE_MAIN({
	    tree = rb_parser_append_print(parser, tree);
	});
    }
    if (opt->do_loop) {
	PREPARE_PARSE_MAIN({
	    tree = rb_parser_while_loop(parser, tree, opt->do_line, opt->do_split);
	});
	rb_define_global_function("sub", rb_f_sub, -1);
	rb_define_global_function("gsub", rb_f_gsub, -1);
	rb_define_global_function("chop", rb_f_chop, 0);
	rb_define_global_function("chomp", rb_f_chomp, -1);
    }

    if (opt->dump & DUMP_BIT(parsetree) || opt->dump & DUMP_BIT(parsetree_with_comment)) {
	rb_io_write(rb_stdout, rb_parser_dump_tree(tree, opt->dump & DUMP_BIT(parsetree_with_comment)));
	rb_io_flush(rb_stdout);
	return Qtrue;
    }

    PREPARE_PARSE_MAIN({
	VALUE path = Qnil;
	if (!opt->e_script && strcmp(opt->script, "-"))
	    path = rb_realpath_internal(Qnil, opt->script_name, 1);
	iseq = rb_iseq_new_main(tree, opt->script_name, path);
    });

    if (opt->dump & DUMP_BIT(insns)) {
	rb_io_write(rb_stdout, rb_iseq_disasm(iseq));
	rb_io_flush(rb_stdout);
	return Qtrue;
    }

    rb_define_readonly_boolean("$-p", opt->do_print);
    rb_define_readonly_boolean("$-l", opt->do_line);
    rb_define_readonly_boolean("$-a", opt->do_split);

    rb_set_safe_level(opt->safe_level);
    rb_gc_set_params();

    return iseq;
}

struct load_file_arg {
    VALUE parser;
    VALUE fname;
    int script;
    struct cmdline_options *opt;
};

static VALUE
load_file_internal(VALUE arg)
{
    extern VALUE rb_stdin;
    struct load_file_arg *argp = (struct load_file_arg *)arg;
    VALUE parser = argp->parser;
    VALUE fname_v = rb_str_encode_ospath(argp->fname);
    const char *fname = StringValueCStr(fname_v);
    int script = argp->script;
    struct cmdline_options *opt = argp->opt;
    VALUE f;
    int line_start = 1;
    NODE *tree = 0;
    rb_encoding *enc;
    ID set_encoding;
    int xflag = 0;

    if (strcmp(fname, "-") == 0) {
	f = rb_stdin;
    }
    else {
	int fd, mode = O_RDONLY;
#if defined DOSISH || defined __CYGWIN__
	{
	    const char *ext = strrchr(fname, '.');
	    if (ext && STRCASECMP(ext, ".exe") == 0) {
		mode |= O_BINARY;
		xflag = 1;
	    }
	}
#endif
	if ((fd = rb_cloexec_open(fname, mode, 0)) < 0) {
	    rb_load_fail(fname_v, strerror(errno));
	}
        rb_update_max_fd(fd);

	f = rb_io_fdopen(fd, mode, fname);
    }

    CONST_ID(set_encoding, "set_encoding");
    if (script) {
	VALUE c = 1;		/* something not nil */
	VALUE line;
	char *p;
	int no_src_enc = !opt->src.enc.name;
	int no_ext_enc = !opt->ext.enc.name;
	int no_int_enc = !opt->intern.enc.name;

	enc = rb_ascii8bit_encoding();
	rb_funcall(f, set_encoding, 1, rb_enc_from_encoding(enc));

	if (xflag || opt->xflag) {
	    line_start--;
	  search_shebang:
	    forbid_setid("-x");
	    opt->xflag = FALSE;
	    while (!NIL_P(line = rb_io_gets(f))) {
		line_start++;
		if (RSTRING_LEN(line) > 2
		    && RSTRING_PTR(line)[0] == '#'
		    && RSTRING_PTR(line)[1] == '!') {
		    if ((p = strstr(RSTRING_PTR(line), "ruby")) != 0) {
			goto start_read;
		    }
		}
	    }
	    rb_loaderror("no Ruby script found in input");
	}

	c = rb_io_getbyte(f);
	if (c == INT2FIX('#')) {
	    c = rb_io_getbyte(f);
	    if (c == INT2FIX('!')) {
		line = rb_io_gets(f);
		if (NIL_P(line))
		    return 0;

		if ((p = strstr(RSTRING_PTR(line), "ruby")) == 0) {
		    /* not ruby script, assume -x flag */
		    goto search_shebang;
		}

	      start_read:
		p += 4;
		RSTRING_PTR(line)[RSTRING_LEN(line) - 1] = '\0';
		if (RSTRING_PTR(line)[RSTRING_LEN(line) - 2] == '\r')
		    RSTRING_PTR(line)[RSTRING_LEN(line) - 2] = '\0';
		if ((p = strstr(p, " -")) != 0) {
		    moreswitches(p + 1, opt, 0);
		}

		/* push back shebang for pragma may exist in next line */
		rb_io_ungetbyte(f, rb_str_new2("!\n"));
	    }
	    else if (!NIL_P(c)) {
		rb_io_ungetbyte(f, c);
	    }
	    rb_io_ungetbyte(f, INT2FIX('#'));
	    if (no_src_enc && opt->src.enc.name) {
		opt->src.enc.index = opt_enc_index(opt->src.enc.name);
		src_encoding_index = opt->src.enc.index;
	    }
	    if (no_ext_enc && opt->ext.enc.name) {
		opt->ext.enc.index = opt_enc_index(opt->ext.enc.name);
	    }
	    if (no_int_enc && opt->intern.enc.name) {
		opt->intern.enc.index = opt_enc_index(opt->intern.enc.name);
	    }
	}
	else if (!NIL_P(c)) {
	    rb_io_ungetbyte(f, c);
	}
	else {
	    if (f != rb_stdin) rb_io_close(f);
	    f = Qnil;
	}
	rb_vm_set_progname(rb_progname = opt->script_name);
	require_libraries(&opt->req_list);	/* Why here? unnatural */
    }
    if (opt->src.enc.index >= 0) {
	enc = rb_enc_from_index(opt->src.enc.index);
    }
    else if (f == rb_stdin) {
	enc = rb_locale_encoding();
    }
    else {
	enc = rb_usascii_encoding();
    }
    if (NIL_P(f)) {
	f = rb_str_new(0, 0);
	rb_enc_associate(f, enc);
	return (VALUE)rb_parser_compile_string(parser, fname, f, line_start);
    }
    rb_funcall(f, set_encoding, 2, rb_enc_from_encoding(enc), rb_str_new_cstr("-"));
    tree = rb_parser_compile_file(parser, fname, f, line_start);
    rb_funcall(f, set_encoding, 1, rb_parser_encoding(parser));
    if (script && tree && rb_parser_end_seen_p(parser)) {
	/*
	 * DATA is a File that contains the data section of the executed file.
	 * To create a data section use <tt>__END__</tt>:
	 *
	 *   $ cat t.rb
	 *   puts DATA.gets
	 *   __END__
	 *   hello world!
	 *
	 *   $ ruby t.rb
	 *   hello world!
	 */
	rb_define_global_const("DATA", f);
    }
    else if (f != rb_stdin) {
	rb_io_close(f);
    }
    return (VALUE)tree;
}

static VALUE
restore_lineno(VALUE lineno)
{
    return rb_gv_set("$.", lineno);
}

static NODE *
load_file(VALUE parser, VALUE fname, int script, struct cmdline_options *opt)
{
    struct load_file_arg arg;
    arg.parser = parser;
    arg.fname = fname;
    arg.script = script;
    arg.opt = opt;
    return (NODE *)rb_ensure(load_file_internal, (VALUE)&arg, restore_lineno, rb_gv_get("$."));
}

void *
rb_load_file(const char *fname)
{
    struct cmdline_options opt;
    VALUE fname_v = rb_str_new_cstr(fname);

    return load_file(rb_parser_new(), fname_v, 0, cmdline_options_init(&opt));
}

static void
set_arg0(VALUE val, ID id)
{
    char *s;
    long i;

    if (origarg.argv == 0)
	rb_raise(rb_eRuntimeError, "$0 not initialized");
    StringValue(val);
    s = RSTRING_PTR(val);
    i = RSTRING_LEN(val);

    setproctitle("%.*s", (int)i, s);

    rb_progname = rb_obj_freeze(rb_external_str_new(s, i));
}

void
ruby_script(const char *name)
{
    if (name) {
	rb_progname = rb_external_str_new(name, strlen(name));
	rb_vm_set_progname(rb_progname);
    }
}

static void
init_ids(struct cmdline_options *opt)
{
    rb_uid_t uid = getuid();
    rb_uid_t euid = geteuid();
    rb_gid_t gid = getgid();
    rb_gid_t egid = getegid();

    if (uid != euid) opt->setids |= 1;
    if (egid != gid) opt->setids |= 2;
    if (uid && opt->setids) {
	if (opt->safe_level < 1) opt->safe_level = 1;
    }
}

#undef forbid_setid
static void
forbid_setid(const char *s, struct cmdline_options *opt)
{
    if (opt->setids & 1)
        rb_raise(rb_eSecurityError, "no %s allowed while running setuid", s);
    if (opt->setids & 2)
        rb_raise(rb_eSecurityError, "no %s allowed while running setgid", s);
    if (opt->safe_level > 0)
        rb_raise(rb_eSecurityError, "no %s allowed in tainted mode", s);
}

static void
verbose_setter(VALUE val, ID id, void *data)
{
    VALUE *variable = data;
    *variable = RTEST(val) ? Qtrue : val;
}

static VALUE
opt_W_getter(ID id, void *data)
{
    VALUE *variable = data;
    switch (*variable) {
      case Qnil:
	return INT2FIX(0);
      case Qfalse:
	return INT2FIX(1);
      case Qtrue:
	return INT2FIX(2);
    }
    return Qnil;		/* not reached */
}

void
ruby_prog_init(void)
{
    rb_define_hooked_variable("$VERBOSE", &ruby_verbose, 0, verbose_setter);
    rb_define_hooked_variable("$-v", &ruby_verbose, 0, verbose_setter);
    rb_define_hooked_variable("$-w", &ruby_verbose, 0, verbose_setter);
    rb_define_hooked_variable("$-W", &ruby_verbose, opt_W_getter, rb_gvar_readonly_setter);
    rb_define_variable("$DEBUG", &ruby_debug);
    rb_define_variable("$-d", &ruby_debug);

    rb_define_hooked_variable("$0", &rb_progname, 0, set_arg0);
    rb_define_hooked_variable("$PROGRAM_NAME", &rb_progname, 0, set_arg0);

    /*
     * ARGV contains the command line arguments used to run ruby with the
     * first value containing the name of the executable.
     *
     * A library like OptionParser can be used to process command-line
     * arguments.
     */
    rb_define_global_const("ARGV", rb_argv);
}

void
ruby_set_argv(int argc, char **argv)
{
    int i;
    VALUE av = rb_argv;

#if defined(USE_DLN_A_OUT)
    if (origarg.argv)
	dln_argv0 = origarg.argv[0];
    else
	dln_argv0 = argv[0];
#endif
    rb_ary_clear(av);
    for (i = 0; i < argc; i++) {
	VALUE arg = rb_external_str_new_cstr(argv[i]);

	OBJ_FREEZE(arg);
	rb_ary_push(av, arg);
    }
}

void *
ruby_process_options(int argc, char **argv)
{
    struct cmdline_options opt;
    VALUE iseq;

    ruby_script(argv[0]);  /* for the time being */
    rb_argv0 = rb_str_new4(rb_progname);
    rb_gc_register_mark_object(rb_argv0);
    iseq = process_options(argc, argv, cmdline_options_init(&opt));

#ifndef HAVE_SETPROCTITLE
    {
	extern void ruby_init_setproctitle(int argc, char *argv[]);
	ruby_init_setproctitle(argc, argv);
    }
#endif

    return (void*)(struct RData*)iseq;
}

static void
fill_standard_fds(void)
{
    int f0, f1, f2, fds[2];
    struct stat buf;
    f0 = fstat(0, &buf) == -1 && errno == EBADF;
    f1 = fstat(1, &buf) == -1 && errno == EBADF;
    f2 = fstat(2, &buf) == -1 && errno == EBADF;
    if (f0) {
        if (pipe(fds) == 0) {
            close(fds[1]);
            if (fds[0] != 0) {
                dup2(fds[0], 0);
                close(fds[0]);
            }
        }
    }
    if (f1 || f2) {
        if (pipe(fds) == 0) {
            close(fds[0]);
            if (f1 && fds[1] != 1)
                dup2(fds[1], 1);
            if (f2 && fds[1] != 2)
                dup2(fds[1], 2);
            if (fds[1] != 1 && fds[1] != 2)
                close(fds[1]);
        }
    }
}

void
ruby_sysinit(int *argc, char ***argv)
{
#if defined(_WIN32)
    void rb_w32_sysinit(int *argc, char ***argv);
    rb_w32_sysinit(argc, argv);
#endif
    origarg.argc = *argc;
    origarg.argv = *argv;
#if defined(USE_DLN_A_OUT)
    dln_argv0 = origarg.argv[0];
#endif
    fill_standard_fds();
}
