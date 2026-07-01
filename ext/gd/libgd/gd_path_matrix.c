#include <math.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_vector2d_private.h"
#include "gd_path_matrix.h"

void gdPathMatrixInit(gdPathMatrixPtr matrix,
                      double m00, double m10,
                      double m01, double m11,
                      double m02, double m12)
{
    matrix->m00 = m00; matrix->m10 = m10;
    matrix->m01 = m01; matrix->m11 = m11;
    matrix->m02 = m02; matrix->m12 = m12;
}

BGD_DECLARE(void) gdPathMatrixInitIdentity(gdPathMatrixPtr matrix)
{
    matrix->m00 = 1.0; matrix->m10 = 0.0;
    matrix->m01 = 0.0; matrix->m11 = 1.0;
    matrix->m02 = 0.0; matrix->m12 = 0.0;
}

BGD_DECLARE(void) gdPathMatrixInitTranslate(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrixInit(matrix, 1.0, 0.0, 0.0, 1.0, x, y);
}

BGD_DECLARE(void) gdPathMatrixInitScale(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrixInit(matrix, x, 0.0, 0.0, y, 0.0, 0.0);
}

void gdPathMatrixInitShear(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrixInit(matrix, 1.0, tan(y), tan(x), 1.0, 0.0, 0.0);
}

void gdPathMatrixInitRotate(gdPathMatrixPtr matrix, double radians)
{
    double c = cos(radians);
    double s = sin(radians);

    gdPathMatrixInit(matrix, c, s, -s, c, 0.0, 0.0);
}

void gdPathMatrixInitRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y)
{
    double c = cos(radians);
    double s = sin(radians);

    double cx = x * (1 - c) + y * s;
    double cy = y * (1 - c) - x * s;

    gdPathMatrixInit(matrix, c, s, -s, c, cx, cy);
}

void gdPathMatrixTranslate(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrix m;
    gdPathMatrixInitTranslate(&m, x, y);
    gdPathMatrixMultiply(matrix, &m, matrix);
}

BGD_DECLARE(void) gdPathMatrixScale(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrix m;
    gdPathMatrixInitScale(&m, x, y);
    gdPathMatrixMultiply(matrix, &m, matrix);
}

void gdPathMatrixShear(gdPathMatrixPtr matrix, double x, double y)
{
    gdPathMatrix m;
    gdPathMatrixInitShear(&m, x, y);
    gdPathMatrixMultiply(matrix, &m, matrix);
}

BGD_DECLARE(void) gdPathMatrixRotate(gdPathMatrixPtr matrix, double radians)
{
    gdPathMatrix m;
    gdPathMatrixInitRotate(&m, radians);
    gdPathMatrixMultiply(matrix, &m, matrix);
}

void gdPathMatrixRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y)
{
    gdPathMatrix m;
    gdPathMatrixInitRotateTranslate(&m, radians, x, y);
    gdPathMatrixMultiply(matrix, &m, matrix);
}

void gdPathMatrixMultiply(gdPathMatrixPtr matrix, const gdPathMatrixPtr a, const gdPathMatrixPtr b)
{
    double m00 = a->m00 * b->m00 + a->m10 * b->m01;
    double m10 = a->m00 * b->m10 + a->m10 * b->m11;
    double m01 = a->m01 * b->m00 + a->m11 * b->m01;
    double m11 = a->m01 * b->m10 + a->m11 * b->m11;
    double m02 = a->m02 * b->m00 + a->m12 * b->m01 + b->m02;
    double m12 = a->m02 * b->m10 + a->m12 * b->m11 + b->m12;

    gdPathMatrixInit(matrix, m00, m10, m01, m11, m02, m12);
}

int gdPathMatrixInvert(gdPathMatrixPtr matrix)
{
    double det = (matrix->m00 * matrix->m11 - matrix->m10 * matrix->m01);
    if(det == 0.0)
        return 0;

    double inv_det = 1.0 / det;
    double m00 = matrix->m00 * inv_det;
    double m10 = matrix->m10 * inv_det;
    double m01 = matrix->m01 * inv_det;
    double m11 = matrix->m11 * inv_det;
    double m02 = (matrix->m01 * matrix->m12 - matrix->m11 * matrix->m02) * inv_det;
    double m12 = (matrix->m10 * matrix->m02 - matrix->m00 * matrix->m12) * inv_det;

    gdPathMatrixInit(matrix, m11, -m10, -m01, m00, m02, m12);
    return 1;
}

