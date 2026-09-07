#ifndef GD_PATH_MATRIX_H
#define GD_PATH_MATRIX_H

#include "gd_vector2d.h"

/* Largest semiaxis after transforming a circle: radius times the largest
 * singular value of the matrix's linear component. Translation is ignored. */
double _gd_matrix_transformed_circle_major_axis(const gdPathMatrixPtr matrix, double radius);

#endif /* GD_PATH_MATRIX_H */
