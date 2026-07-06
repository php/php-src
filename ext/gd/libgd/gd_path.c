
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd_intern.h"
#include "gd_vector2d_private.h"
#include "gdhelpers.h"

#include "gd_compositor.h"
#include "gd_gradient.h"
#include "gd_path.h"
#include "gd_path_arc.h"
#include "gd_path_dash.h"
#include "gd_path_matrix.h"
#include "gd_span_rle.h"

static gdSurfacePtr gdSurfaceSnapshotImage(gdImagePtr image)
{
    gdSurfacePtr surface;
    int x, y;

    if (!image)
        return NULL;
    surface = gdSurfaceCreate(image->sx, image->sy, GD_SURFACE_ARGB32);
    if (!surface)
        return NULL;
    for (y = 0; y < image->sy; y++) {
        uint32_t *row = (uint32_t *)(surface->data + y * surface->stride);
        for (x = 0; x < image->sx; x++) {
            int pixel = gdImageGetTrueColorPixel(image, x, y);
            row[x] = gdCompositePixelToArgb32(gdCompositePixelFromGd(pixel));
        }
    }
    return surface;
}

gdPaintPtr gdPaintAddRef(gdPaintPtr paint)
{
    if (paint == NULL)
        return NULL;

    ++paint->ref;
    return paint;
}

BGD_DECLARE(void) gdPaintDestroy(gdPaintPtr paint)
{
    if (paint == NULL)
        return;
    paint->ref--;

    if (paint->ref == 0) {
        switch (paint->type) {
        case gdPaintTypeColor:
            gdFree(paint->color);
            break;
        case gdPaintTypeGradient:
            gdGradientDestroy(paint->gradient);
            break;
        case gdPaintTypeSurface:
            // Not implemented
            break;
        case gdPaintTypePattern:
            gdPathPatternDestroy(paint->pattern);
            break;
        default: {
        }
        }
        gdFree(paint);
    }
}

void gdColorInitRgba(gdColorPtr color, double r, double g, double b, double a)
{
    color->r = CLAMP(r, 0.0, 1.0);
    color->g = CLAMP(g, 0.0, 1.0);
    color->b = CLAMP(b, 0.0, 1.0);
    color->a = CLAMP(a, 0.0, 1.0);
}

gdPathPatternPtr gdPaintGetPattern(const gdPaintPtr paint)
{
    return paint->type == gdPaintTypePattern ? paint->pattern : NULL;
}

GD_VECTOR2D_INTERNAL gdPathPatternPtr gdPathPatternCreate(gdSurfacePtr surface)
{
    if (!surface)
        return NULL;
    gdPathPatternPtr pattern = gdMalloc(sizeof(gdPathPattern));
    if (!pattern)
        return NULL;
    pattern->ref = 1;
    pattern->extend = GD_EXTEND_NONE;
    pattern->surface = surface;
    gdSurfaceAddRef(surface);
    pattern->opacity = 1.0;
    gdPathMatrixInitIdentity(&pattern->matrix);
    return pattern;
}

BGD_DECLARE(gdPathPatternPtr) gdPathPatternCreateForImage(gdImagePtr image)
{
    gdSurfacePtr snapshot = gdSurfaceSnapshotImage(image);
    gdPathPatternPtr pattern;

    if (!snapshot)
        return NULL;
    pattern = gdPathPatternCreate(snapshot);
    gdSurfaceDestroy(snapshot);
    return pattern;
}

BGD_DECLARE(void) gdPathPatternDestroy(gdPathPatternPtr pattern)
{
    if (pattern == NULL)
        return;

    if (--pattern->ref == 0) {
        gdSurfaceDestroy(pattern->surface);
        gdFree(pattern);
    }
}

void gdPathPatternAddRef(gdPathPatternPtr pattern)
{
    if (pattern == NULL)
        return;
    pattern->ref++;
}

BGD_DECLARE(gdPaintPtr) gdPaintCreateFromPattern(gdPathPatternPtr pattern)
{
    gdPaintPtr paint = gdMalloc(sizeof(gdPaint));
    if (!paint)
        return NULL;
    paint->ref = 1;
    paint->type = gdPaintTypePattern;
    paint->pattern = pattern;
    gdPathPatternAddRef(pattern);
    return paint;
}

gdPaintPtr gdPaintCreateForSurface(gdSurfacePtr surface)
{
    gdPathPatternPtr pattern = gdPathPatternCreate(surface);
    gdPaintPtr paint = gdPaintCreateFromPattern(pattern);
    gdPathPatternDestroy(pattern);
    return paint;
}

