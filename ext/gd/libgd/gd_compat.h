#ifndef GD_COMPAT_H
#define GD_COMPAT_H 1

#if HAVE_GD_BUNDLED
# include "gd.h"
#else
# include <gd.h>
#endif

const char * gdPngGetVersionString();
const char * gdJpegGetVersionString();
int gdJpegGetVersionInt();

#endif /* GD_COMPAT_H */
