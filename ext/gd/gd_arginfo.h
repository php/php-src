/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0f8a22bff1d123313f37da400500e573baace837 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gd_info, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imageloadfont, 0, 1, GdFont, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetstyle, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatetruecolor, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageistruecolor, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagetruecolortopalette, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dither, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, num_colors, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagepalettetotruecolor arginfo_imageistruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolormatch, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image1, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, image2, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetthickness, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, thickness, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilledellipse, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, center_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, center_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilledarc, 0, 9, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, center_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, center_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start_angle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_angle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagealphablending, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagesavealpha arginfo_imagealphablending

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagelayereffect, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, effect, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorallocatealpha, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alpha, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorresolvealpha, 0, 5, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alpha, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorclosestalpha arginfo_imagecolorresolvealpha

#define arginfo_imagecolorexactalpha arginfo_imagecolorresolvealpha

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopyresampled, 0, 10, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_image, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_height, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagegrabwindow, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, client_area, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagegrabscreen, 0, 0, GdImage, MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagerotate, 0, 3, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, background_color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesettile, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, tile, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetbrush, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, brush, GdImage, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecreate arginfo_imagecreatetruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagetypes, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromstring, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_AVIF)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromavif, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromgif, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_JPG)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromjpeg, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_PNG)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefrompng, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_WEBP)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromwebp, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_imagecreatefromxbm arginfo_imagecreatefromgif

#if defined(HAVE_GD_XPM)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromxpm, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_imagecreatefromwbmp arginfo_imagecreatefromgif

#define arginfo_imagecreatefromgd arginfo_imagecreatefromgif

#define arginfo_imagecreatefromgd2 arginfo_imagecreatefromgif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromgd2part, 0, 5, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_BMP)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefrombmp, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_TGA)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromtga, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagexbm, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, foreground_color, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_AVIF)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageavif, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, quality, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, speed, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegif, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_PNG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepng, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, quality, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filters, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_WEBP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagewebp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, quality, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_JPG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagejpeg, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, quality, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagewbmp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, foreground_color, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegd, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, file, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegd2, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, file, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, chunk_size, IS_LONG, 0, "128")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "IMG_GD2_RAW")
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_BMP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagebmp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, file, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compressed, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#define arginfo_imagedestroy arginfo_imageistruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorallocate, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepalettecopy, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dst, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorat, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorclosest, 0, 4, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorclosesthwb arginfo_imagecolorclosest

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolordeallocate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorresolve arginfo_imagecolorclosest

#define arginfo_imagecolorexact arginfo_imagecolorclosest

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorset, 0, 5, IS_FALSE, 1)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, alpha, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorsforindex, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegammacorrect, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, input_gamma, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, output_gamma, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetpixel, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageline, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagedashedline arginfo_imageline

#define arginfo_imagerectangle arginfo_imageline

#define arginfo_imagefilledrectangle arginfo_imageline

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagearc, 0, 8, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, center_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, center_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start_angle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end_angle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imageellipse arginfo_imagefilledellipse

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilltoborder, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, border_color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagefill arginfo_imagesetpixel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorstotal, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolortransparent, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, color, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageinterlace, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepolygon, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, points, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num_points_or_color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, color, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_imageopenpolygon arginfo_imagepolygon

#define arginfo_imagefilledpolygon arginfo_imagepolygon

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefontwidth, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, font, GdFont, MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_imagefontheight arginfo_imagefontwidth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagechar, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, font, GdFont, MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, char, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecharup arginfo_imagechar

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagestring, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, font, GdFont, MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagestringup arginfo_imagestring

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopy, 0, 8, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_image, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_height, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopymerge, 0, 9, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_image, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pct, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecopymergegray arginfo_imagecopymerge

#define arginfo_imagecopyresized arginfo_imagecopyresampled

#define arginfo_imagesx arginfo_imagecolorstotal