BGD_DECLARE(void) gdPathPatternSetExtend(gdPathPatternPtr pattern, gdExtendMode extend)
{
    pattern->extend = extend;
}

BGD_DECLARE(void) gdPathPatternSetMatrix(gdPathPatternPtr pattern, gdPathMatrixPtr matrix)
{
    memcpy(&pattern->matrix, matrix, sizeof(gdPathMatrix));
}

BGD_DECLARE(void) gdPathPatternSetOpacity(gdPathPatternPtr pattern, double opacity)
{
    if (!pattern || !isfinite(opacity))
        return;
    pattern->opacity = CLAMP(opacity, 0.0, 1.0);
}

void gdPaintSetSourceSurface(gdContextPtr context, gdSurfacePtr surface, double x, double y)
{
    gdPathMatrix matrix;
    gdPaintPtr paint;
    gdPathPatternPtr pattern = gdPathPatternCreate(surface);
    gdPathPatternSetExtend(pattern, GD_EXTEND_NONE);
    gdPathMatrixMultiply(&matrix, &matrix, &context->state->matrix);
    gdPathMatrixInitTranslate(&matrix, x, y);
    gdPathPatternSetMatrix(pattern, &matrix);
    paint = gdPaintCreateFromPattern(pattern);
    gdContextSetSource(context, paint);
}

gdPaintPtr gdPaintCreateRgba(double r, double g, double b, double a)
{
    gdPaintPtr paint = gdMalloc(sizeof(gdPaint));
    if (!paint) {
        return NULL;
    }
    paint->color = gdMalloc(sizeof(gdColor));
    if (!paint->color) {
        gdFree(paint);
        return NULL;
    }

    paint->ref = 1;
    paint->type = gdPaintTypeColor;
    gdColorInitRgba(paint->color, r, g, b, a);
    return paint;
}

gdPaintPtr gdPaintCreateRgb(double r, double g, double b)
{
    return gdPaintCreateRgba(r, g, b, 1.0);
}

BGD_DECLARE(void) gdContextSetSource(gdContextPtr context, gdPaintPtr source)
{
    source = gdPaintAddRef(source);
    gdPaintDestroy(context->state->source);
    context->state->source = source;
}

void gdContextSetSourceColorRgb(gdContextPtr context, double r, double g, double b)
{
    gdContextSetSourceRgba(context, r, g, b, 1.0);
}

gdStatePtr gdStateCreate()
{
    gdStatePtr state = gdMalloc(sizeof(gdState));
    if (!state) {
        return NULL;
    }
    // state->font = NULL;
    state->source = gdPaintCreateRgba(0, 0, 0, 1.0);
    if (!state->source) {
        gdFree(state);
        return NULL;
    }
    gdPathMatrixInitIdentity(&state->matrix);
    state->winding = gdFillRuleNonZero;
    state->stroke.width = 1.0;
    state->stroke.miterlimit = 4.0;
    state->stroke.cap = gdLineCapButt;
    state->stroke.join = gdLineJoinMiter;
    state->stroke.dash = NULL;
    state->op = GD_OP_OVER;
    // state->fontsize = 12.0;
    state->opacity = 1.0;
    state->clippath = NULL;
    state->next = NULL;
    return state;
}

void gdStateDestroy(gdStatePtr state)
{
    // state->font
    gdSpanRleDestroy(state->clippath);
    gdPaintDestroy(state->source);
    gdPathDashDestroy(state->stroke.dash);
    gdFree(state);
}

BGD_DECLARE(gdPathPtr) gdPathCreate()
{
    gdPathPtr path = gdMalloc(sizeof(gdPath));
    if (!path)
        return NULL;
    path->ref = 1;
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
    gdArrayInit(&path->elements, sizeof(gdPathOps));
    gdArrayInit(&path->points, sizeof(gdPointF));
    return path;
}

gdPathPtr gdPathDuplicate(const gdPathPtr path)
{
    gdPathPtr result = gdPathCreate();
    if (!result)
        return NULL;
    gdArrayInit(&result->elements, sizeof(gdPathOps));
    gdArrayInit(&result->points, sizeof(gdPointF));
    gdArrayAppendMultiple(&result->elements, gdArrayGetData(&path->elements),
                          gdArrayNumElements(&path->elements));
    gdArrayAppendMultiple(&result->points, gdArrayGetData(&path->points),
                          gdArrayNumElements(&path->points));

    result->contours = path->contours;
    result->start = path->start;

    return result;
}

