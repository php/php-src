#include "php_config.h"

#ifdef HAVE_GD_PNG
/* needs to be first */
# include <png.h>
#endif

#ifdef HAVE_GD_JPG
# include <jpeglib.h>
#endif

#include "gd_compat.c"

#ifdef HAVE_GD_JPG
int gdJpegGetVersionInt()
{
	return JPEG_LIB_VERSION;
}

const char * gdJpegGetVersionString()
{
	switch(JPEG_LIB_VERSION) {
		case 62:
			return "6b";
			break;

		case 70:
			return "7";
			break;

		case 80:
			return "8";
			break;

		default:
			return "unknown";
	}
}
#endif

#ifdef HAVE_GD_PNG
const char * gdPngGetVersionString()
{
	return PNG_LIBPNG_VER_STRING;
}
#endif

