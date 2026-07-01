#include "gd_vector2d_private.h"
#include "gdhelpers.h"
#include "gd_path.h"
#include "gd_path_matrix.h"
#include "gd_path_outline.h"
#include "ftraster/gd_ft_stroker.h"
#include "ftraster/gd_ft_raster.h"
#include <math.h>

static gdPathPtr ft_outline_to_gdpath(const GD_FT_Outline* outline)
{
    if (!outline || outline->n_points == 0)
        return NULL;

    gdPathPtr path = gdPathCreate();
    if (!path)
        return NULL;

    int first = 0;
    for (int contour = 0; contour < outline->n_contours; contour++) {
        int last = outline->contours[contour];
        int point = first;
        GD_FT_Vector start;

        if (last < first)
            goto invalid_outline;

        char first_tag = GD_FT_CURVE_TAG(outline->tags[first]);
        char last_tag = GD_FT_CURVE_TAG(outline->tags[last]);

        if (first_tag == GD_FT_CURVE_TAG_ON) {
            start = outline->points[point++];
        } else if (first_tag == GD_FT_CURVE_TAG_CONIC) {
            if (last_tag == GD_FT_CURVE_TAG_ON) {
                start = outline->points[last--];
            } else if (last_tag == GD_FT_CURVE_TAG_CONIC) {
                start.x = (outline->points[first].x + outline->points[last].x) / 2;
                start.y = (outline->points[first].y + outline->points[last].y) / 2;
            } else {
                goto invalid_outline;
            }
        } else {
            goto invalid_outline;
        }

        gdPathMoveTo(path, start.x / 64.0, start.y / 64.0);

        while (point <= last) {
            char tag = GD_FT_CURVE_TAG(outline->tags[point]);
            GD_FT_Vector current = outline->points[point++];

            if (tag == GD_FT_CURVE_TAG_ON) {
                gdPathLineTo(path, current.x / 64.0, current.y / 64.0);
                continue;
            }

            if (tag == GD_FT_CURVE_TAG_CONIC) {
                GD_FT_Vector control = current;
                for (;;) {
                    if (point > last) {
                        gdPathQuadTo(path, control.x / 64.0, control.y / 64.0,
                                    start.x / 64.0, start.y / 64.0);
                        break;
                    }

                    tag = GD_FT_CURVE_TAG(outline->tags[point]);
                    current = outline->points[point++];
                    if (tag == GD_FT_CURVE_TAG_ON) {
                        gdPathQuadTo(path, control.x / 64.0, control.y / 64.0,
                                    current.x / 64.0, current.y / 64.0);
                        break;
                    }
                    if (tag != GD_FT_CURVE_TAG_CONIC)
                        goto invalid_outline;

                    GD_FT_Vector middle = {
                        (control.x + current.x) / 2,
                        (control.y + current.y) / 2
                    };
                    gdPathQuadTo(path, control.x / 64.0, control.y / 64.0,
                                middle.x / 64.0, middle.y / 64.0);
                    control = current;
                }
                continue;
            }

            if (tag != GD_FT_CURVE_TAG_CUBIC || point > last ||
                GD_FT_CURVE_TAG(outline->tags[point]) != GD_FT_CURVE_TAG_CUBIC)
                goto invalid_outline;

            GD_FT_Vector control1 = current;
            GD_FT_Vector control2 = outline->points[point++];
            GD_FT_Vector end = start;
            if (point <= last) {
                if (GD_FT_CURVE_TAG(outline->tags[point]) != GD_FT_CURVE_TAG_ON)
                    goto invalid_outline;
                end = outline->points[point++];
            }
            gdPathCurveTo(path, control1.x / 64.0, control1.y / 64.0,
                          control2.x / 64.0, control2.y / 64.0,
                          end.x / 64.0, end.y / 64.0);
        }

        gdPathClose(path);
        first = outline->contours[contour] + 1;
    }

    return path;

invalid_outline:
    gdPathDestroy(path);
    return NULL;
}

GD_VECTOR2D_INTERNAL gdPathPtr gdPathStrokeToPath(const gdPathPtr path, const gdStrokePtr stroke, const gdPathMatrixPtr matrix)
{
    if (!path || !stroke || stroke->width <= 0)
        return NULL;

    GD_FT_Outline *outline = gd_ft_outline_convert(path, matrix);
    if (!outline)
        return NULL;

    GD_FT_Stroker stroker;
    GD_FT_Stroker_New(&stroker);

    double radius = stroke->width / 2.0;
    GD_FT_Fixed ftWidth = (GD_FT_Fixed)(radius * 64);
    GD_FT_Fixed ftMiterLimit = (GD_FT_Fixed)(stroke->miterlimit * 65536);

    GD_FT_Stroker_LineCap ftCap;
    switch (stroke->cap)
    {
        case gdLineCapSquare:
            ftCap = GD_FT_STROKER_LINECAP_SQUARE;
            break;
        case gdLineCapRound:
            ftCap = GD_FT_STROKER_LINECAP_ROUND;
            break;
        case gdLineCapButt:
        default:
            ftCap = GD_FT_STROKER_LINECAP_BUTT;
            break;
    }

    GD_FT_Stroker_LineJoin ftJoin;
    switch (stroke->join)
    {
        case gdLineJoinBevel:
            ftJoin = GD_FT_STROKER_LINEJOIN_BEVEL;
            break;
        case gdLineJoinRound:
            ftJoin = GD_FT_STROKER_LINEJOIN_ROUND;
            break;
        case gdLineJoinMiter:
        default:
            ftJoin = GD_FT_STROKER_LINEJOIN_MITER_FIXED;
            break;
    }

    GD_FT_Stroker_Set(stroker, ftWidth, ftCap, ftJoin, ftMiterLimit);

    GD_FT_Stroker_ParseOutline(stroker, outline);

    GD_FT_UInt points, contours;
    GD_FT_Stroker_GetCounts(stroker, &points, &contours);

    GD_FT_Outline *strokeOutline = gd_ft_outline_create((int)points, (int)contours);
    if (!strokeOutline)
    {
        GD_FT_Stroker_Done(stroker);
        gd_ft_outline_destroy(outline);
        return NULL;
    }

    // Use combined export (both borders + caps) - works for both open and closed paths
    GD_FT_Stroker_Export(stroker, strokeOutline);
    GD_FT_Stroker_Done(stroker);

    gdPathPtr strokePath = ft_outline_to_gdpath(strokeOutline);

    gd_ft_outline_destroy(outline);
    gd_ft_outline_destroy(strokeOutline);

    if (!strokePath)
    {
        return NULL;
    }

    return strokePath;
}