BGD_DECLARE(void) gdPathAppendPath(gdPathPtr path, const gdPathPtr source)
{
    gdArrayAppendMultiple(&path->elements, gdArrayGetData(&source->elements),
                          gdArrayNumElements(&source->elements));
    gdArrayAppendMultiple(&path->points, gdArrayGetData(&source->points),
                          gdArrayNumElements(&source->points));

    path->contours += source->contours;
    path->start = source->start;
}

BGD_DECLARE(void) gdPathTransform(gdPathPtr path, const gdPathMatrixPtr matrix)
{
    gdPointFPtr p;
    unsigned int numElements = gdArrayNumElements(&path->points);
    unsigned int i;

    // memset(p, 0, sizeof(gdPointF) * 3);
    for (i = 0; i < numElements; i++) {
        p = (gdPointFPtr)gdArrayIndex(&path->points, i);
        gdPathMatrixMapPoint(matrix, p, p);
    }
}

static inline void _path_get_current_point(const gdPathPtr path, double *x, double *y)
{
    unsigned int numElems = gdArrayNumElements(&path->points);
    if (x)
        *x = 0.0;
    if (y)
        *y = 0.0;
    if (numElems < 1)
        return;
    gdPointFPtr point = gdArrayIndex(&path->points, numElems - 1);
    if (x) {
        *x = point->x;
    }
    if (y) {
        *y = point->y;
    }
}

typedef struct {
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
    double x4;
    double y4;
} cubic_points;

static inline void split(const cubic_points *b, cubic_points *first, cubic_points *second)
{
    double c = (b->x2 + b->x3) * 0.5;
    first->x2 = (b->x1 + b->x2) * 0.5;
    second->x3 = (b->x3 + b->x4) * 0.5;
    first->x1 = b->x1;
    second->x4 = b->x4;
    first->x3 = (first->x2 + c) * 0.5;
    second->x2 = (second->x3 + c) * 0.5;
    first->x4 = second->x1 = (first->x3 + second->x2) * 0.5;

    c = (b->y2 + b->y3) * 0.5;
    first->y2 = (b->y1 + b->y2) * 0.5;
    second->y3 = (b->y3 + b->y4) * 0.5;
    first->y1 = b->y1;
    second->y4 = b->y4;
    first->y3 = (first->y2 + c) * 0.5;
    second->y2 = (second->y3 + c) * 0.5;
    first->y4 = second->y1 = (first->y3 + second->y2) * 0.5;
}

/* See http://agg.sourceforge.net/antigrain.com/research/adaptive_bezier/index.html */
static void _cubic_flatten(gdPathPtr path, const gdPointFPtr p0, const gdPointFPtr p1,
                           const gdPointFPtr p2, const gdPointFPtr p3)
{
    cubic_points beziers[32];
    beziers[0].x1 = p0->x;
    beziers[0].y1 = p0->y;
    beziers[0].x2 = p1->x;
    beziers[0].y2 = p1->y;
    beziers[0].x3 = p2->x;
    beziers[0].y3 = p2->y;
    beziers[0].x4 = p3->x;
    beziers[0].y4 = p3->y;

    /* tolerance for the distance t to the line
        0.1 is a common accepted value
    */
    const double tolerance = 0.1;

    cubic_points *b = beziers;
    while (b >= beziers) {
        double y4y1 = b->y4 - b->y1;
        double x4x1 = b->x4 - b->x1;
        double l = fabs(x4x1) + fabs(y4y1);
        double d;
        if (l > 1.0) {
            d = fabs((x4x1) * (b->y1 - b->y2) - (y4y1) * (b->x1 - b->x2)) +
                fabs((x4x1) * (b->y1 - b->y3) - (y4y1) * (b->x1 - b->x3));
        } else {
            d = fabs(b->x1 - b->x2) + fabs(b->y1 - b->y2) + fabs(b->x1 - b->x3) +
                fabs(b->y1 - b->y3);
            l = 1.0;
        }

        if (d < tolerance * l || b == beziers + 31) {
            gdPathLineTo(path, b->x4, b->y4);
            --b;
        } else {
            split(b, b + 1, b);
            ++b;
        }
    }
}

