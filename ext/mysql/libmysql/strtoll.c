/* Copyright (C) 1996  TCX DataKonsult AB & Monty Program KB & Detron HB
   For a more info consult the file COPYRIGHT distributed with this file */

/* This is defines strtoll() if neaded */

#include <global.h>
#include <m_string.h>
#if !defined(HAVE_STRTOULL) && defined(HAVE_LONG_LONG)
#define LONGLONG
#include "strto.c"
#endif
