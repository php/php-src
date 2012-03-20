/**********************************************************************

  ruby/version.h -

  $Author$
  created at: Wed May 13 12:56:56 JST 2009

  Copyright (C) 1993-2009 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

/*
 * This file contains only
 * - never-changable informations, and
 * - interfaces accessible from extension libraries.
 *
 * Never try to check RUBY_VERSION_CODE etc in extension libraries,
 * check the features with mkmf.rb instead.
 */

#ifndef RUBY_VERSION_H
#define RUBY_VERSION_H 1

/* The origin. */
#define RUBY_AUTHOR "Yukihiro Matsumoto"
#define RUBY_BIRTH_YEAR 1993
#define RUBY_BIRTH_MONTH 2
#define RUBY_BIRTH_DAY 24

/* API version */
#define RUBY_API_VERSION_MAJOR 2
#define RUBY_API_VERSION_MINOR 0
#define RUBY_API_VERSION_TEENY 0
#define RUBY_API_VERSION_CODE (RUBY_API_VERSION_MAJOR*10000+RUBY_API_VERSION_MINOR*100+RUBY_API_VERSION_TEENY)

#ifdef RUBY_EXTERN
#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Interfaces from extension libraries.
 *
 * Before using these infos, think thrice whether they are really
 * necessary or not, and if the answer was yes, think twice a week
 * later again.
 */
RUBY_EXTERN const int ruby_api_version[3];
RUBY_EXTERN const char ruby_version[];
RUBY_EXTERN const char ruby_release_date[];
RUBY_EXTERN const char ruby_platform[];
RUBY_EXTERN const int  ruby_patchlevel;
RUBY_EXTERN const char ruby_description[];
RUBY_EXTERN const char ruby_copyright[];
RUBY_EXTERN const char ruby_engine[];

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif
#endif

#endif
