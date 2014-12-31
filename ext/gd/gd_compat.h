#ifndef GD_COMPAT_H
#define GD_COMPAT_H 1

#ifndef HAVE_GD_BUNDLED
/* from gd_compat.c */
const char * gdPngGetVersionString();
const char * gdJpegGetVersionString();
int gdJpegGetVersionInt();
#endif

/* from gd_compat.c of libgd/gd_security.c */
int overflow2(int a, int b);

#endif /* GD_COMPAT_H */