gdPathPtr gdPathDuplicateFlattened(const gdPathPtr path)
{
    gdPathPtr result = gdPathCreate();

    gdArrayReallocBy(&result->elements, gdArrayNumElements(&path->elements));
    gdArrayReallocBy(&result->points, gdArrayNumElements(&path->points));

    gdPointFPtr points = gdArrayGetData(&path->points);
    for (unsigned int i = 0; i < gdArrayNumElements(&path->elements); i++) {
        const gdPathOpsPtr cur_elem = gdArrayIndex(&path->elements, i);
        switch (*cur_elem) {
        case gdPathOpsMoveTo:
            gdPathMoveTo(result, points[0].x, points[0].y);
            points += 1;
            break;
        case gdPathOpsLineTo:
        case gdPathOpsClose:
            gdPathLineTo(result, points[0].x, points[0].y);
            points += 1;
            break;
        case gdPathOpsCubicTo: {
            gdPointF p0;
            _path_get_current_point(result, &p0.x, &p0.y);
            _cubic_flatten(result, &p0, points, points + 1, points + 2);
            points += 3;
            break;
        }
        default:
            // Only to silent compiler
            break;
        }
    }
    return result;
}

gdPathPtr gdPathAddRef(gdPathPtr path)
{
    if (path == NULL)
        return NULL;
    path->ref++;
    return path;
}

BGD_DECLARE(void) gdPathDestroy(gdPathPtr path)
{
    if (path == NULL)
        return;
    path->ref--;
    if (path->ref == 0) {
        gdArrayDestroy(&path->elements);
        gdArrayDestroy(&path->points);
        gdFree(path);
    }
}

void gdPathClear(gdPathPtr path)
{
    gdArrayTruncate(&path->elements, 0);
    gdArrayTruncate(&path->points, 0);
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
}

/* dump a path, could be nicer but good enough for now
Not exported, only for debugging purposes here */
void gdPathDumpPathTransform(const gdPathPtr path, const gdPathMatrixPtr matrix)
{
    // GD_FT_Outline* outline = gd_ft_outline_create(path->points.size, path->contours);
    gdPointF p[3];
    unsigned int numElements = gdArrayNumElements(&path->elements);
    unsigned int pointsIndex = 0;
    unsigned int i;

    memset(p, 0, sizeof(gdPointF) * 3);
    printf("NEWOUTLINE CONVERT\n");
    for (i = 0; i < numElements; i++) {
        gdPathOpsPtr element = (gdPathOpsPtr)gdArrayIndex(&path->elements, i);
        gdPointFPtr point = gdArrayIndex(&path->points, pointsIndex);
        printf("-------\n");
        switch (*element) {
        case gdPathOpsMoveTo:
            if (matrix)
                gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("MoveTo(%f, %f)", point->x, point->y);
            printf("(%f, %f)", p[0].x, p[0].y);
            pointsIndex += 1;
            break;
        case gdPathOpsLineTo:
            if (matrix)
                gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("LineTo(%f, %f)", point->x, point->y);
            printf("(%f, %f)", p[0].x, p[0].y);
            pointsIndex += 1;
            break;
        case gdPathOpsCubicTo:
            printf("CubicTo(%f, %f)", point->x, point->y);
            if (matrix)
                gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("(%f, %f)", p[0].x, p[0].y);

            point = gdArrayIndex(&path->points, pointsIndex + 1);
            if (matrix)
                gdPathMatrixMapPoint(matrix, point, &p[1]);
            printf("(%f, %f)", p[1].x, p[1].y);

            point = gdArrayIndex(&path->points, pointsIndex + 2);
            if (matrix)
                gdPathMatrixMapPoint(matrix, point, &p[2]);
            printf("(%f, %f)", p[2].x, p[2].y);
            pointsIndex += 3;
            break;
        case gdPathOpsClose:
            printf("Outline close");
            pointsIndex += 1;
            break;
        default:
            break;
        }
        printf("\n-------\n");
    }
}

static inline void _relativeTo(const gdPathPtr path, double *x, double *y)
{
    double _x = -1, _y = -1;
    _path_get_current_point(path, &_x, &_y);
    *x += _x;
    *y += _y;
}

BGD_DECLARE(void) gdPathMoveTo(gdPathPtr path, double x, double y)
{
    const gdPathOps op = gdPathOpsMoveTo;
    gdPointF point;

    gdArrayAppend(&path->elements, &op);
    path->contours += 1;

    point.x = x;
    point.y = y;
    gdArrayAppend(&path->points, &point);
    path->start.x = x;
    path->start.y = y;
}

BGD_DECLARE(void) gdPathLineTo(gdPathPtr path, double x, double y)
{
    const gdPathOps op = gdPathOpsLineTo;
    gdPointF point;

    gdArrayAppend(&path->elements, &op);

    point.x = x;
    point.y = y;
    gdArrayAppend(&path->points, &point);
}
BGD_DECLARE(void) gdPathRelLineTo(gdPathPtr path, double dx, double dy)
{
    _relativeTo(path, &dx, &dy);
    gdPathLineTo(path, dx, dy);
}

