/************************************************

  etc.c -

  $Author$
  created at: Tue Mar 22 18:39:19 JST 1994

************************************************/

#include "ruby.h"
#include "ruby/encoding.h"

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETPWENT
#include <pwd.h>
#endif

#ifdef HAVE_GETGRENT
#include <grp.h>
#endif

static VALUE sPasswd;
#ifdef HAVE_GETGRENT
static VALUE sGroup;
#endif

#ifdef _WIN32
#include <shlobj.h>
#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA 35
#endif
#endif

#ifndef _WIN32
char *getenv();
#endif
char *getlogin();

/* Returns the short user name of the currently logged in user.
 * Unfortunately, it is often rather easy to fool getlogin().
 * Avoid getlogin() for security-related purposes.
 *
 * e.g.
 *   Etc.getlogin -> 'guest'
 */
static VALUE
etc_getlogin(VALUE obj)
{
    char *login;

    rb_secure(4);
#ifdef HAVE_GETLOGIN
    login = getlogin();
    if (!login) login = getenv("USER");
#else
    login = getenv("USER");
#endif

    if (login)
	return rb_tainted_str_new2(login);
    return Qnil;
}

#if defined(HAVE_GETPWENT) || defined(HAVE_GETGRENT)
static VALUE
safe_setup_str(const char *str)
{
    if (str == 0) str = "";
    return rb_tainted_str_new2(str);
}
#endif

#ifdef HAVE_GETPWENT
static VALUE
setup_passwd(struct passwd *pwd)
{
    if (pwd == 0) rb_sys_fail("/etc/passwd");
    return rb_struct_new(sPasswd,
			 safe_setup_str(pwd->pw_name),
#ifdef HAVE_ST_PW_PASSWD
			 safe_setup_str(pwd->pw_passwd),
#endif
			 UIDT2NUM(pwd->pw_uid),
			 GIDT2NUM(pwd->pw_gid),
#ifdef HAVE_ST_PW_GECOS
			 safe_setup_str(pwd->pw_gecos),
#endif
			 safe_setup_str(pwd->pw_dir),
			 safe_setup_str(pwd->pw_shell),
#ifdef HAVE_ST_PW_CHANGE
			 INT2NUM(pwd->pw_change),
#endif
#ifdef HAVE_ST_PW_QUOTA
			 INT2NUM(pwd->pw_quota),
#endif
#ifdef HAVE_ST_PW_AGE
			 PW_AGE2VAL(pwd->pw_age),
#endif
#ifdef HAVE_ST_PW_CLASS
			 safe_setup_str(pwd->pw_class),
#endif
#ifdef HAVE_ST_PW_COMMENT
			 safe_setup_str(pwd->pw_comment),
#endif
#ifdef HAVE_ST_PW_EXPIRE
			 INT2NUM(pwd->pw_expire),
#endif
			 0		/*dummy*/
	);
}
#endif

/* Returns the /etc/passwd information for the user with specified integer
 * user id (uid).
 *
 * The information is returned as a Struct::Passwd; see getpwent above for
 * details.
 *
 * e.g.  * Etc.getpwuid(0) -> #<struct Struct::Passwd name="root",
 * passwd="x", uid=0, gid=0, gecos="root",dir="/root", shell="/bin/bash">
 */
static VALUE
etc_getpwuid(int argc, VALUE *argv, VALUE obj)
{
#if defined(HAVE_GETPWENT)
    VALUE id;
    rb_uid_t uid;
    struct passwd *pwd;

    rb_secure(4);
    if (rb_scan_args(argc, argv, "01", &id) == 1) {
	uid = NUM2UIDT(id);
    }
    else {
	uid = getuid();
    }
    pwd = getpwuid(uid);
    if (pwd == 0) rb_raise(rb_eArgError, "can't find user for %d", (int)uid);
    return setup_passwd(pwd);
#else
    return Qnil;
#endif
}

