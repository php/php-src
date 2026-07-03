#include <math.h>
#include <string.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd_vector2d_private.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_array.h"
#include "gd_path.h"
#include "gd_path_dash.h"
#include "gd_path_matrix.h"

gdPathDashPtr gdPathDashCreate(const double *data, int size, double offset)
{
    if (data == NULL || size == 0)
        return NULL;

    gdPathDashPtr dash = gdMalloc(sizeof(gdPathDash));
    if (!dash)
    {
        return NULL;
    }
    dash->offset = offset;
    dash->data = gdMalloc((size_t)size * sizeof(double));
    if (!dash->data)
    {
        gdFree(dash);
        return NULL;
    }
    dash->size = size;
    memcpy(dash->data, data, (size_t)size * sizeof(double));
    return dash;
}

void gdPathDashDestroy(gdPathDashPtr dash)
{
    if (dash == NULL)
        return;
    gdFree(dash->data);
    gdFree(dash);
}

gdPathDashPtr gdPathDashClone(const gdPathDashPtr dash)
{
    if (dash == NULL)
        return NULL;

    return gdPathDashCreate(dash->data, dash->size, dash->offset);
}

gdPathPtr gdPathApplyDash(const gdPathDashPtr dash, const gdPathPtr path)
{
    gdPathPtr flat = gdPathDuplicateFlattened(path);
    gdPathPtr result = gdPathCreate();

    gdArrayReallocBy(&result->elements, gdArrayNumElements(&flat->elements));
    gdArrayReallocBy(&result->points, gdArrayNumElements(&flat->points));

    int toggle = 1;
    int offset = 0;
    double phase = dash->offset;
    while(phase >= dash->data[offset])
    {
        toggle = !toggle;
        phase -= dash->data[offset];
        if(++offset == dash->size) offset = 0;
    }

    gdPathOpsPtr elements = (gdPathOpsPtr)gdArrayGetData(&flat->elements);
    gdPathOpsPtr end = elements + gdArrayNumElements(&flat->elements);
    gdPointFPtr points = (gdPointFPtr)gdArrayGetData(&flat->points);

    while(elements < end)
    {
        int itoggle = toggle;
        int ioffset = offset;
        double iphase = phase;

        double x0 = points->x;
        double y0 = points->y;

        if(itoggle) gdPathMoveTo(result, x0, y0);
        ++elements;
        ++points;
        while(elements < end
         && *elements==gdPathOpsLineTo)
        {
            double dx = points->x - x0;
            double dy = points->y - y0;
            double dist0 = sqrt(dx*dx + dy*dy);
            double dist1 = 0;
            while(dist0 - dist1 > dash->data[ioffset] - iphase)
            {
                dist1 += dash->data[ioffset] - iphase;
                double a = dist1 / dist0;
                double x = x0 + a * dx;
                double y = y0 + a * dy;

                if(itoggle) gdPathLineTo(result, x, y);
                else {
                     gdPathMoveTo(result, x, y);
                }

                itoggle = !itoggle;
                iphase = 0;
                if(++ioffset==dash->size) ioffset = 0;
            }

            iphase += dist0 - dist1;

            x0 = points->x;
            y0 = points->y;

            if(itoggle) gdPathLineTo(result, x0, y0);

            ++elements;
            ++points;
        }
    }

    gdPathDestroy(flat);
    return result;
}
