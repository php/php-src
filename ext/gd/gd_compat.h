#ifndef GD_COMPAT_H
#define GD_COMPAT_H 1

#ifndef HAVE_GD_BUNDLED
const char * gdJpegGetVersionString();
#endif

/* from gd_compat.c of libgd/gd_security.c */
int overflow2(int a, int b);

#endif /* GD_COMPAT_H */