/* Returns the /etc/passwd information for the user with specified login name.
 *
 * The information is returned as a Struct::Passwd; see getpwent above for
 * details.
 *
 * e.g.  * Etc.getpwnam('root') -> #<struct Struct::Passwd name="root",
 * passwd="x", uid=0, gid=0, gecos="root",dir="/root", shell="/bin/bash">
 */
static VALUE
etc_getpwnam(VALUE obj, VALUE nam)
{
#ifdef HAVE_GETPWENT
    struct passwd *pwd;

    SafeStringValue(nam);
    pwd = getpwnam(RSTRING_PTR(nam));
    if (pwd == 0) rb_raise(rb_eArgError, "can't find user for %s", RSTRING_PTR(nam));
    return setup_passwd(pwd);
#else
    return Qnil;
#endif
}

#ifdef HAVE_GETPWENT
static int passwd_blocking = 0;
static VALUE
passwd_ensure(void)
{
    passwd_blocking = (int)Qfalse;
    return Qnil;
}

static VALUE
passwd_iterate(void)
{
    struct passwd *pw;

    setpwent();
    while (pw = getpwent()) {
	rb_yield(setup_passwd(pw));
    }
    endpwent();
    return Qnil;
}

static void
each_passwd(void)
{
    if (passwd_blocking) {
	rb_raise(rb_eRuntimeError, "parallel passwd iteration");
    }
    passwd_blocking = (int)Qtrue;
    rb_ensure(passwd_iterate, 0, passwd_ensure, 0);
}
#endif

/* Provides a convenient Ruby iterator which executes a block for each entry
 * in the /etc/passwd file.
 *
 * The code block is passed an Struct::Passwd struct; see getpwent above for
 * details.
 *
 * Example:
 *
 *     require 'etc'
 *
 *     Etc.passwd {|u|
 *       puts u.name + " = " + u.gecos
 *     }
 *
 */
static VALUE
etc_passwd(VALUE obj)
{
#ifdef HAVE_GETPWENT
    struct passwd *pw;

    rb_secure(4);
    if (rb_block_given_p()) {
	each_passwd();
    }
    else if (pw = getpwent()) {
	return setup_passwd(pw);
    }
#endif
    return Qnil;
}

/* Iterates for each entry in the /etc/passwd file if a block is given.
 * If no block is given, returns the enumerator.
 *
 * The code block is passed an Struct::Passwd struct; see getpwent above for
 * details.
 *
 * Example:
 *
 *     require 'etc'
 *
 *     Etc::Passwd.each {|u|
 *       puts u.name + " = " + u.gecos
 *     }
 *
 *     Etc::Passwd.collect {|u| u.gecos}
 *     Etc::Passwd.collect {|u| u.gecos}
 *
 */
static VALUE
etc_each_passwd(VALUE obj)
{
#ifdef HAVE_GETPWENT
    RETURN_ENUMERATOR(obj, 0, 0);
    each_passwd();
#endif
    return obj;
}

/* Resets the process of reading the /etc/passwd file, so that the next call
 * to getpwent will return the first entry again.
 */
static VALUE
etc_setpwent(VALUE obj)
{
#ifdef HAVE_GETPWENT
    setpwent();
#endif
    return Qnil;
}

/* Ends the process of scanning through the /etc/passwd file begun with
 * getpwent, and closes the file.
 */
static VALUE
etc_endpwent(VALUE obj)
{
#ifdef HAVE_GETPWENT
    endpwent();
#endif
    return Qnil;
}

