#include "gd.h"
#include <math.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/**
 * Title: Matrix
 * Group: Affine Matrix
 */

/**
 * Function: gdAffineApplyToPointF
 *  Applies an affine transformation to a point (floating point
 *  gdPointF)
 *
 *
 * Parameters:
 * 	dst - Where to store the resulting point
 *  affine - Source Point
 *  flip_horz - affine matrix
 *
 * Returns:
 *  GD_TRUE if the affine is rectilinear or GD_FALSE
 */
int gdAffineApplyToPointF (gdPointFPtr dst, const gdPointFPtr src,
		  const double affine[6])
{
	double x = src->x;
	double y = src->y;
	dst->x = x * affine[0] + y * affine[2] + affine[4];
	dst->y = x * affine[1] + y * affine[3] + affine[5];
	return GD_TRUE;
}

/**
 * Function: gdAffineInvert
 *  Find the inverse of an affine transformation.
 *
 * All non-degenerate affine transforms are invertible. Applying the
 * inverted matrix will restore the original values. Multiplying <src>
 * by <dst> (commutative) will return the identity affine (rounding
 * error possible).
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 *  src_affine - Original affine matrix
 *  flip_horz - Whether or not to flip horizontally
 *  flip_vert - Whether or not to flip vertically
 *
 * See also:
 *  <gdAffineIdentity>
 *
 * Returns:
 *  GD_TRUE on success or GD_FALSE on failure
 */
int gdAffineInvert (double dst[6], const double src[6])
{
	double r_det = (src[0] * src[3] - src[1] * src[2]);

	if (r_det <= 0.0) {
		return GD_FALSE;
	}

	r_det = 1.0 / r_det;
	dst[0] = src[3] * r_det;
	dst[1] = -src[1] * r_det;
	dst[2] = -src[2] * r_det;
	dst[3] = src[0] * r_det;
	dst[4] = -src[4] * dst[0] - src[5] * dst[2];
	dst[5] = -src[4] * dst[1] - src[5] * dst[3];
	return GD_TRUE;
}

