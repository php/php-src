#ifndef GD_INTERN_H
#define GD_INTERN_H
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#define MIN3(a,b,c) ((a)<(b)?(MIN(a,c)):(MIN(b,c)))
#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif
#define MAX3(a,b,c) ((a)<(b)?(MAX(b,c)):(MAX(a,c)))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/* Internal prototypes: */

/* gd_rotate.c */
gdImagePtr gdImageRotate90(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate180(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate270(gdImagePtr src, int ignoretransparent);

#endif
