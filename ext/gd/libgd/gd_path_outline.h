#ifndef GD_PATH_OUTLINE_H
#define GD_PATH_OUTLINE_H

#include "ftraster/gd_ft_raster.h"
#include "gd_vector2d_private.h"

/* Internal gdPath <-> FreeType outline bridge. */
GD_FT_Outline *gd_ft_outline_create(int points, int contours);
void gd_ft_outline_close(GD_FT_Outline *outline);
void gd_ft_outline_end(GD_FT_Outline *outline);
void gd_ft_outline_move_to(GD_FT_Outline *outline, double x, double y);
void gd_ft_outline_line_to(GD_FT_Outline *outline, double x, double y);
void gd_ft_outline_cubic_to(GD_FT_Outline *outline, double x1, double y1, double x2, double y2,
                            double x3, double y3);
void gd_ft_outline_conic_to(GD_FT_Outline *outline, double x1, double y1, double x2, double y2);
GD_FT_Outline *gd_ft_outline_convert(const gdPathPtr path, const gdPathMatrixPtr matrix);
void gd_ft_outline_destroy(GD_FT_Outline *outline);

#endif
