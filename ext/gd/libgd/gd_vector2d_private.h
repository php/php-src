#ifndef GD_VECTOR2D_PRIVATE_H
#define GD_VECTOR2D_PRIVATE_H

#include "gd_vector2d.h"
#include "gd_array.h"

#if defined(__GNUC__) && !defined(_WIN32)
# define GD_VECTOR2D_INTERNAL __attribute__((visibility("hidden")))
#else
# define GD_VECTOR2D_INTERNAL
#endif

typedef enum { GD_SURFACE_NONE, GD_SURFACE_ARGB32, GD_SURFACE_XRGB32, GD_SURFACE_A8, GD_SURFACE_COUNT } gdSurfaceType;
typedef struct gdSurfaceStruct {
	int ref;
	unsigned char *data;
	gdSurfaceType type;
	int gdOwned;
	int width, height, stride;
} gdSurface;
typedef gdSurface *gdSurfacePtr;

typedef struct gdRectFStruct { double x, y, w, h; } gdRectF;
typedef gdRectF *gdRectFPtr;
typedef struct gdSpanStruct { short x, y; unsigned short len; unsigned char coverage; } gdSpan;
typedef gdSpan *gdSpanPtr;
typedef struct gdSpanRleStruct {
	int ref;
	struct { gdSpanPtr data; int size, capacity; } spans;
	int x, y, w, h;
} gdSpanRle;
typedef gdSpanRle *gdSpanRlePtr;

typedef enum { gdPaintTypeColor, gdPaintTypeGradient, gdPaintTypeSurface, gdPaintTypePattern } gdPaintType;
typedef struct gdColorStruct { double r, g, b, a; } gdColor;
typedef gdColor *gdColorPtr;
struct gdPathPatternStruct {
	int ref;
	gdExtendMode extend;
	gdSurfacePtr surface;
	gdPathMatrix matrix;
	double opacity;
};
typedef struct gdGradientStruct gdGradient;
struct gdPaintStruct {
	int ref;
	gdPaintType type;
	union { gdColorPtr color; gdSurfacePtr surface; gdPathPatternPtr pattern; gdGradient *gradient; };
};
typedef struct gdPathDashStruct { double *data; int size; double offset; } gdPathDash;
typedef gdPathDash *gdPathDashPtr;
typedef struct gdStrokeStruct {
	double width, miterlimit;
	gdLineCap cap;
	gdLineJoin join;
	gdPathDashPtr dash;
} gdStroke;
typedef gdStroke *gdStrokePtr;
typedef struct gdStateStruct {
	gdSpanRlePtr clippath;
	gdPaintPtr source;
	gdPathMatrix matrix;
	gdFillRule winding;
	gdStroke stroke;
	gdCompositeOperator op;
	double opacity;
	struct gdStateStruct *next;
} gdState;
typedef gdState *gdStatePtr;
typedef enum { gdPathOpsMoveTo, gdPathOpsLineTo, gdPathOpsCubicTo, gdPathOpsQuadTo, gdPathOpsClose } gdPathOps;
typedef gdPathOps *gdPathOpsPtr;
struct gdPathStruct {
	int ref, contours;
	gdPointF start;
	gdArray elements;
	gdArray points;
};
struct gdContextStruct {
	int ref;
	gdSurfacePtr surface;
	gdPathPtr path;
	gdStatePtr state;
	gdSpanRlePtr rle, clippath;
	gdRectF clip;
	gdImagePtr image;
	int imageOwned;
};

GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceCreate(int width, int height, unsigned int type);
GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceCreateForData(unsigned char *data, int width, int height, int stride, unsigned int type);
GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceAddRef(gdSurfacePtr surface);
GD_VECTOR2D_INTERNAL void gdSurfaceDestroy(gdSurfacePtr surface);
GD_VECTOR2D_INTERNAL gdContextPtr gdContextCreate(gdSurfacePtr surface);
GD_VECTOR2D_INTERNAL gdPathPatternPtr gdPathPatternCreate(gdSurfacePtr surface);
GD_VECTOR2D_INTERNAL void gdContextSetSourceSurface(gdContextPtr context, gdSurfacePtr surface, double x, double y);
GD_VECTOR2D_INTERNAL gdPathPtr gdPathStrokeToPath(const gdPathPtr path, const gdStrokePtr stroke, const gdPathMatrixPtr matrix);

#endif
