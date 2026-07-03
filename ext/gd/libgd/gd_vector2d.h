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

typedef enum {
	GD_EXTEND_NONE, GD_EXTEND_REPEAT, GD_EXTEND_REFLECT, GD_EXTEND_PAD
} gdExtendMode;
typedef enum { gdLineCapButt, gdLineCapRound, gdLineCapSquare } gdLineCap;
typedef enum { gdLineJoinMiter, gdLineJoinRound, gdLineJoinBevel } gdLineJoin;
typedef enum { gdFillRuleNonZero, gdFillRuleEvenOdd } gdFillRule;
/* Compatibility for the spelling used by the original experimental branch. */
#define gdFillRulEvenOdd gdFillRuleEvenOdd

typedef enum {
	GD_OP_CLEAR, GD_OP_SOURCE, GD_OP_OVER, GD_OP_IN, GD_OP_OUT, GD_OP_ATOP,
	GD_OP_DEST, GD_OP_DEST_OVER, GD_OP_DEST_IN, GD_OP_DEST_OUT, GD_OP_DEST_ATOP,
	GD_OP_XOR, GD_OP_ADD, GD_OP_SATURATE, GD_OP_MULTIPLY, GD_OP_SCREEN,
	GD_OP_OVERLAY, GD_OP_DARKEN, GD_OP_LIGHTEN, GD_OP_COLOR_DODGE,
	GD_OP_COLOR_BURN, GD_OP_HARD_LIGHT, GD_OP_SOFT_LIGHT, GD_OP_DIFFERENCE,
	GD_OP_EXCLUSION, GD_OP_HSL_HUE, GD_OP_HSL_SATURATION, GD_OP_HSL_COLOR,
	GD_OP_HSL_LUMINOSITY, GD_OP_COUNT
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
BGD_DECLARE(void) gdContextSetSourceRgba(gdContextPtr context, double r, double g, double b, double a);
BGD_DECLARE(void) gdContextSetSourceRgb(gdContextPtr context, double r, double g, double b);
BGD_DECLARE(void) gdContextSetSourceImage(gdContextPtr context, gdImagePtr image, double x, double y);
BGD_DECLARE(void) gdContextSetSource(gdContextPtr context, gdPaintPtr source);
BGD_DECLARE(void) gdContextSetOperator(gdContextPtr context, gdCompositeOperator op);
BGD_DECLARE(void) gdContextSetOpacity(gdContextPtr context, double opacity);
BGD_DECLARE(void) gdContextMoveTo(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextLineTo(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextRelLineTo(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextCurveTo(gdContextPtr context, double x1, double y1, double x2, double y2, double x3, double y3);
BGD_DECLARE(void) gdContextQuadTo(gdContextPtr context, double x1, double y1, double x2, double y2);
BGD_DECLARE(void) gdContextArc(gdContextPtr context, double cx, double cy, double radius, double a0, double a1);
BGD_DECLARE(void) gdContextNegativeArc(gdContextPtr context, double cx, double cy, double radius, double a0, double a1);
BGD_DECLARE(void) gdContextRectangle(gdContextPtr context, double x, double y, double width, double height);
BGD_DECLARE(void) gdContextClosePath(gdContextPtr context);
BGD_DECLARE(void) gdContextScale(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextTranslate(gdContextPtr context, double x, double y);
BGD_DECLARE(void) gdContextRotate(gdContextPtr context, double radians);
BGD_DECLARE(void) gdContextTransform(gdContextPtr context, const gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdContextSetLineWidth(gdContextPtr context, double width);
BGD_DECLARE(void) gdContextSetLineCap(gdContextPtr context, gdLineCap cap);
BGD_DECLARE(void) gdContextSetLineJoin(gdContextPtr context, gdLineJoin join);
BGD_DECLARE(void) gdContextSetDash(gdContextPtr context, double offset, const double *data, int size);
BGD_DECLARE(void) gdContextSetFillRule(gdContextPtr context, gdFillRule rule);
BGD_DECLARE(void) gdContextStroke(gdContextPtr context);
BGD_DECLARE(void) gdContextStrokePreserve(gdContextPtr context);
BGD_DECLARE(void) gdContextFill(gdContextPtr context);
BGD_DECLARE(void) gdContextFillPreserve(gdContextPtr context);
BGD_DECLARE(void) gdContextPaint(gdContextPtr context);

BGD_DECLARE(gdPaintPtr) gdPaintCreateFromPattern(gdPathPatternPtr pattern);
BGD_DECLARE(gdPaintPtr) gdPaintCreateLinear(double x0, double y0, double x1, double y1);
BGD_DECLARE(gdPaintPtr) gdPaintCreateRadial(double x0, double y0, double r0, double x1, double y1, double r1);
BGD_DECLARE(int) gdPaintAddColorStopRgb(gdPaintPtr paint, double offset, double r, double g, double b);
BGD_DECLARE(int) gdPaintAddColorStopRgba(gdPaintPtr paint, double offset, double r, double g, double b, double a);
BGD_DECLARE(int) gdPaintSetExtend(gdPaintPtr paint, gdExtendMode extend);
BGD_DECLARE(int) gdPaintSetMatrix(gdPaintPtr paint, const gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPaintDestroy(gdPaintPtr paint);

BGD_DECLARE(gdPathPatternPtr) gdPathPatternCreateForImage(gdImagePtr image);
BGD_DECLARE(void) gdPathPatternDestroy(gdPathPatternPtr pattern);
BGD_DECLARE(void) gdPathPatternSetExtend(gdPathPatternPtr pattern, gdExtendMode extend);
BGD_DECLARE(void) gdPathPatternSetMatrix(gdPathPatternPtr pattern, gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPathPatternSetOpacity(gdPathPatternPtr pattern, double opacity);

BGD_DECLARE(void) gdPathMatrixInitIdentity(gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPathMatrixInitScale(gdPathMatrixPtr matrix, double x, double y);
BGD_DECLARE(void) gdPathMatrixInitTranslate(gdPathMatrixPtr matrix, double x, double y);
BGD_DECLARE(void) gdPathMatrixRotate(gdPathMatrixPtr matrix, double radians);
BGD_DECLARE(void) gdPathMatrixScale(gdPathMatrixPtr matrix, double x, double y);

BGD_DECLARE(gdPathPtr) gdPathCreate(void);
BGD_DECLARE(void) gdPathDestroy(gdPathPtr path);
BGD_DECLARE(void) gdPathAppendPath(gdPathPtr path, const gdPathPtr source);
BGD_DECLARE(void) gdPathTransform(gdPathPtr path, const gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPathMoveTo(gdPathPtr path, double x, double y);
BGD_DECLARE(void) gdPathLineTo(gdPathPtr path, double x, double y);
BGD_DECLARE(void) gdPathRelLineTo(gdPathPtr path, double dx, double dy);
BGD_DECLARE(void) gdPathQuadTo(gdPathPtr path, double x1, double y1, double x2, double y2);
BGD_DECLARE(void) gdPathCurveTo(gdPathPtr path, double x1, double y1, double x2, double y2, double x3, double y3);
BGD_DECLARE(void) gdPathClose(gdPathPtr path);

#ifdef __cplusplus
}
#endif
#endif
