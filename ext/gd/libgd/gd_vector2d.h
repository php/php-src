#ifndef GD_VECTOR2D_H
#define GD_VECTOR2D_H

#include "gd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Experimental API: source and ABI compatibility are not yet guaranteed. */
#define GD_VECTOR2D_EXPERIMENTAL 1
#define GD_VECTOR2D_VERSION 1

typedef struct gdContextStruct gdContext;
typedef gdContext *gdContextPtr;
typedef struct gdPathStruct gdPath;
typedef gdPath *gdPathPtr;
typedef struct gdPaintStruct gdPaint;
typedef gdPaint *gdPaintPtr;
typedef struct gdPathPatternStruct gdPathPattern;
typedef gdPathPattern *gdPathPatternPtr;

typedef struct gdPathMatrixStruct {
    double m00, m10, m01, m11, m02, m12;
} gdPathMatrix;
typedef gdPathMatrix *gdPathMatrixPtr;

typedef struct gdRectFStruct {
    double x, y, w, h;
} gdRectF;
typedef gdRectF *gdRectFPtr;

typedef enum { GD_EXTEND_NONE, GD_EXTEND_REPEAT, GD_EXTEND_REFLECT, GD_EXTEND_PAD } gdExtendMode;
typedef enum { gdLineCapButt, gdLineCapRound, gdLineCapSquare } gdLineCap;
typedef enum { gdLineJoinMiter, gdLineJoinRound, gdLineJoinBevel } gdLineJoin;
typedef enum { gdFillRuleNonZero, gdFillRuleEvenOdd } gdFillRule;

typedef enum {
    GD_OP_CLEAR,
    GD_OP_SOURCE,
    GD_OP_OVER,
    GD_OP_IN,
    GD_OP_OUT,
    GD_OP_ATOP,
    GD_OP_DEST,
    GD_OP_DEST_OVER,
    GD_OP_DEST_IN,
    GD_OP_DEST_OUT,
    GD_OP_DEST_ATOP,
    GD_OP_XOR,
    GD_OP_ADD,
    GD_OP_SATURATE,
    GD_OP_MULTIPLY,
    GD_OP_SCREEN,
    GD_OP_OVERLAY,
    GD_OP_DARKEN,
    GD_OP_LIGHTEN,
    GD_OP_COLOR_DODGE,
    GD_OP_COLOR_BURN,
    GD_OP_HARD_LIGHT,
    GD_OP_SOFT_LIGHT,
    GD_OP_DIFFERENCE,
    GD_OP_EXCLUSION,
    GD_OP_HSL_HUE,
    GD_OP_HSL_SATURATION,
    GD_OP_HSL_COLOR,
    GD_OP_HSL_LUMINOSITY,
    GD_OP_COUNT
} gdCompositeOperator;
typedef gdCompositeOperator gdImageOp;
#define gdImageOpsSrc GD_OP_SOURCE
#define gdImageOpsSrcOver GD_OP_OVER
#define gdImageOpsDstIn GD_OP_DEST_IN
#define gdImageOpsDstOut GD_OP_DEST_OUT