#define arginfo_imagesy arginfo_imagecolorstotal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetclip, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y2, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegetclip, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageftbbox, 0, 4, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, font_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagefttext, 0, 8, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, font_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_FREETYPE)
#define arginfo_imagettfbbox arginfo_imageftbbox
#endif

#if defined(HAVE_GD_FREETYPE)
#define arginfo_imagettftext arginfo_imagefttext
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilter, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, filter, IS_LONG, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageconvolution, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, matrix, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, divisor, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageflip, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imageantialias arginfo_imagealphablending

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecrop, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, rectangle, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecropauto, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "IMG_CROP_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, threshold, IS_DOUBLE, 0, "0.5")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, color, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagescale, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, height, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "IMG_BILINEAR_FIXED")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imageaffine, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, affine, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, clip, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageaffinematrixget, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageaffinematrixconcat, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, matrix1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, matrix2, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagegetinterpolation arginfo_imagecolorstotal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetinterpolation, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, method, IS_LONG, 0, "IMG_BILINEAR_FIXED")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageresolution, 0, 1, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, resolution_x, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, resolution_y, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(gd_info);
ZEND_FUNCTION(imageloadfont);
ZEND_FUNCTION(imagesetstyle);
ZEND_FUNCTION(imagecreatetruecolor);
ZEND_FUNCTION(imageistruecolor);
ZEND_FUNCTION(imagetruecolortopalette);
ZEND_FUNCTION(imagepalettetotruecolor);
ZEND_FUNCTION(imagecolormatch);
ZEND_FUNCTION(imagesetthickness);
ZEND_FUNCTION(imagefilledellipse);
ZEND_FUNCTION(imagefilledarc);
ZEND_FUNCTION(imagealphablending);
ZEND_FUNCTION(imagesavealpha);
ZEND_FUNCTION(imagelayereffect);
ZEND_FUNCTION(imagecolorallocatealpha);
ZEND_FUNCTION(imagecolorresolvealpha);
ZEND_FUNCTION(imagecolorclosestalpha);
ZEND_FUNCTION(imagecolorexactalpha);
ZEND_FUNCTION(imagecopyresampled);
#if defined(PHP_WIN32)
ZEND_FUNCTION(imagegrabwindow);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(imagegrabscreen);
#endif
ZEND_FUNCTION(imagerotate);
ZEND_FUNCTION(imagesettile);
ZEND_FUNCTION(imagesetbrush);
ZEND_FUNCTION(imagecreate);
ZEND_FUNCTION(imagetypes);
ZEND_FUNCTION(imagecreatefromstring);
#if defined(HAVE_GD_AVIF)
ZEND_FUNCTION(imagecreatefromavif);
#endif
ZEND_FUNCTION(imagecreatefromgif);
#if defined(HAVE_GD_JPG)
ZEND_FUNCTION(imagecreatefromjpeg);
#endif
#if defined(HAVE_GD_PNG)
ZEND_FUNCTION(imagecreatefrompng);
#endif
#if defined(HAVE_GD_WEBP)
ZEND_FUNCTION(imagecreatefromwebp);
#endif
ZEND_FUNCTION(imagecreatefromxbm);
#if defined(HAVE_GD_XPM)
ZEND_FUNCTION(imagecreatefromxpm);
#endif
ZEND_FUNCTION(imagecreatefromwbmp);
ZEND_FUNCTION(imagecreatefromgd);
ZEND_FUNCTION(imagecreatefromgd2);
ZEND_FUNCTION(imagecreatefromgd2part);
#if defined(HAVE_GD_BMP)
ZEND_FUNCTION(imagecreatefrombmp);
#endif
#if defined(HAVE_GD_TGA)
ZEND_FUNCTION(imagecreatefromtga);
#endif
ZEND_FUNCTION(imagexbm);
#if defined(HAVE_GD_AVIF)
ZEND_FUNCTION(imageavif);
#endif
ZEND_FUNCTION(imagegif);
#if defined(HAVE_GD_PNG)
ZEND_FUNCTION(imagepng);
#endif
#if defined(HAVE_GD_WEBP)
ZEND_FUNCTION(imagewebp);
#endif
#if defined(HAVE_GD_JPG)
ZEND_FUNCTION(imagejpeg);
#endif
ZEND_FUNCTION(imagewbmp);
ZEND_FUNCTION(imagegd);
ZEND_FUNCTION(imagegd2);
#if defined(HAVE_GD_BMP)
ZEND_FUNCTION(imagebmp);
#endif
ZEND_FUNCTION(imagedestroy);
ZEND_FUNCTION(imagecolorallocate);
ZEND_FUNCTION(imagepalettecopy);
ZEND_FUNCTION(imagecolorat);
ZEND_FUNCTION(imagecolorclosest);
ZEND_FUNCTION(imagecolorclosesthwb);
ZEND_FUNCTION(imagecolordeallocate);
ZEND_FUNCTION(imagecolorresolve);
ZEND_FUNCTION(imagecolorexact);
ZEND_FUNCTION(imagecolorset);
ZEND_FUNCTION(imagecolorsforindex);
ZEND_FUNCTION(imagegammacorrect);
ZEND_FUNCTION(imagesetpixel);
ZEND_FUNCTION(imageline);
ZEND_FUNCTION(imagedashedline);
ZEND_FUNCTION(imagerectangle);
ZEND_FUNCTION(imagefilledrectangle);
ZEND_FUNCTION(imagearc);
ZEND_FUNCTION(imageellipse);
ZEND_FUNCTION(imagefilltoborder);
ZEND_FUNCTION(imagefill);
ZEND_FUNCTION(imagecolorstotal);
ZEND_FUNCTION(imagecolortransparent);
ZEND_FUNCTION(imageinterlace);
ZEND_FUNCTION(imagepolygon);
ZEND_FUNCTION(imageopenpolygon);
ZEND_FUNCTION(imagefilledpolygon);
ZEND_FUNCTION(imagefontwidth);
ZEND_FUNCTION(imagefontheight);
ZEND_FUNCTION(imagechar);
ZEND_FUNCTION(imagecharup);
ZEND_FUNCTION(imagestring);
ZEND_FUNCTION(imagestringup);
ZEND_FUNCTION(imagecopy);
ZEND_FUNCTION(imagecopymerge);
ZEND_FUNCTION(imagecopymergegray);
ZEND_FUNCTION(imagecopyresized);
ZEND_FUNCTION(imagesx);
ZEND_FUNCTION(imagesy);
ZEND_FUNCTION(imagesetclip);
ZEND_FUNCTION(imagegetclip);
#if defined(HAVE_GD_FREETYPE)
ZEND_FUNCTION(imageftbbox);
#endif
#if defined(HAVE_GD_FREETYPE)
ZEND_FUNCTION(imagefttext);
#endif
ZEND_FUNCTION(imagefilter);
ZEND_FUNCTION(imageconvolution);
ZEND_FUNCTION(imageflip);
ZEND_FUNCTION(imageantialias);
ZEND_FUNCTION(imagecrop);
ZEND_FUNCTION(imagecropauto);
ZEND_FUNCTION(imagescale);
ZEND_FUNCTION(imageaffine);
ZEND_FUNCTION(imageaffinematrixget);
ZEND_FUNCTION(imageaffinematrixconcat);
ZEND_FUNCTION(imagegetinterpolation);
ZEND_FUNCTION(imagesetinterpolation);
ZEND_FUNCTION(imageresolution);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("gd_info", zif_gd_info, arginfo_gd_info, 0, NULL)
	ZEND_RAW_FENTRY("imageloadfont", zif_imageloadfont, arginfo_imageloadfont, 0, NULL)
	ZEND_RAW_FENTRY("imagesetstyle", zif_imagesetstyle, arginfo_imagesetstyle, 0, NULL)
	ZEND_RAW_FENTRY("imagecreatetruecolor", zif_imagecreatetruecolor, arginfo_imagecreatetruecolor, 0, NULL)
	ZEND_RAW_FENTRY("imageistruecolor", zif_imageistruecolor, arginfo_imageistruecolor, 0, NULL)
	ZEND_RAW_FENTRY("imagetruecolortopalette", zif_imagetruecolortopalette, arginfo_imagetruecolortopalette, 0, NULL)
	ZEND_RAW_FENTRY("imagepalettetotruecolor", zif_imagepalettetotruecolor, arginfo_imagepalettetotruecolor, 0, NULL)
	ZEND_RAW_FENTRY("imagecolormatch", zif_imagecolormatch, arginfo_imagecolormatch, 0, NULL)
	ZEND_RAW_FENTRY("imagesetthickness", zif_imagesetthickness, arginfo_imagesetthickness, 0, NULL)
	ZEND_RAW_FENTRY("imagefilledellipse", zif_imagefilledellipse, arginfo_imagefilledellipse, 0, NULL)
	ZEND_RAW_FENTRY("imagefilledarc", zif_imagefilledarc, arginfo_imagefilledarc, 0, NULL)
	ZEND_RAW_FENTRY("imagealphablending", zif_imagealphablending, arginfo_imagealphablending, 0, NULL)
	ZEND_RAW_FENTRY("imagesavealpha", zif_imagesavealpha, arginfo_imagesavealpha, 0, NULL)
	ZEND_RAW_FENTRY("imagelayereffect", zif_imagelayereffect, arginfo_imagelayereffect, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorallocatealpha", zif_imagecolorallocatealpha, arginfo_imagecolorallocatealpha, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorresolvealpha", zif_imagecolorresolvealpha, arginfo_imagecolorresolvealpha, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorclosestalpha", zif_imagecolorclosestalpha, arginfo_imagecolorclosestalpha, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorexactalpha", zif_imagecolorexactalpha, arginfo_imagecolorexactalpha, 0, NULL)
	ZEND_RAW_FENTRY("imagecopyresampled", zif_imagecopyresampled, arginfo_imagecopyresampled, 0, NULL)
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("imagegrabwindow", zif_imagegrabwindow, arginfo_imagegrabwindow, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("imagegrabscreen", zif_imagegrabscreen, arginfo_imagegrabscreen, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagerotate", zif_imagerotate, arginfo_imagerotate, 0, NULL)
	ZEND_RAW_FENTRY("imagesettile", zif_imagesettile, arginfo_imagesettile, 0, NULL)
	ZEND_RAW_FENTRY("imagesetbrush", zif_imagesetbrush, arginfo_imagesetbrush, 0, NULL)
	ZEND_RAW_FENTRY("imagecreate", zif_imagecreate, arginfo_imagecreate, 0, NULL)
	ZEND_RAW_FENTRY("imagetypes", zif_imagetypes, arginfo_imagetypes, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("imagecreatefromstring", zif_imagecreatefromstring, arginfo_imagecreatefromstring, 0, NULL)
#if defined(HAVE_GD_AVIF)
	ZEND_RAW_FENTRY("imagecreatefromavif", zif_imagecreatefromavif, arginfo_imagecreatefromavif, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagecreatefromgif", zif_imagecreatefromgif, arginfo_imagecreatefromgif, 0, NULL)
#if defined(HAVE_GD_JPG)
	ZEND_RAW_FENTRY("imagecreatefromjpeg", zif_imagecreatefromjpeg, arginfo_imagecreatefromjpeg, 0, NULL)
#endif
#if defined(HAVE_GD_PNG)
	ZEND_RAW_FENTRY("imagecreatefrompng", zif_imagecreatefrompng, arginfo_imagecreatefrompng, 0, NULL)
#endif
#if defined(HAVE_GD_WEBP)
	ZEND_RAW_FENTRY("imagecreatefromwebp", zif_imagecreatefromwebp, arginfo_imagecreatefromwebp, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagecreatefromxbm", zif_imagecreatefromxbm, arginfo_imagecreatefromxbm, 0, NULL)
#if defined(HAVE_GD_XPM)
	ZEND_RAW_FENTRY("imagecreatefromxpm", zif_imagecreatefromxpm, arginfo_imagecreatefromxpm, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagecreatefromwbmp", zif_imagecreatefromwbmp, arginfo_imagecreatefromwbmp, 0, NULL)
	ZEND_RAW_FENTRY("imagecreatefromgd", zif_imagecreatefromgd, arginfo_imagecreatefromgd, 0, NULL)
	ZEND_RAW_FENTRY("imagecreatefromgd2", zif_imagecreatefromgd2, arginfo_imagecreatefromgd2, 0, NULL)
	ZEND_RAW_FENTRY("imagecreatefromgd2part", zif_imagecreatefromgd2part, arginfo_imagecreatefromgd2part, 0, NULL)
#if defined(HAVE_GD_BMP)
	ZEND_RAW_FENTRY("imagecreatefrombmp", zif_imagecreatefrombmp, arginfo_imagecreatefrombmp, 0, NULL)
#endif
#if defined(HAVE_GD_TGA)
	ZEND_RAW_FENTRY("imagecreatefromtga", zif_imagecreatefromtga, arginfo_imagecreatefromtga, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagexbm", zif_imagexbm, arginfo_imagexbm, 0, NULL)
#if defined(HAVE_GD_AVIF)
	ZEND_RAW_FENTRY("imageavif", zif_imageavif, arginfo_imageavif, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagegif", zif_imagegif, arginfo_imagegif, 0, NULL)
#if defined(HAVE_GD_PNG)
	ZEND_RAW_FENTRY("imagepng", zif_imagepng, arginfo_imagepng, 0, NULL)
#endif
#if defined(HAVE_GD_WEBP)
	ZEND_RAW_FENTRY("imagewebp", zif_imagewebp, arginfo_imagewebp, 0, NULL)
#endif
#if defined(HAVE_GD_JPG)
	ZEND_RAW_FENTRY("imagejpeg", zif_imagejpeg, arginfo_imagejpeg, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagewbmp", zif_imagewbmp, arginfo_imagewbmp, 0, NULL)
	ZEND_RAW_FENTRY("imagegd", zif_imagegd, arginfo_imagegd, 0, NULL)
	ZEND_RAW_FENTRY("imagegd2", zif_imagegd2, arginfo_imagegd2, 0, NULL)
#if defined(HAVE_GD_BMP)
	ZEND_RAW_FENTRY("imagebmp", zif_imagebmp, arginfo_imagebmp, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagedestroy", zif_imagedestroy, arginfo_imagedestroy, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorallocate", zif_imagecolorallocate, arginfo_imagecolorallocate, 0, NULL)
	ZEND_RAW_FENTRY("imagepalettecopy", zif_imagepalettecopy, arginfo_imagepalettecopy, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorat", zif_imagecolorat, arginfo_imagecolorat, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorclosest", zif_imagecolorclosest, arginfo_imagecolorclosest, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorclosesthwb", zif_imagecolorclosesthwb, arginfo_imagecolorclosesthwb, 0, NULL)
	ZEND_RAW_FENTRY("imagecolordeallocate", zif_imagecolordeallocate, arginfo_imagecolordeallocate, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorresolve", zif_imagecolorresolve, arginfo_imagecolorresolve, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorexact", zif_imagecolorexact, arginfo_imagecolorexact, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorset", zif_imagecolorset, arginfo_imagecolorset, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorsforindex", zif_imagecolorsforindex, arginfo_imagecolorsforindex, 0, NULL)
	ZEND_RAW_FENTRY("imagegammacorrect", zif_imagegammacorrect, arginfo_imagegammacorrect, 0, NULL)
	ZEND_RAW_FENTRY("imagesetpixel", zif_imagesetpixel, arginfo_imagesetpixel, 0, NULL)
	ZEND_RAW_FENTRY("imageline", zif_imageline, arginfo_imageline, 0, NULL)
	ZEND_RAW_FENTRY("imagedashedline", zif_imagedashedline, arginfo_imagedashedline, 0, NULL)
	ZEND_RAW_FENTRY("imagerectangle", zif_imagerectangle, arginfo_imagerectangle, 0, NULL)
	ZEND_RAW_FENTRY("imagefilledrectangle", zif_imagefilledrectangle, arginfo_imagefilledrectangle, 0, NULL)
	ZEND_RAW_FENTRY("imagearc", zif_imagearc, arginfo_imagearc, 0, NULL)
	ZEND_RAW_FENTRY("imageellipse", zif_imageellipse, arginfo_imageellipse, 0, NULL)
	ZEND_RAW_FENTRY("imagefilltoborder", zif_imagefilltoborder, arginfo_imagefilltoborder, 0, NULL)
	ZEND_RAW_FENTRY("imagefill", zif_imagefill, arginfo_imagefill, 0, NULL)
	ZEND_RAW_FENTRY("imagecolorstotal", zif_imagecolorstotal, arginfo_imagecolorstotal, 0, NULL)
	ZEND_RAW_FENTRY("imagecolortransparent", zif_imagecolortransparent, arginfo_imagecolortransparent, 0, NULL)
	ZEND_RAW_FENTRY("imageinterlace", zif_imageinterlace, arginfo_imageinterlace, 0, NULL)
	ZEND_RAW_FENTRY("imagepolygon", zif_imagepolygon, arginfo_imagepolygon, 0, NULL)
	ZEND_RAW_FENTRY("imageopenpolygon", zif_imageopenpolygon, arginfo_imageopenpolygon, 0, NULL)
	ZEND_RAW_FENTRY("imagefilledpolygon", zif_imagefilledpolygon, arginfo_imagefilledpolygon, 0, NULL)
	ZEND_RAW_FENTRY("imagefontwidth", zif_imagefontwidth, arginfo_imagefontwidth, 0, NULL)
	ZEND_RAW_FENTRY("imagefontheight", zif_imagefontheight, arginfo_imagefontheight, 0, NULL)
	ZEND_RAW_FENTRY("imagechar", zif_imagechar, arginfo_imagechar, 0, NULL)
	ZEND_RAW_FENTRY("imagecharup", zif_imagecharup, arginfo_imagecharup, 0, NULL)
	ZEND_RAW_FENTRY("imagestring", zif_imagestring, arginfo_imagestring, 0, NULL)
	ZEND_RAW_FENTRY("imagestringup", zif_imagestringup, arginfo_imagestringup, 0, NULL)
	ZEND_RAW_FENTRY("imagecopy", zif_imagecopy, arginfo_imagecopy, 0, NULL)
	ZEND_RAW_FENTRY("imagecopymerge", zif_imagecopymerge, arginfo_imagecopymerge, 0, NULL)
	ZEND_RAW_FENTRY("imagecopymergegray", zif_imagecopymergegray, arginfo_imagecopymergegray, 0, NULL)
	ZEND_RAW_FENTRY("imagecopyresized", zif_imagecopyresized, arginfo_imagecopyresized, 0, NULL)
	ZEND_RAW_FENTRY("imagesx", zif_imagesx, arginfo_imagesx, 0, NULL)
	ZEND_RAW_FENTRY("imagesy", zif_imagesy, arginfo_imagesy, 0, NULL)
	ZEND_RAW_FENTRY("imagesetclip", zif_imagesetclip, arginfo_imagesetclip, 0, NULL)
	ZEND_RAW_FENTRY("imagegetclip", zif_imagegetclip, arginfo_imagegetclip, 0, NULL)
#if defined(HAVE_GD_FREETYPE)
	ZEND_RAW_FENTRY("imageftbbox", zif_imageftbbox, arginfo_imageftbbox, 0, NULL)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_RAW_FENTRY("imagefttext", zif_imagefttext, arginfo_imagefttext, 0, NULL)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_RAW_FENTRY("imagettfbbox", zif_imageftbbox, arginfo_imagettfbbox, 0, NULL)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_RAW_FENTRY("imagettftext", zif_imagefttext, arginfo_imagettftext, 0, NULL)
#endif
	ZEND_RAW_FENTRY("imagefilter", zif_imagefilter, arginfo_imagefilter, 0, NULL)
	ZEND_RAW_FENTRY("imageconvolution", zif_imageconvolution, arginfo_imageconvolution, 0, NULL)
	ZEND_RAW_FENTRY("imageflip", zif_imageflip, arginfo_imageflip, 0, NULL)
	ZEND_RAW_FENTRY("imageantialias", zif_imageantialias, arginfo_imageantialias, 0, NULL)
	ZEND_RAW_FENTRY("imagecrop", zif_imagecrop, arginfo_imagecrop, 0, NULL)
	ZEND_RAW_FENTRY("imagecropauto", zif_imagecropauto, arginfo_imagecropauto, 0, NULL)
	ZEND_RAW_FENTRY("imagescale", zif_imagescale, arginfo_imagescale, 0, NULL)
	ZEND_RAW_FENTRY("imageaffine", zif_imageaffine, arginfo_imageaffine, 0, NULL)
	ZEND_RAW_FENTRY("imageaffinematrixget", zif_imageaffinematrixget, arginfo_imageaffinematrixget, 0, NULL)
	ZEND_RAW_FENTRY("imageaffinematrixconcat", zif_imageaffinematrixconcat, arginfo_imageaffinematrixconcat, 0, NULL)
	ZEND_RAW_FENTRY("imagegetinterpolation", zif_imagegetinterpolation, arginfo_imagegetinterpolation, 0, NULL)
	ZEND_RAW_FENTRY("imagesetinterpolation", zif_imagesetinterpolation, arginfo_imagesetinterpolation, 0, NULL)
	ZEND_RAW_FENTRY("imageresolution", zif_imageresolution, arginfo_imageresolution, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_GdImage_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_GdFont_methods[] = {
	ZEND_FE_END
};

static void register_gd_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("IMG_AVIF", PHP_IMG_AVIF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_GIF", PHP_IMG_GIF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPG", PHP_IMG_JPG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPEG", PHP_IMG_JPEG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_PNG", PHP_IMG_PNG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_WBMP", PHP_IMG_WBMP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_XPM", PHP_IMG_XPM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_WEBP", PHP_IMG_WEBP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BMP", PHP_IMG_BMP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_TGA", PHP_IMG_TGA, CONST_PERSISTENT);
#if defined(gdWebpLossless)
	REGISTER_LONG_CONSTANT("IMG_WEBP_LOSSLESS", gdWebpLossless, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("IMG_COLOR_TILED", gdTiled, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_STYLED", gdStyled, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_BRUSHED", gdBrushed, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_STYLEDBRUSHED", gdStyledBrushed, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_TRANSPARENT", gdTransparent, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_ROUNDED", gdArc, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_PIE", gdPie, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_CHORD", gdChord, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_NOFILL", gdNoFill, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_EDGED", gdEdged, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_GD2_RAW", GD2_FMT_RAW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_GD2_COMPRESSED", GD2_FMT_COMPRESSED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FLIP_HORIZONTAL", PHP_GD_FLIP_HORIZONTAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FLIP_VERTICAL", PHP_GD_FLIP_VERTICAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FLIP_BOTH", PHP_GD_FLIP_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_REPLACE", gdEffectReplace, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_ALPHABLEND", gdEffectAlphaBlend, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_NORMAL", gdEffectNormal, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_OVERLAY", gdEffectOverlay, CONST_PERSISTENT);
#if defined(gdEffectMultiply)
	REGISTER_LONG_CONSTANT("IMG_EFFECT_MULTIPLY", gdEffectMultiply, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("IMG_CROP_DEFAULT", GD_CROP_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CROP_TRANSPARENT", GD_CROP_TRANSPARENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CROP_BLACK", GD_CROP_BLACK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CROP_WHITE", GD_CROP_WHITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CROP_SIDES", GD_CROP_SIDES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CROP_THRESHOLD", GD_CROP_THRESHOLD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BELL", GD_BELL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BESSEL", GD_BESSEL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BILINEAR_FIXED", GD_BILINEAR_FIXED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BICUBIC", GD_BICUBIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BICUBIC_FIXED", GD_BICUBIC_FIXED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BLACKMAN", GD_BLACKMAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BOX", GD_BOX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_BSPLINE", GD_BSPLINE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_CATMULLROM", GD_CATMULLROM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_GAUSSIAN", GD_GAUSSIAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_GENERALIZED_CUBIC", GD_GENERALIZED_CUBIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_HERMITE", GD_HERMITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_HAMMING", GD_HAMMING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_HANNING", GD_HANNING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_MITCHELL", GD_MITCHELL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_POWER", GD_POWER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_QUADRATIC", GD_QUADRATIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_SINC", GD_SINC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_NEAREST_NEIGHBOUR", GD_NEAREST_NEIGHBOUR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_WEIGHTED4", GD_WEIGHTED4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_TRIANGLE", GD_TRIANGLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_AFFINE_TRANSLATE", GD_AFFINE_TRANSLATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_AFFINE_SCALE", GD_AFFINE_SCALE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_AFFINE_ROTATE", GD_AFFINE_ROTATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_AFFINE_SHEAR_HORIZONTAL", GD_AFFINE_SHEAR_HORIZONTAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_AFFINE_SHEAR_VERTICAL", GD_AFFINE_SHEAR_VERTICAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GD_BUNDLED", GD_BUNDLED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_NEGATE", IMAGE_FILTER_NEGATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_GRAYSCALE", IMAGE_FILTER_GRAYSCALE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_BRIGHTNESS", IMAGE_FILTER_BRIGHTNESS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_CONTRAST", IMAGE_FILTER_CONTRAST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_COLORIZE", IMAGE_FILTER_COLORIZE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_EDGEDETECT", IMAGE_FILTER_EDGEDETECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_GAUSSIAN_BLUR", IMAGE_FILTER_GAUSSIAN_BLUR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_SELECTIVE_BLUR", IMAGE_FILTER_SELECTIVE_BLUR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_EMBOSS", IMAGE_FILTER_EMBOSS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_MEAN_REMOVAL", IMAGE_FILTER_MEAN_REMOVAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_SMOOTH", IMAGE_FILTER_SMOOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_PIXELATE", IMAGE_FILTER_PIXELATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_FILTER_SCATTER", IMAGE_FILTER_SCATTER, CONST_PERSISTENT);
#if defined(GD_VERSION_STRING)
	REGISTER_STRING_CONSTANT("GD_VERSION", GD_VERSION_STRING, CONST_PERSISTENT);
#endif
#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION))
	REGISTER_LONG_CONSTANT("GD_MAJOR_VERSION", GD_MAJOR_VERSION, CONST_PERSISTENT);
#endif
#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION))
	REGISTER_LONG_CONSTANT("GD_MINOR_VERSION", GD_MINOR_VERSION, CONST_PERSISTENT);
#endif
#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION))
	REGISTER_LONG_CONSTANT("GD_RELEASE_VERSION", GD_RELEASE_VERSION, CONST_PERSISTENT);
#endif
#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION))
	REGISTER_STRING_CONSTANT("GD_EXTRA_VERSION", GD_EXTRA_VERSION, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_NO_FILTER", 0x0, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_FILTER_NONE", 0x8, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_FILTER_SUB", 0x10, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_FILTER_UP", 0x20, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_FILTER_AVG", 0x40, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_FILTER_PAETH", 0x80, CONST_PERSISTENT);
#endif
#if defined(HAVE_GD_PNG)
	REGISTER_LONG_CONSTANT("PNG_ALL_FILTERS", 0x8 | 0x10 | 0x20 | 0x40 | 0x80, CONST_PERSISTENT);
#endif
}

static zend_class_entry *register_class_GdImage(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GdImage", class_GdImage_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_GdFont(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GdFont", class_GdFont_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
