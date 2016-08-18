#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "php_config.h"
#endif

#ifdef HAVE_GD_PNG
/* needs to be first */
# include <png.h>
#endif

#ifdef HAVE_GD_JPG
# include <jpeglib.h>
#endif

#include "gd_compat.h"
#include "php.h"

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

int overflow2(int a, int b)
{

	if(a <= 0 || b <= 0) {
		php_error_docref(NULL, E_WARNING, "gd warning: one parameter to a memory allocation multiplication is negative or zero, failing operation gracefully\n");
		return 1;
	}
	if(a > INT_MAX / b) {
		php_error_docref(NULL, E_WARNING, "gd warning: product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully\n");
		return 1;
	}
	return 0;
}

