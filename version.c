/**********************************************************************

  version.c -

  $Author$
  created at: Thu Sep 30 20:08:01 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "version.h"
#include <stdio.h>

#define PRINT(type) puts(ruby_##type)
#define MKSTR(type) rb_obj_freeze(rb_usascii_str_new(ruby_##type, sizeof(ruby_##type)-1))

#ifndef RUBY_ARCH
#define RUBY_ARCH RUBY_PLATFORM
#endif
#ifndef RUBY_SITEARCH
#define RUBY_SITEARCH RUBY_ARCH
#endif
#ifdef RUBY_PLATFORM_CPU
#define RUBY_THINARCH RUBY_PLATFORM_CPU"-"RUBY_PLATFORM_OS
#endif
#ifndef RUBY_LIB_PREFIX
#ifndef RUBY_EXEC_PREFIX
#error RUBY_EXEC_PREFIX must be defined
#endif
#define RUBY_LIB_PREFIX RUBY_EXEC_PREFIX"/lib/ruby"
#endif
#ifndef RUBY_SITE_LIB
#define RUBY_SITE_LIB RUBY_LIB_PREFIX"/site_ruby"
#endif
#ifndef RUBY_VENDOR_LIB
#define RUBY_VENDOR_LIB RUBY_LIB_PREFIX"/vendor_ruby"
#endif

#define RUBY_LIB                    RUBY_LIB_PREFIX  "/"RUBY_LIB_VERSION
#define RUBY_SITE_LIB2              RUBY_SITE_LIB    "/"RUBY_LIB_VERSION
#define RUBY_VENDOR_LIB2            RUBY_VENDOR_LIB  "/"RUBY_LIB_VERSION
#define RUBY_ARCHLIB                RUBY_LIB         "/"RUBY_ARCH
#define RUBY_SITE_ARCHLIB           RUBY_SITE_LIB2   "/"RUBY_SITEARCH
#define RUBY_VENDOR_ARCHLIB         RUBY_VENDOR_LIB2 "/"RUBY_SITEARCH
#ifdef  RUBY_THINARCH
#define RUBY_THIN_ARCHLIB           RUBY_LIB         "/"RUBY_THINARCH
#define RUBY_SITE_THIN_ARCHLIB      RUBY_SITE_LIB2   "/"RUBY_THINARCH
#define RUBY_VENDOR_THIN_ARCHLIB    RUBY_VENDOR_LIB2 "/"RUBY_THINARCH
#endif

const int ruby_api_version[] = {
    RUBY_API_VERSION_MAJOR,
    RUBY_API_VERSION_MINOR,
    RUBY_API_VERSION_TEENY,
};
const char ruby_version[] = RUBY_VERSION;
const char ruby_release_date[] = RUBY_RELEASE_DATE;
const char ruby_platform[] = RUBY_PLATFORM;
const int ruby_patchlevel = RUBY_PATCHLEVEL;
const char ruby_description[] = RUBY_DESCRIPTION;
const char ruby_copyright[] = RUBY_COPYRIGHT;
const char ruby_engine[] = "ruby";
VALUE ruby_engine_name = Qnil;

const char ruby_initial_load_paths[] =
#ifndef NO_INITIAL_LOAD_PATH
#ifdef RUBY_SEARCH_PATH
    RUBY_SEARCH_PATH "\0"
#endif
#ifndef NO_RUBY_SITE_LIB
    RUBY_SITE_LIB2 "\0"
#ifdef RUBY_SITE_THIN_ARCHLIB
    RUBY_SITE_THIN_ARCHLIB "\0"
#endif
    RUBY_SITE_ARCHLIB "\0"
    RUBY_SITE_LIB "\0"
#endif

#ifndef NO_RUBY_VENDOR_LIB
    RUBY_VENDOR_LIB2 "\0"
#ifdef RUBY_VENDOR_THIN_ARCHLIB
    RUBY_VENDOR_THIN_ARCHLIB "\0"
#endif
    RUBY_VENDOR_ARCHLIB "\0"
    RUBY_VENDOR_LIB "\0"
#endif

    RUBY_LIB "\0"
#ifdef RUBY_THIN_ARCHLIB
    RUBY_THIN_ARCHLIB "\0"
#endif
    RUBY_ARCHLIB "\0"
#endif
    "";

void
Init_version(void)
{
    /*
     * The running version of ruby
     */
    rb_define_global_const("RUBY_VERSION", MKSTR(version));
    /*
     * The date this ruby was released
     */
    rb_define_global_const("RUBY_RELEASE_DATE", MKSTR(release_date));
    /*
     * The platform for this ruby
     */
    rb_define_global_const("RUBY_PLATFORM", MKSTR(platform));
    /*
     * The patchlevel for this ruby.  If this is a development build of ruby
     * the patchlevel will be -1
     */
    rb_define_global_const("RUBY_PATCHLEVEL", INT2FIX(RUBY_PATCHLEVEL));
    /*
     * The SVN revision for this ruby.
     */
    rb_define_global_const("RUBY_REVISION", INT2FIX(RUBY_REVISION));
    /*
     * The full ruby version string, like <tt>ruby -v</tt> prints'
     */
    rb_define_global_const("RUBY_DESCRIPTION", MKSTR(description));
    /*
     * The copyright string for ruby
     */
    rb_define_global_const("RUBY_COPYRIGHT", MKSTR(copyright));
    /*
     * The engine or interpreter this ruby uses.
     */
    rb_define_global_const("RUBY_ENGINE", ruby_engine_name = MKSTR(engine));
}

void
ruby_show_version(void)
{
    PRINT(description);
    fflush(stdout);
}

void
ruby_show_copyright(void)
{
    PRINT(copyright);
    exit(0);
}
