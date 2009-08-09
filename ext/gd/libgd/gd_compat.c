#include "php_config.h" 
#ifdef HAVE_GD_PNG
/* needs to be first */
# include <png.h>
#endif

#ifdef HAVE_GD_JPG
# include <jpeglib.h>
#endif

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