void gdPathMatrixMap(const gdPathMatrixPtr matrix, double x, double y, double* _x, double* _y)
{
    *_x = x * matrix->m00 + y * matrix->m01 + matrix->m02;
    *_y = x * matrix->m10 + y * matrix->m11 + matrix->m12;
}

void gdPathMatrixMapPoint(const gdPathMatrixPtr matrix, const gdPointFPtr src, gdPointFPtr dst)
{
    gdPathMatrixMap(matrix, src->x, src->y, &dst->x, &dst->y);
}

void gdPathMatrixMapRect(const gdPathMatrixPtr matrix, const gdRectFPtr src, gdRectFPtr dst)
{
    gdPointF p[4];
    p[0].x = src->x;
    p[0].y = src->y;
    p[1].x = src->x + src->w;
    p[1].y = src->y;
    p[2].x = src->x + src->w;
    p[2].y = src->y + src->h;
    p[3].x = src->x;
    p[3].y = src->y + src->h;

    gdPathMatrixMapPoint(matrix, &p[0], &p[0]);
    gdPathMatrixMapPoint(matrix, &p[1], &p[1]);
    gdPathMatrixMapPoint(matrix, &p[2], &p[2]);
    gdPathMatrixMapPoint(matrix, &p[3], &p[3]);

    double l = p[0].x;
    double t = p[0].y;
    double r = p[0].x;
    double b = p[0].y;

    for(int i = 1;i < 4;i++)
    {
        if(p[i].x < l) l = p[i].x;
        if(p[i].x > r) r = p[i].x;
        if(p[i].y < t) t = p[i].y;
        if(p[i].y > b) b = p[i].y;
    }

    dst->x = l;
    dst->y = t;
    dst->w = r - l;
    dst->h = b - t;
}

#define SCALING_EPSILON (double)(1.0 / 256.0)
double _matrix_compute_determinant (const gdPathMatrixPtr matrix)
{
    double a, b, c, d;

    a = matrix->m00; b = matrix->m10;
    c = matrix->m01; d = matrix->m11;

    return a*d - b*c;
}

void
_matrix_get_affine (const gdPathMatrixPtr matrix,
			  double *m00, double *m10,
			  double *m01, double *m11,
			  double *m02, double *m12)
{
    *m00  = matrix->m00;
    *m10  = matrix->m10;

    *m01  = matrix->m01;
    *m11  = matrix->m11;

    if (m02)
	*m02 = matrix->m02;
    if (m12)
	*m12 = matrix->m12;
}

int _matrix_has_unity_scale (const gdPathMatrixPtr matrix)
{
    /* check that the determinant is near +/-1 */
    double det = _matrix_compute_determinant (matrix);
    if (fabs (det * det - 1.0) < SCALING_EPSILON) {
	/* check that one axis is close to zero */
	if (fabs (matrix->m01) < SCALING_EPSILON  &&
	    fabs (matrix->m10) < SCALING_EPSILON)
	    return 1;
	if (fabs (matrix->m00) < SCALING_EPSILON  &&
	    fabs (matrix->m11) < SCALING_EPSILON)
	    return 1;
	/* If rotations are allowed then it must instead test for
	 * orthogonality. This is m00*m01+m10*m11 ~= 0.
	 */
    }
    return 0;
}

double _gd_matrix_transformed_circle_major_axis (const gdPathMatrixPtr matrix, double radius)
{
    double  a, b, c, d, f, g, h, i, j;

    if (_matrix_has_unity_scale (matrix)) return radius;

    _matrix_get_affine (matrix,
                              &a, &b,
                              &c, &d,
                              NULL, NULL);

    i = a*a + b*b;
    j = c*c + d*d;

    f = 0.5 * (i + j);
    g = 0.5 * (i - j);
    h = a*c + b*d;

    return radius * sqrt (f + hypot (g, h));

    /*
     * we don't need the minor axis length, which is
     * double min = radius * sqrt (f - sqrt (g*g+h*h));
     */
}