BGD_DECLARE(void)
gdPathCurveTo(gdPathPtr path, double x1, double y1, double x2, double y2, double x3, double y3)
{
    const gdPathOps op = gdPathOpsCubicTo;
    gdPointF points[3];

    gdArrayAppend(&path->elements, &op);
    points[0].x = x1;
    points[0].y = y1;
    points[1].x = x2;
    points[1].y = y2;
    points[2].x = x3;
    points[2].y = y3;
    gdArrayAppend(&path->points, &points[0]);
    gdArrayAppend(&path->points, &points[1]);
    gdArrayAppend(&path->points, &points[2]);
}

BGD_DECLARE(void) gdPathQuadTo(gdPathPtr path, double x1, double y1, double x2, double y2)
{
    const gdPathOps op = gdPathOpsQuadTo;
    gdPointF points[2] = {{x1, y1}, {x2, y2}};

    gdArrayAppend(&path->elements, &op);
    gdArrayAppend(&path->points, &points[0]);
    gdArrayAppend(&path->points, &points[1]);
}

/*
Based on http://www.whizkidtech.redprince.net/bezier/circle/kappa/
*/
void gdPathAddArc(gdPathPtr path, double cx, double cy, double radius, double angle1, double angle2,
                  int ccw)
{
    if (ccw)
        _gd_arc_path_negative(path, cx, cy, radius, angle1, angle2);
    else
        _gd_arc_path(path, cx, cy, radius, angle1, angle2);
}

void gdPathArcTo(gdPathPtr path, double x1, double y1, double x2, double y2, double radius)
{
    double x0, y0;
    _path_get_current_point(path, &x0, &y0);
    if ((x0 == x1 && y0 == y1) || (x1 == x2 && y1 == y2) || radius == 0.0) {
        gdPathLineTo(path, x1, y2);
        return;
    }

    double dir = (x2 - x1) * (y0 - y1) + (y2 - y1) * (x1 - x0);
    if (dir == 0.0) {
        gdPathLineTo(path, x1, y2);
        return;
    }

    double a2 = (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
    double b2 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    double c2 = (x0 - x2) * (x0 - x2) + (y0 - y2) * (y0 - y2);

    double cosx = (a2 + b2 - c2) / (2 * sqrt(a2 * b2));
    double sinx = sqrt(1 - cosx * cosx);
    double d = radius / ((1 - cosx) / sinx);

    double anx = (x1 - x0) / sqrt(a2);
    double any = (y1 - y0) / sqrt(a2);
    double bnx = (x1 - x2) / sqrt(b2);
    double bny = (y1 - y2) / sqrt(b2);

    double x3 = x1 - anx * d;
    double y3 = y1 - any * d;
    double x4 = x1 - bnx * d;
    double y4 = y1 - bny * d;

    int ccw = dir < 0.0;
    double cx = x3 + any * radius * (ccw ? 1 : -1);
    double cy = y3 - anx * radius * (ccw ? 1 : -1);
    double a0 = atan2(y3 - cy, x3 - cx);
    double a1 = atan2(y4 - cy, x4 - cx);

    gdPathLineTo(path, x3, y3);
    gdPathAddArc(path, cx, cy, radius, a0, a1, ccw);
}

void gdPathAddRectangle(gdPathPtr path, double x, double y, double w, double h)
{
    gdPathMoveTo(path, x, y);
    gdPathLineTo(path, x + w, y);
    gdPathLineTo(path, x + w, y + h);
    gdPathLineTo(path, x, y + h);
    gdPathLineTo(path, x, y);
    gdPathClose(path);
}

BGD_DECLARE(void) gdPathClose(gdPathPtr path)
{
    const int numElements = gdArrayNumElements(&path->elements);
    const gdPathOps OpClose = gdPathOpsClose;
    if (numElements == 0)
        return;
    const gdPathOpsPtr lastOpPtr = gdArrayIndex(&path->elements, (unsigned int)numElements - 1);
    if (*lastOpPtr == gdPathOpsClose)
        return;

    gdPointF point;
    point.x = path->start.x;
    point.y = path->start.y;
    gdArrayAppend(&path->elements, &OpClose);
    gdArrayAppend(&path->points, &point);
}

void gdPathBlend(gdContextPtr context, const gdSpanRlePtr rle);