/* Returns an entry from the /etc/passwd file. The first time it is called it
 * opens the file and returns the first entry; each successive call returns
 * the next entry, or nil if the end of the file has been reached.
 *
 * To close the file when processing is complete, call endpwent.
 *
 * Each entry is returned as a Struct::Passwd:
 *
 * - Passwd#name contains the short login name of the user as a String.
 *
 * - Passwd#passwd contains the encrypted password of the user as a String.
 *   an 'x' is returned if shadow passwords are in use. An '*' is returned
 *   if the user cannot log in using a password.
 *
 * - Passwd#uid contains the integer user ID (uid) of the user.
 *
 * - Passwd#gid contains the integer group ID (gid) of the user's primary group.
 *
 * - Passwd#gecos contains a longer String description of the user, such as
 *   a full name. Some Unix systems provide structured information in the
 *   gecos field, but this is system-dependent.
 *
 * - Passwd#dir contains the path to the home directory of the user as a String.
 *
 * - Passwd#shell contains the path to the login shell of the user as a String.
 */
static VALUE
etc_getpwent(VALUE obj)
{
#ifdef HAVE_GETPWENT
    struct passwd *pw;

    if (pw = getpwent()) {
	return setup_passwd(pw);
    }
#endif
    return Qnil;
}

#ifdef HAVE_GETGRENT
static VALUE
setup_group(struct group *grp)
{
    VALUE mem;
    char **tbl;

    mem = rb_ary_new();
    tbl = grp->gr_mem;
    while (*tbl) {
	rb_ary_push(mem, safe_setup_str(*tbl));
	tbl++;
    }
    return rb_struct_new(sGroup,
			 safe_setup_str(grp->gr_name),
#ifdef HAVE_ST_GR_PASSWD
			 safe_setup_str(grp->gr_passwd),
#endif
			 GIDT2NUM(grp->gr_gid),
			 mem);
}
#endif

/* Returns information about the group with specified integer group id (gid),
 * as found in /etc/group.
 *
 * The information is returned as a Struct::Group; see getgrent above for
 * details.
 *
 * e.g.  Etc.getgrgid(100) -> #<struct Struct::Group name="users", passwd="x",
 * gid=100, mem=["meta", "root"]>
 *
 */
static VALUE
etc_getgrgid(int argc, VALUE *argv, VALUE obj)
{
#ifdef HAVE_GETGRENT
    VALUE id;
    gid_t gid;
    struct group *grp;

    rb_secure(4);
    if (rb_scan_args(argc, argv, "01", &id) == 1) {
	gid = NUM2GIDT(id);
    }
    else {
	gid = getgid();
    }
    grp = getgrgid(gid);
    if (grp == 0) rb_raise(rb_eArgError, "can't find group for %d", (int)gid);
    return setup_group(grp);
#else
    return Qnil;
#endif
}

/* Returns information about the group with specified String name, as found
 * in /etc/group.
 *
 * The information is returned as a Struct::Group; see getgrent above for
 * details.
 *
 * e.g.  Etc.getgrnam('users') -> #<struct Struct::Group name="users",
 * passwd="x", gid=100, mem=["meta", "root"]>
 *
 */
static VALUE
etc_getgrnam(VALUE obj, VALUE nam)
{
#ifdef HAVE_GETGRENT
    struct group *grp;

    rb_secure(4);
    SafeStringValue(nam);
    grp = getgrnam(RSTRING_PTR(nam));
    if (grp == 0) rb_raise(rb_eArgError, "can't find group for %s", RSTRING_PTR(nam));
    return setup_group(grp);
#else
    return Qnil;
#endif
}

#ifdef HAVE_GETGRENT
static int group_blocking = 0;
static VALUE
group_ensure(void)
{
    group_blocking = (int)Qfalse;
    return Qnil;
}

static VALUE
group_iterate(void)
{
    struct group *pw;

    setgrent();
    while (pw = getgrent()) {
	rb_yield(setup_group(pw));
    }
    endgrent();
    return Qnil;
}

static void
each_group(void)
{
    if (group_blocking) {
	rb_raise(rb_eRuntimeError, "parallel group iteration");
    }
    group_blocking = (int)Qtrue;
    rb_ensure(group_iterate, 0, group_ensure, 0);
}
#endif

