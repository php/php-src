#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_intern.h"
#include "gd_vector2d_private.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_surface.h"
#include "gd_array.h"

static int checkOverflowAndType(int width, int height, unsigned type) {
    int bytes_per_pixel;
    if (type == GD_SURFACE_NONE || type >= GD_SURFACE_COUNT || width <= 0 || height <= 0) {
		gd_error("gdSurface: invalid dimensions or surface type\n");
        return 1;
    }
	bytes_per_pixel = type == GD_SURFACE_A8 ? 1 : 4;
	if (overflow2(width, bytes_per_pixel) ||
		overflow2(width * bytes_per_pixel, height)) {
		return 1;
	}
    return 0;
}

GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceCreate(int width, int height, unsigned int type)
{
    gdSurfacePtr surface;

    if (type >= GD_SURFACE_COUNT) {
        return NULL;
    }
    if (checkOverflowAndType(width, height, type)) {
        return NULL;
    }
    surface = gdMalloc(sizeof(gdSurface));
	if (!surface) {
		return NULL;
	}

    const int bytes_per_pixel = type == GD_SURFACE_A8 ? 1 : 4;
    const size_t size = (size_t)width * (size_t)height * (size_t)bytes_per_pixel;
    surface->data = gdCalloc(1, size);
	if (!surface->data) {
        gdFree(surface);
		return NULL;
	}
    surface->type = type;
    surface->ref = 1;
    surface->gdOwned = 1;
    surface->width = width;
    surface->height = height;
    surface->stride = width * bytes_per_pixel;
    return surface;
}

GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceCreateForData(unsigned char* data, int width, int height, int stride, unsigned int type)
{
    gdSurfacePtr surface;

    if (!data) {
        return NULL;
    }
    if (checkOverflowAndType(width, height, type)) {
        return NULL;
    }
    if (stride < width * (type == GD_SURFACE_A8 ? 1 : 4)) {
        return NULL;
    }

    surface = gdMalloc(sizeof(gdSurface));
	if (!surface) {
		return NULL;
	}
    surface->ref = 1;
    surface->gdOwned = 0;
    surface->data = data;
    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    surface->type = type;
    return surface;
}

GD_VECTOR2D_INTERNAL gdSurfacePtr gdSurfaceAddRef(gdSurfacePtr surface)
{
    if(surface==NULL) {
        return NULL;
    }
    surface->ref++;
    return surface;
}

GD_VECTOR2D_INTERNAL unsigned char *gdSurfaceGetData(const gdSurfacePtr surface)
{
    if(surface==NULL) {
        return NULL;
    }
    return surface->data;
}

GD_VECTOR2D_INTERNAL gdSurfaceType gdSurfaceGetType(const gdSurfacePtr surface)
{
    if(surface==NULL) {
        return GD_SURFACE_NONE;
    }
    return surface->type;
}

GD_VECTOR2D_INTERNAL int gdSurfaceGetWidth(const gdSurfacePtr surface)
{
    return surface->width;
}

GD_VECTOR2D_INTERNAL int gdSurfaceGetHeight(const gdSurfacePtr surface)
{
    return surface->height;
}

GD_VECTOR2D_INTERNAL int gdSurfaceGetStride(const gdSurfacePtr surface)
{
    return surface->stride;
}

GD_VECTOR2D_INTERNAL void gdSurfaceDestroy (gdSurfacePtr surface)
{
    if (!surface) {
        return;
    }
    if (--surface->ref == 0) {
        if (surface->gdOwned) {
            gdFree(surface->data);
        }
        gdFree(surface);
    }

}
