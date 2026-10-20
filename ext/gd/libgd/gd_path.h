#ifndef GD_PATH_H
#define GD_PATH_H

#include "gd_vector2d_private.h"

#include <math.h>

gdPathPtr gdPathAddRef(gdPathPtr path);

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI2
#define M_PI2 M_PI * 2
#endif

#define EPSILON_DOUBLE 0.000000000001f
#define MAX_FULL_CIRCLES 65536
#define PATH_KAPPA 0.5522847498
#define DEFAULT_TOLERANCE 0.1
static inline int _doubleEqualsEpsilon(double p1, double p2)
{
    return (fabs(p1 - p2) < EPSILON_DOUBLE);
}

static inline int _doubleIsZero(double f) { return (fabs(f) <= EPSILON_DOUBLE); }

#define ARRAY_LENGTH(__array) ((int)(sizeof(__array) / sizeof(__array[0])))

gdPaintPtr gdPaintCreateRgba(double r, double g, double b, double a);
gdPaintPtr gdPaintAddRef(gdPaintPtr paint);

gdStatePtr gdStateCreate();
void gdStateDestroy(gdStatePtr state);

gdPathPtr gdPathDuplicateFlattened(const gdPathPtr path);
void gdPathClear(gdPathPtr path);

gdPaintPtr gdPaintCreateForSurface(gdSurfacePtr surface);
gdPathPatternPtr gdPaintGetPattern(const gdPaintPtr paint);
void gdPaintSetSourceSurface(gdContextPtr context, gdSurfacePtr surface, double x, double y);
void gdPathDumpPathTransform(const gdPathPtr path, const gdPathMatrixPtr matrix);
#endif // GD_PATH_H
