/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* This is defines strtoull() */

#include <global.h>
#include <m_string.h>
#if !defined(HAVE_STRTOULL) && defined(HAVE_LONG_LONG)
#define USE_UNSIGNED
#define USE_LONGLONG
#include "strto.c"
#endif