BGD_DECLARE(gdContextPtr) gdContextCreateForImage(gdImagePtr image);
BGD_DECLARE(void) gdContextFlushImage(gdContextPtr context);
BGD_DECLARE(gdImagePtr) gdContextGetImage(gdContextPtr context);
BGD_DECLARE(void) gdContextDestroy(gdContextPtr context);
BGD_DECLARE(int) gdContextSave(gdContextPtr context);
BGD_DECLARE(int) gdContextRestore(gdContextPtr context);
BGD_DECLARE(int) gdContextClip(gdContextPtr context);
BGD_DECLARE(int) gdContextClipPreserve(gdContextPtr context);
BGD_DECLARE(void) gdContextNewPath(gdContextPtr context);
BGD_DECLARE(void) gdContextAppendPath(gdContextPtr context, gdPathPtr path);
BGD_DECLARE(void)
gdContextSetSourceRgba(gdContextPtr context, double r, double g, double b, double a);
BGD_DECLARE(void) gdContextSetSourceRgb(gdContextPtr context, double r, double g, double b);
BGD_DECLARE(void)
gdContextSetSourceImage(gdContextPtr context, gdImagePtr image, double x, double y);
BGD_DECLARE(void) gdContextSetSource(gdContextPtr context, gdPaintPtr source);
BGD_DECLARE(void) gdContextSetOperator(gdContextPtr context, gdCompositeOperator op);
BGD_DECLARE(void) gdContextSetOpacity(gdContextPtr context, double opacity);
BGD_DECLARE(void) gdContextMoveTo(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextRelMoveTo(gdContextPtr context, double dx, double dy);
BGD_DECLARE(void) gdContextLineTo(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextRelLineTo(gdContextPtr context, double dx, double dy);
BGD_DECLARE(void)
gdContextCurveTo(gdContextPtr context, double x1, double y1, double x2, double y2, double x3,
                 double y3);
BGD_DECLARE(void)
gdContextRelCurveTo(gdContextPtr context, double dx1, double dy1, double dx2, double dy2,
                    double dx3, double dy3);
BGD_DECLARE(void) gdContextQuadTo(gdContextPtr context, double x1, double y1, double x2, double y2);
BGD_DECLARE(void)
gdContextRelQuadTo(gdContextPtr context, double dx1, double dy1, double dx2, double dy2);
BGD_DECLARE(void)
gdContextArc(gdContextPtr context, double cx, double cy, double radius, double a0, double a1);
BGD_DECLARE(void)
gdContextNegativeArc(gdContextPtr context, double cx, double cy, double radius, double a0,
                     double a1);
BGD_DECLARE(void)
gdContextRectangle(gdContextPtr context, double x, double y, double width, double height);
BGD_DECLARE(void) gdContextClosePath(gdContextPtr context);
BGD_DECLARE(void) gdContextScale(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextTranslate(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextRotate(gdContextPtr context, double radians);
BGD_DECLARE(void) gdContextTransform(gdContextPtr context, const gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdContextSetLineWidth(gdContextPtr context, double width);
BGD_DECLARE(void) gdContextSetLineCap(gdContextPtr context, gdLineCap cap);
BGD_DECLARE(void) gdContextSetLineJoin(gdContextPtr context, gdLineJoin join);
BGD_DECLARE(void)
gdContextSetDash(gdContextPtr context, double offset, const double *data, int size);
BGD_DECLARE(void) gdContextSetFillRule(gdContextPtr context, gdFillRule rule);
BGD_DECLARE(void) gdContextStroke(gdContextPtr context);
BGD_DECLARE(void) gdContextStrokePreserve(gdContextPtr context);
BGD_DECLARE(void) gdContextFill(gdContextPtr context);
BGD_DECLARE(void) gdContextFillPreserve(gdContextPtr context);
BGD_DECLARE(void) gdContextPaint(gdContextPtr context);

BGD_DECLARE(gdPaintPtr) gdPaintCreateFromPattern(gdPathPatternPtr pattern);
BGD_DECLARE(gdPaintPtr) gdPaintCreateLinear(double x0, double y0, double x1, double y1);
BGD_DECLARE(gdPaintPtr)
gdPaintCreateRadial(double x0, double y0, double r0, double x1, double y1, double r1);
BGD_DECLARE(int)
gdPaintAddColorStopRgb(gdPaintPtr paint, double offset, double r, double g, double b);
BGD_DECLARE(int)
gdPaintAddColorStopRgba(gdPaintPtr paint, double offset, double r, double g, double b, double a);
BGD_DECLARE(int) gdPaintSetExtend(gdPaintPtr paint, gdExtendMode extend);
BGD_DECLARE(int) gdPaintSetMatrix(gdPaintPtr paint, const gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPaintDestroy(gdPaintPtr paint);

BGD_DECLARE(gdPathPatternPtr) gdPathPatternCreateForImage(gdImagePtr image);
BGD_DECLARE(void) gdPathPatternDestroy(gdPathPatternPtr pattern);
BGD_DECLARE(void) gdPathPatternSetExtend(gdPathPatternPtr pattern, gdExtendMode extend);
BGD_DECLARE(void) gdPathPatternSetMatrix(gdPathPatternPtr pattern, gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPathPatternSetOpacity(gdPathPatternPtr pattern, double opacity);

/**
 * Function: gdPathMatrixInit
 *
 * Initialize an affine transformation matrix from its six coefficients.
 *
 * A point is mapped to (x * m00 + y * m01 + m02,
 * x * m10 + y * m11 + m12).
 *
 * Parameters:
 *   matrix - The matrix to initialize.
 *   m00    - The horizontal x coefficient.
 *   m10    - The vertical x coefficient.
 *   m01    - The horizontal y coefficient.
 *   m11    - The vertical y coefficient.
 *   m02    - The horizontal translation.
 *   m12    - The vertical translation.
 *
 * See also:
 *   - <gdPathMatrixInitIdentity>
 *   - <gdPathMatrixMap>
 */
BGD_DECLARE(void)
gdPathMatrixInit(gdPathMatrixPtr matrix, double m00, double m10, double m01, double m11, double m02,
                 double m12);
/**
 * Function: gdPathMatrixInitIdentity
 *
 * Initialize a matrix to the identity transformation.
 *
 * Parameters:
 *   matrix - The matrix to initialize.
 */
BGD_DECLARE(void) gdPathMatrixInitIdentity(gdPathMatrixPtr matrix);

/**
 * Function: gdPathMatrixInitTranslate
 *
 * Initialize a translation matrix.
 *
 * Parameters:
 *   matrix - The matrix to initialize.
 *   x      - The horizontal translation.
 *   y      - The vertical translation.
 */
BGD_DECLARE(void) gdPathMatrixInitTranslate(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixInitScale
 *
 * Initialize a scaling matrix.
 *
 * Parameters:
 *   matrix - The matrix to initialize.
 *   x      - The horizontal scale factor.
 *   y      - The vertical scale factor.
 */
BGD_DECLARE(void) gdPathMatrixInitScale(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixInitShear
 *
 * Initialize a shear matrix. The shear factors are the tangents of the
 * supplied angles.
 *
 * Parameters:
 *   matrix - The matrix to initialize.
 *   x      - The horizontal shear angle, in radians.
 *   y      - The vertical shear angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixInitShear(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixInitRotate
 *
 * Initialize a rotation matrix about the origin.
 *
 * Parameters:
 *   matrix  - The matrix to initialize.
 *   radians - The rotation angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixInitRotate(gdPathMatrixPtr matrix, double radians);

/**
 * Function: gdPathMatrixInitRotateTranslate
 *
 * Initialize a rotation matrix about a specified point. The point (x, y)
 * remains unchanged by the resulting transformation.
 *
 * Parameters:
 *   matrix  - The matrix to initialize.
 *   radians - The rotation angle, in radians.
 *   x       - The horizontal coordinate of the rotation center.
 *   y       - The vertical coordinate of the rotation center.
 */
BGD_DECLARE(void)
gdPathMatrixInitRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);

/**
 * Function: gdPathMatrixTranslate
 *
 * Apply a translation before the transformation already in a matrix.
 *
 * Parameters:
 *   matrix - The matrix to modify.
 *   x      - The horizontal translation.
 *   y      - The vertical translation.
 */
BGD_DECLARE(void) gdPathMatrixTranslate(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixScale
 *
 * Apply scaling before the transformation already in a matrix.
 *
 * Parameters:
 *   matrix - The matrix to modify.
 *   x      - The horizontal scale factor.
 *   y      - The vertical scale factor.
 */
BGD_DECLARE(void) gdPathMatrixScale(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixShear
 *
 * Apply a shear before the transformation already in a matrix.
 *
 * Parameters:
 *   matrix - The matrix to modify.
 *   x      - The horizontal shear angle, in radians.
 *   y      - The vertical shear angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixShear(gdPathMatrixPtr matrix, double x, double y);

/**
 * Function: gdPathMatrixRotate
 *
 * Apply a rotation about the origin before the transformation already in a
 * matrix.
 *
 * Parameters:
 *   matrix  - The matrix to modify.
 *   radians - The rotation angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixRotate(gdPathMatrixPtr matrix, double radians);

/**
 * Function: gdPathMatrixRotateTranslate
 *
 * Apply a rotation about a specified point before the transformation already
 * in a matrix.
 *
 * Parameters:
 *   matrix  - The matrix to modify.
 *   radians - The rotation angle, in radians.
 *   x       - The horizontal coordinate of the rotation center.
 *   y       - The vertical coordinate of the rotation center.
 */
BGD_DECLARE(void)
gdPathMatrixRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);

/**
 * Function: gdPathMatrixMultiply
 *
 * Compose two affine transformations. The result maps through a first and
 * then through b. matrix may alias a or b.
 *
 * Parameters:
 *   matrix - The destination matrix.
 *   a      - The transformation applied first.
 *   b      - The transformation applied second.
 */
BGD_DECLARE(void)
gdPathMatrixMultiply(gdPathMatrixPtr matrix, const gdPathMatrixPtr a, const gdPathMatrixPtr b);

/**
 * Function: gdPathMatrixInvert
 *
 * Invert an affine transformation in place. A singular matrix is left
 * unchanged.
 *
 * Parameters:
 *   matrix - The matrix to invert.
 *
 * Returns:
 *   Non-zero on success, or zero if the matrix is singular.
 */
BGD_DECLARE(int) gdPathMatrixInvert(gdPathMatrixPtr matrix);

/**
 * Function: gdPathMatrixMap
 *
 * Transform a pair of coordinates.
 *
 * Parameters:
 *   matrix   - The transformation matrix.
 *   x        - The source horizontal coordinate.
 *   y        - The source vertical coordinate.
 *   result_x - Where to store the transformed horizontal coordinate.
 *   result_y - Where to store the transformed vertical coordinate.
 */
BGD_DECLARE(void)
gdPathMatrixMap(const gdPathMatrixPtr matrix, double x, double y, double *result_x,
                double *result_y);

/**
 * Function: gdPathMatrixMapPoint
 *
 * Transform a point. src and dst may point to the same object.
 *
 * Parameters:
 *   matrix - The transformation matrix.
 *   src    - The source point.
 *   dst    - Where to store the transformed point.
 */
BGD_DECLARE(void)
gdPathMatrixMapPoint(const gdPathMatrixPtr matrix, const gdPointFPtr src, gdPointFPtr dst);

/**
 * Function: gdPathMatrixMapRect
 *
 * Transform all four corners of a rectangle and calculate their axis-aligned
 * bounding box. src and dst may point to the same object.
 *
 * Parameters:
 *   matrix - The transformation matrix.
 *   src    - The source rectangle.
 *   dst    - Where to store the transformed bounding rectangle.
 */
BGD_DECLARE(void)
gdPathMatrixMapRect(const gdPathMatrixPtr matrix, const gdRectFPtr src, gdRectFPtr dst);

/**
 * Function: gdPathCreate
 *
 * Create an empty path.
 *
 * Returns:
 *   A new path, or NULL if allocation fails. Destroy it with <gdPathDestroy>.
 */
BGD_DECLARE(gdPathPtr) gdPathCreate(void);

/**
 * Function: gdPathDuplicate
 *
 * Create an independent copy of a path.
 *
 * Parameters:
 *   path - The path to copy. Must not be NULL.
 *
 * Returns:
 *   A new path, or NULL if path is NULL or allocation fails. Destroy the copy
 *   with <gdPathDestroy>.
 */
BGD_DECLARE(gdPathPtr) gdPathDuplicate(const gdPathPtr path);

/**
 * Function: gdPathDestroy
 *
 * Release a path. Passing NULL has no effect.
 *
 * Parameters:
 *   path - The path to release.
 */
BGD_DECLARE(void) gdPathDestroy(gdPathPtr path);

/**
 * Function: gdPathAppendPath
 *
 * Append all contours from one path to another.
 *
 * Parameters:
 *   path   - The destination path.
 *   source - The path to append.
 */
BGD_DECLARE(void) gdPathAppendPath(gdPathPtr path, const gdPathPtr source);

/**
 * Function: gdPathTransform
 *
 * Transform every point in a path in place.
 *
 * Parameters:
 *   path   - The path to transform.
 *   matrix - The transformation matrix.
 */
BGD_DECLARE(void) gdPathTransform(gdPathPtr path, const gdPathMatrixPtr matrix);

/**
 * Function: gdPathMoveTo
 *
 * Start a new contour at an absolute position.
 *
 * Parameters:
 *   path - The path to modify.
 *   x    - The horizontal coordinate.
 *   y    - The vertical coordinate.
 */
BGD_DECLARE(void) gdPathMoveTo(gdPathPtr path, double x, double y);

/**
 * Function: gdPathRelMoveTo
 *
 * Start a new contour at an offset from the current point. For an empty path,
 * the offset is relative to (0, 0).
 *
 * Parameters:
 *   path - The path to modify.
 *   dx   - The horizontal offset.
 *   dy   - The vertical offset.
 */
BGD_DECLARE(void) gdPathRelMoveTo(gdPathPtr path, double dx, double dy);

/**
 * Function: gdPathLineTo
 *
 * Add a straight line to an absolute position.
 *
 * Parameters:
 *   path - The path to modify.
 *   x    - The endpoint's horizontal coordinate.
 *   y    - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void) gdPathLineTo(gdPathPtr path, double x, double y);

/**
 * Function: gdPathRelLineTo
 *
 * Add a straight line using an offset from the current point.
 *
 * Parameters:
 *   path - The path to modify.
 *   dx   - The horizontal offset.
 *   dy   - The vertical offset.
 */
BGD_DECLARE(void) gdPathRelLineTo(gdPathPtr path, double dx, double dy);

/**
 * Function: gdPathQuadTo
 *
 * Add a quadratic Bezier curve.
 *
 * Parameters:
 *   path - The path to modify.
 *   x1   - The control point's horizontal coordinate.
 *   y1   - The control point's vertical coordinate.
 *   x2   - The endpoint's horizontal coordinate.
 *   y2   - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void) gdPathQuadTo(gdPathPtr path, double x1, double y1, double x2, double y2);

/**
 * Function: gdPathRelQuadTo
 *
 * Add a quadratic Bezier curve using offsets from the current point.
 *
 * Parameters:
 *   path - The path to modify.
 *   dx1  - The control point's horizontal offset.
 *   dy1  - The control point's vertical offset.
 *   dx2  - The endpoint's horizontal offset.
 *   dy2  - The endpoint's vertical offset.
 */
BGD_DECLARE(void) gdPathRelQuadTo(gdPathPtr path, double dx1, double dy1, double dx2, double dy2);

/**
 * Function: gdPathCurveTo
 *
 * Add a cubic Bezier curve.
 *
 * Parameters:
 *   path - The path to modify.
 *   x1   - The first control point's horizontal coordinate.
 *   y1   - The first control point's vertical coordinate.
 *   x2   - The second control point's horizontal coordinate.
 *   y2   - The second control point's vertical coordinate.
 *   x3   - The endpoint's horizontal coordinate.
 *   y3   - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void)
gdPathCurveTo(gdPathPtr path, double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * Function: gdPathRelCurveTo
 *
 * Add a cubic Bezier curve using offsets from the current point.
 *
 * Parameters:
 *   path - The path to modify.
 *   dx1  - The first control point's horizontal offset.
 *   dy1  - The first control point's vertical offset.
 *   dx2  - The second control point's horizontal offset.
 *   dy2  - The second control point's vertical offset.
 *   dx3  - The endpoint's horizontal offset.
 *   dy3  - The endpoint's vertical offset.
 */
BGD_DECLARE(void)
gdPathRelCurveTo(gdPathPtr path, double dx1, double dy1, double dx2, double dy2, double dx3,
                 double dy3);

/**
 * Function: gdPathArc
 *
 * Add a circular arc in the positive-angle direction. Angles are in radians.
 * A line is added from the current point to the beginning of the arc when
 * necessary.
 *
 * Parameters:
 *   path   - The path to modify.
 *   cx     - The center's horizontal coordinate.
 *   cy     - The center's vertical coordinate.
 *   radius - The arc radius.
 *   angle1 - The starting angle in radians.
 *   angle2 - The ending angle in radians.
 */
BGD_DECLARE(void)
gdPathArc(gdPathPtr path, double cx, double cy, double radius, double angle1, double angle2);

/**
 * Function: gdPathNegativeArc
 *
 * Add a circular arc in the negative-angle direction. Angles are in radians.
 * A line is added from the current point to the beginning of the arc when
 * necessary.
 *
 * Parameters:
 *   path   - The path to modify.
 *   cx     - The center's horizontal coordinate.
 *   cy     - The center's vertical coordinate.
 *   radius - The arc radius.
 *   angle1 - The starting angle in radians.
 *   angle2 - The ending angle in radians.
 */
BGD_DECLARE(void)
gdPathNegativeArc(gdPathPtr path, double cx, double cy, double radius, double angle1,
                  double angle2);

/**
 * Function: gdPathArcTo
 *
 * Connect the current point to (x1, y1) and (x2, y2) with a circular arc
 * tangent to both line segments. Degenerate geometry or a non-positive radius
 * adds a line to (x1, y1).
 *
 * Parameters:
 *   path   - The path to modify.
 *   x1     - The corner's horizontal coordinate.
 *   y1     - The corner's vertical coordinate.
 *   x2     - The second tangent line's horizontal endpoint.
 *   y2     - The second tangent line's vertical endpoint.
 *   radius - The arc radius.
 */
BGD_DECLARE(void)
gdPathArcTo(gdPathPtr path, double x1, double y1, double x2, double y2, double radius);

/**
 * Function: gdPathRectangle
 *
 * Add a closed rectangular contour.
 *
 * Parameters:
 *   path   - The path to modify.
 *   x      - The rectangle's left coordinate.
 *   y      - The rectangle's top coordinate.
 *   width  - The rectangle width.
 *   height - The rectangle height.
 */
BGD_DECLARE(void) gdPathRectangle(gdPathPtr path, double x, double y, double width, double height);

/**
 * Function: gdPathClose
 *
 * Close the current contour with a line to its starting point. An empty path
 * or an already closed contour is unchanged.
 *
 * Parameters:
 *   path - The path to modify.
 */
BGD_DECLARE(void) gdPathClose(gdPathPtr path);

#ifdef __cplusplus
}
#endif
#endif
