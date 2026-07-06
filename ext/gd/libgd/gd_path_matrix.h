#ifndef GD_PATH_MATRIX_H
#define GD_PATH_MATRIX_H

void gdPathMatrixInit(gdPathMatrixPtr matrix, double m00, double m10, double m01, double m11,
                      double m02, double m12);
BGD_DECLARE(void) gdPathMatrixInitIdentity(gdPathMatrixPtr matrix);
BGD_DECLARE(void) gdPathMatrixInitTranslate(gdPathMatrixPtr matrix, double x, double y);
BGD_DECLARE(void) gdPathMatrixInitScale(gdPathMatrixPtr matrix, double x, double y);
void gdPathMatrixInitShear(gdPathMatrixPtr matrix, double x, double y);
void gdPathMatrixInitRotate(gdPathMatrixPtr matrix, double radians);
void gdPathMatrixInitRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);
void gdPathMatrixTranslate(gdPathMatrixPtr matrix, double x, double y);
BGD_DECLARE(void) gdPathMatrixScale(gdPathMatrixPtr matrix, double x, double y);
void gdPathMatrixShear(gdPathMatrixPtr matrix, double x, double y);
BGD_DECLARE(void) gdPathMatrixRotate(gdPathMatrixPtr matrix, double radians);
void gdPathMatrixRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);
void gdPathMatrixMultiply(gdPathMatrixPtr matrix, const gdPathMatrixPtr a, const gdPathMatrixPtr b);
int gdPathMatrixInvert(gdPathMatrixPtr matrix);
void gdPathMatrixMap(const gdPathMatrixPtr matrix, double x, double y, double *_x, double *_y);
void gdPathMatrixMapPoint(const gdPathMatrixPtr matrix, const gdPointFPtr src, gdPointFPtr dst);
void gdPathMatrixMapRect(const gdPathMatrixPtr matrix, const gdRectFPtr src, gdRectFPtr dst);

double _gd_matrix_transformed_circle_major_axis(const gdPathMatrixPtr matrix, double radius);
int _matrix_has_unity_scale(const gdPathMatrixPtr matrix);

#endif // GD_PATH_MATRIX_H
