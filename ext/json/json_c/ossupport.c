/*
 * $Id$
 *
 * Copyright Marc Butler 2005.
 * Marc Butler <marcbutler@acm.org>
 *
 * Copyright (c) 2005 Omar Kilani <omar@rmilk.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public (LGPL)
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details: http://www.gnu.org/
 *
 */

#include <string.h>
#include <stdlib.h>
#include "ossupport.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if (defined(HAVE_CONFIG_H) && !defined(HAVE_STRNDUP)) || defined(__APPLE_CC__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__sun__) || defined(_MSC_VER)
/**
 * Synthesize strndup for BSD.
 */
char *strndup(const char *s, size_t len)
{
  char *copy;
  if (s == NULL)
    return NULL;
  copy = malloc(sizeof (char) * (len + 1));
  if (copy == NULL)
    return NULL;
  strncpy(copy, s, len);
  copy[len] = '\0';
  return copy;
}
#endif /* !HAVE_STRNDUP || __APPLE_CC__ || __FreeBSD__ || __OpenBSD__ || __sun__*/