/* Provides a convenient Ruby iterator which executes a block for each entry
 * in the /etc/group file.
 *
 * The code block is passed an Struct::Group struct; see getgrent above for
 * details.
 *
 * Example:
 *
 *     require 'etc'
 *
 *     Etc.group {|g|
 *       puts g.name + ": " + g.mem.join(', ')
 *     }
 *
 */
static VALUE
etc_group(VALUE obj)
{
#ifdef HAVE_GETGRENT
    struct group *grp;

    rb_secure(4);
    if (rb_block_given_p()) {
	each_group();
    }
    else if (grp = getgrent()) {
	return setup_group(grp);
    }
#endif
    return Qnil;
}

#ifdef HAVE_GETGRENT
/* Iterates for each entry in the /etc/group file if a block is given.
 * If no block is given, returns the enumerator.
 *
 * The code block is passed an Struct::Group struct; see getpwent above for
 * details.
 *
 * Example:
 *
 *     require 'etc'
 *
 *     Etc::Group.each {|g|
 *       puts g.name + ": " + g.mem.join(', ')
 *     }
 *
 *     Etc::Group.collect {|g| g.name}
 *     Etc::Group.select {|g| !g.mem.empty?}
 *
 */
static VALUE
etc_each_group(VALUE obj)
{
    RETURN_ENUMERATOR(obj, 0, 0);
    each_group();
    return obj;
}
#endif

/* Resets the process of reading the /etc/group file, so that the next call
 * to getgrent will return the first entry again.
 */
static VALUE
etc_setgrent(VALUE obj)
{
#ifdef HAVE_GETGRENT
    setgrent();
#endif
    return Qnil;
}

/* Ends the process of scanning through the /etc/group file begun by
 * getgrent, and closes the file.
 */
static VALUE
etc_endgrent(VALUE obj)
{
#ifdef HAVE_GETGRENT
    endgrent();
#endif
    return Qnil;
}

/* Returns an entry from the /etc/group file. The first time it is called it
 * opens the file and returns the first entry; each successive call returns
 * the next entry, or nil if the end of the file has been reached.
 *
 * To close the file when processing is complete, call endgrent.
 *
 * Each entry is returned as a Struct::Group:
 *
 * - Group#name contains the name of the group as a String.
 *
 * - Group#passwd contains the encrypted password as a String. An 'x' is
 *   returned if password access to the group is not available; an empty
 *   string is returned if no password is needed to obtain membership of
 *   the group.
 *
 * - Group#gid contains the group's numeric ID as an integer.
 *
 * - Group#mem is an Array of Strings containing the short login names of the
 *   members of the group.
 */
static VALUE
etc_getgrent(VALUE obj)
{
#ifdef HAVE_GETGRENT
    struct group *gr;

    if (gr = getgrent()) {
	return setup_group(gr);
    }
#endif
    return Qnil;
}

#define numberof(array) (sizeof(array) / sizeof(*(array)))

#ifdef _WIN32
VALUE rb_w32_special_folder(int type);
UINT rb_w32_system_tmpdir(WCHAR *path, UINT len);
VALUE rb_w32_conv_from_wchar(const WCHAR *wstr, rb_encoding *enc);
#endif

/*
 * Returns system configuration directory. This is typically "/etc", but
 * is modified by the prefix used when Ruby was compiled. For example,
 * if Ruby is built and installed in /usr/local, returns "/usr/local/etc".
 */
static VALUE
etc_sysconfdir(VALUE obj)
{
#ifdef _WIN32
    return rb_w32_special_folder(CSIDL_COMMON_APPDATA);
#else
    return rb_filesystem_str_new_cstr(SYSCONFDIR);
#endif
}

/*
 * Returns system temporary directory; typically "/tmp".
 */
