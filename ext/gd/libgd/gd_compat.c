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

/* Not exported by libgd, copied from gdhelpers.h */
int overflow2(int a, int b)
{
	if(a <= 0 || b <= 0) {
		return 1;
	}
	if(a > INT_MAX / b) {
		return 1;
	}
	return 0;
}

/* Not exported by libgd, copied from wbmp.c */
int
getmbi (int (*getin) (void *in), void *in)
{
  int i, mbi = 0;

  do
    {
      i = getin (in);
      if (i < 0)
	return (-1);
      mbi = (mbi << 7) | (i & 0x7f);
    }
  while (i & 0x80);

  return (mbi);
}

/* Not exported by libgd, copied from wbmp.c */
int
skipheader (int (*getin) (void *in), void *in)
{
  int i;

  do
    {
      i = getin (in);
      if (i < 0)
	return (-1);
    }
  while (i & 0x80);

  return (0);
}