/**
 * Function: gdAffineFlip
 *  Flip an affine transformation horizontally or vertically.
 *
 * Flips the affine transform, giving GD_FALSE for <flip_horz> and
 * <flip_vert> will clone the affine matrix. GD_TRUE for both will
 * copy a 180° rotation.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 *  src_affine - Original affine matrix
 *  flip_h - Whether or not to flip horizontally
 *  flip_v - Whether or not to flip vertically
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineFlip (double dst[6], const double src[6], const int flip_h, const int flip_v)
{
	dst[0] = flip_h ? - src[0] : src[0];
	dst[1] = flip_h ? - src[1] : src[1];
	dst[2] = flip_v ? - src[2] : src[2];
	dst[3] = flip_v ? - src[3] : src[3];
	dst[4] = flip_h ? - src[4] : src[4];
	dst[5] = flip_v ? - src[5] : src[5];
	return GD_TRUE;
}

/**
 * Function: gdAffineConcat
 * Concat (Multiply) two affine transformation matrices.
 *
 * Concats two affine transforms together, i.e. the result
 * will be the equivalent of doing first the transformation m1 and then
 * m2. All parameters can be the same matrix (safe to call using
 * the same array for all three arguments).
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 *  m1 - First affine matrix
 *  m2 - Second affine matrix
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineConcat (double dst[6], const double m1[6], const double m2[6])
{
	double dst0, dst1, dst2, dst3, dst4, dst5;

	dst0 = m1[0] * m2[0] + m1[1] * m2[2];
	dst1 = m1[0] * m2[1] + m1[1] * m2[3];
	dst2 = m1[2] * m2[0] + m1[3] * m2[2];
	dst3 = m1[2] * m2[1] + m1[3] * m2[3];
	dst4 = m1[4] * m2[0] + m1[5] * m2[2] + m2[4];
	dst5 = m1[4] * m2[1] + m1[5] * m2[3] + m2[5];
	dst[0] = dst0;
	dst[1] = dst1;
	dst[2] = dst2;
	dst[3] = dst3;
	dst[4] = dst4;
	dst[5] = dst5;
	return GD_TRUE;
}

/**
 * Function: gdAffineIdentity
 * Set up the identity matrix.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineIdentity (double dst[6])
{
	dst[0] = 1;
	dst[1] = 0;
	dst[2] = 0;
	dst[3] = 1;
	dst[4] = 0;
	dst[5] = 0;
	return GD_TRUE;
}

/**
 * Function: gdAffineScale
 * Set up a scaling matrix.
 *
 * Parameters:
 * 	scale_x - X scale factor
 * 	scale_y - Y scale factor
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineScale (double dst[6], const double scale_x, const double scale_y)
{
	dst[0] = scale_x;
	dst[1] = 0;
	dst[2] = 0;
	dst[3] = scale_y;
	dst[4] = 0;
	dst[5] = 0;
	return GD_TRUE;
}

/**
 * Function: gdAffineRotate
 * Set up a rotation affine transform.
 *
 * Like the other angle in libGD, in which increasing y moves
 * downward, this is a counterclockwise rotation.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 * 	angle - Rotation angle in degrees
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineRotate (double dst[6], const double angle)
{
	const double sin_t = sin (angle * M_PI / 180.0);
	const double cos_t = cos (angle * M_PI / 180.0);

	dst[0] = cos_t;
	dst[1] = sin_t;
	dst[2] = -sin_t;
	dst[3] = cos_t;
	dst[4] = 0;
	dst[5] = 0;
	return GD_TRUE;
}

/**
 * Function: gdAffineShearHorizontal
 * Set up a horizontal shearing matrix || becomes \\.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 * 	angle - Shear angle in degrees
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineShearHorizontal(double dst[6], const double angle)
{
	dst[0] = 1;
	dst[1] = 0;
	dst[2] = tan(angle * M_PI / 180.0);
	dst[3] = 1;
	dst[4] = 0;
	dst[5] = 0;
  	return GD_TRUE;
}

/**
 * Function: gdAffineShearVertical
 * Set up a vertical shearing matrix, columns are untouched.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 * 	angle - Shear angle in degrees
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineShearVertical(double dst[6], const double angle)
{
	dst[0] = 1;
	dst[1] = tan(angle * M_PI / 180.0);
	dst[2] = 0;
	dst[3] = 1;
	dst[4] = 0;
	dst[5] = 0;
  	return GD_TRUE;
}

/**
 * Function: gdAffineTranslate
 * Set up a translation matrix.
 *
 * Parameters:
 * 	dst - Where to store the resulting affine transform
 * 	offset_x - Horizontal translation amount
 * 	offset_y - Vertical translation amount
 *
 * Returns:
 *  GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineTranslate (double dst[6], const double offset_x, const double offset_y)
{
	dst[0] = 1;
	dst[1] = 0;
	dst[2] = 0;
	dst[3] = 1;
	dst[4] = offset_x;
	dst[5] = offset_y;
    return GD_TRUE;
}

/**
 * gdAffineexpansion: Find the affine's expansion factor.
 * @src: The affine transformation.
 *
 * Finds the expansion factor, i.e. the square root of the factor
 * by which the affine transform affects area. In an affine transform
 * composed of scaling, rotation, shearing, and translation, returns
 * the amount of scaling.
 *
 *  GD_SUCCESS on success or GD_FAILURE
 **/
double gdAffineExpansion (const double src[6])
{
  return sqrt (fabs (src[0] * src[3] - src[1] * src[2]));
}

/**
 * Function: gdAffineRectilinear
 * Determines whether the affine transformation is axis aligned. A
 * tolerance has been implemented using GD_EPSILON.
 *
 * Parameters:
 * 	m - The affine transformation
 *
 * Returns:
 *  GD_TRUE if the affine is rectilinear or GD_FALSE
 */
int gdAffineRectilinear (const double m[6])
{
  return ((fabs (m[1]) < GD_EPSILON && fabs (m[2]) < GD_EPSILON) ||
	  (fabs (m[0]) < GD_EPSILON && fabs (m[3]) < GD_EPSILON));
}

/**
 * Function: gdAffineEqual
 * Determines whether two affine transformations are equal. A tolerance
 * has been implemented using GD_EPSILON.
 *
 * Parameters:
 * 	m1 - The first affine transformation
 * 	m2 - The first affine transformation
 *
 * Returns:
 * 	GD_SUCCESS on success or GD_FAILURE
 */
int gdAffineEqual (const double m1[6], const double m2[6])
{
  return (fabs (m1[0] - m2[0]) < GD_EPSILON &&
	  fabs (m1[1] - m2[1]) < GD_EPSILON &&
	  fabs (m1[2] - m2[2]) < GD_EPSILON &&
	  fabs (m1[3] - m2[3]) < GD_EPSILON &&
	  fabs (m1[4] - m2[4]) < GD_EPSILON &&
	  fabs (m1[5] - m2[5]) < GD_EPSILON);
}