static VALUE
etc_systmpdir(void)
{
    VALUE tmpdir;
#ifdef _WIN32
    WCHAR path[_MAX_PATH];
    UINT len = rb_w32_system_tmpdir(path, numberof(path));
    if (!len) return Qnil;
    tmpdir = rb_w32_conv_from_wchar(path, rb_filesystem_encoding());
#else
    tmpdir = rb_filesystem_str_new_cstr("/tmp");
#endif
    FL_UNSET(tmpdir, FL_TAINT|FL_UNTRUSTED);
    return tmpdir;
}

/*
 * The Etc module provides access to information typically stored in
 * files in the /etc directory on Unix systems.
 *
 * The information accessible consists of the information found in the
 * /etc/passwd and /etc/group files, plus information about the system's
 * temporary directory (/tmp) and configuration directory (/etc).
 *
 * The Etc module provides a more reliable way to access information about
 * the logged in user than environment variables such as $USER. For example:
 *
 *     require 'etc'
 *
 *     login = Etc.getlogin
 *     info = Etc.getpwnam(login)
 *     username = info.gecos.split(/,/).first
 *     puts "Hello #{username}, I see your login name is #{login}"
 *
 * Note that the methods provided by this module are not always secure.
 * It should be used for informational purposes, and not for security.
 */
void
Init_etc(void)
{
    VALUE mEtc;

    mEtc = rb_define_module("Etc");
    rb_define_module_function(mEtc, "getlogin", etc_getlogin, 0);

    rb_define_module_function(mEtc, "getpwuid", etc_getpwuid, -1);
    rb_define_module_function(mEtc, "getpwnam", etc_getpwnam, 1);
    rb_define_module_function(mEtc, "setpwent", etc_setpwent, 0);
    rb_define_module_function(mEtc, "endpwent", etc_endpwent, 0);
    rb_define_module_function(mEtc, "getpwent", etc_getpwent, 0);
    rb_define_module_function(mEtc, "passwd", etc_passwd, 0);

    rb_define_module_function(mEtc, "getgrgid", etc_getgrgid, -1);
    rb_define_module_function(mEtc, "getgrnam", etc_getgrnam, 1);
    rb_define_module_function(mEtc, "group", etc_group, 0);
    rb_define_module_function(mEtc, "setgrent", etc_setgrent, 0);
    rb_define_module_function(mEtc, "endgrent", etc_endgrent, 0);
    rb_define_module_function(mEtc, "getgrent", etc_getgrent, 0);
    rb_define_module_function(mEtc, "sysconfdir", etc_sysconfdir, 0);
    rb_define_module_function(mEtc, "systmpdir", etc_systmpdir, 0);

    sPasswd =  rb_struct_define("Passwd",
				"name", "passwd", "uid", "gid",
#ifdef HAVE_ST_PW_GECOS
				"gecos",
#endif
				"dir", "shell",
#ifdef HAVE_ST_PW_CHANGE
				"change",
#endif
#ifdef HAVE_ST_PW_QUOTA
				"quota",
#endif
#ifdef HAVE_ST_PW_AGE
				"age",
#endif
#ifdef HAVE_ST_PW_CLASS
				"uclass",
#endif
#ifdef HAVE_ST_PW_COMMENT
				"comment",
#endif
#ifdef HAVE_ST_PW_EXPIRE
				"expire",
#endif
				NULL);
    rb_define_const(mEtc, "Passwd", sPasswd);
    rb_extend_object(sPasswd, rb_mEnumerable);
    rb_define_singleton_method(sPasswd, "each", etc_each_passwd, 0);

#ifdef HAVE_GETGRENT
    sGroup = rb_struct_define("Group", "name",
#ifdef HAVE_ST_GR_PASSWD
			      "passwd",
#endif
			      "gid", "mem", NULL);

    rb_define_const(mEtc, "Group", sGroup);
    rb_extend_object(sGroup, rb_mEnumerable);
    rb_define_singleton_method(sGroup, "each", etc_each_group, 0);
#endif
}
