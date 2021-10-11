/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c45bd3923e6c65a869b39822f574bde363a3499f */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gd_info, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageloadfont, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ignore_transparent, _IS_BOOL, 0, "false")
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
	ZEND_ARG_TYPE_INFO(0, chunk_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorset, 0, 5, _IS_BOOL, 1)
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
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagefontheight arginfo_imagefontwidth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagechar, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, char, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecharup arginfo_imagechar

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagestring, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
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
	ZEND_FE(gd_info, arginfo_gd_info)
	ZEND_FE(imageloadfont, arginfo_imageloadfont)
	ZEND_FE(imagesetstyle, arginfo_imagesetstyle)
	ZEND_FE(imagecreatetruecolor, arginfo_imagecreatetruecolor)
	ZEND_FE(imageistruecolor, arginfo_imageistruecolor)
	ZEND_FE(imagetruecolortopalette, arginfo_imagetruecolortopalette)
	ZEND_FE(imagepalettetotruecolor, arginfo_imagepalettetotruecolor)
	ZEND_FE(imagecolormatch, arginfo_imagecolormatch)
	ZEND_FE(imagesetthickness, arginfo_imagesetthickness)
	ZEND_FE(imagefilledellipse, arginfo_imagefilledellipse)
	ZEND_FE(imagefilledarc, arginfo_imagefilledarc)
	ZEND_FE(imagealphablending, arginfo_imagealphablending)
	ZEND_FE(imagesavealpha, arginfo_imagesavealpha)
	ZEND_FE(imagelayereffect, arginfo_imagelayereffect)
	ZEND_FE(imagecolorallocatealpha, arginfo_imagecolorallocatealpha)
	ZEND_FE(imagecolorresolvealpha, arginfo_imagecolorresolvealpha)
	ZEND_FE(imagecolorclosestalpha, arginfo_imagecolorclosestalpha)
	ZEND_FE(imagecolorexactalpha, arginfo_imagecolorexactalpha)
	ZEND_FE(imagecopyresampled, arginfo_imagecopyresampled)
#if defined(PHP_WIN32)
	ZEND_FE(imagegrabwindow, arginfo_imagegrabwindow)
#endif
#if defined(PHP_WIN32)
	ZEND_FE(imagegrabscreen, arginfo_imagegrabscreen)
#endif
	ZEND_FE(imagerotate, arginfo_imagerotate)
	ZEND_FE(imagesettile, arginfo_imagesettile)
	ZEND_FE(imagesetbrush, arginfo_imagesetbrush)
	ZEND_FE(imagecreate, arginfo_imagecreate)
	ZEND_FE(imagetypes, arginfo_imagetypes)
	ZEND_FE(imagecreatefromstring, arginfo_imagecreatefromstring)
	ZEND_FE(imagecreatefromgif, arginfo_imagecreatefromgif)
#if defined(HAVE_GD_JPG)
	ZEND_FE(imagecreatefromjpeg, arginfo_imagecreatefromjpeg)
#endif
#if defined(HAVE_GD_PNG)
	ZEND_FE(imagecreatefrompng, arginfo_imagecreatefrompng)
#endif
#if defined(HAVE_GD_WEBP)
	ZEND_FE(imagecreatefromwebp, arginfo_imagecreatefromwebp)
#endif
	ZEND_FE(imagecreatefromxbm, arginfo_imagecreatefromxbm)
#if defined(HAVE_GD_XPM)
	ZEND_FE(imagecreatefromxpm, arginfo_imagecreatefromxpm)
#endif
	ZEND_FE(imagecreatefromwbmp, arginfo_imagecreatefromwbmp)
	ZEND_FE(imagecreatefromgd, arginfo_imagecreatefromgd)
	ZEND_FE(imagecreatefromgd2, arginfo_imagecreatefromgd2)
	ZEND_FE(imagecreatefromgd2part, arginfo_imagecreatefromgd2part)
#if defined(HAVE_GD_BMP)
	ZEND_FE(imagecreatefrombmp, arginfo_imagecreatefrombmp)
#endif
#if defined(HAVE_GD_TGA)
	ZEND_FE(imagecreatefromtga, arginfo_imagecreatefromtga)
#endif
	ZEND_FE(imagexbm, arginfo_imagexbm)
	ZEND_FE(imagegif, arginfo_imagegif)
#if defined(HAVE_GD_PNG)
	ZEND_FE(imagepng, arginfo_imagepng)
#endif
#if defined(HAVE_GD_WEBP)
	ZEND_FE(imagewebp, arginfo_imagewebp)
#endif
#if defined(HAVE_GD_JPG)
	ZEND_FE(imagejpeg, arginfo_imagejpeg)
#endif
	ZEND_FE(imagewbmp, arginfo_imagewbmp)
	ZEND_FE(imagegd, arginfo_imagegd)
	ZEND_FE(imagegd2, arginfo_imagegd2)
#if defined(HAVE_GD_BMP)
	ZEND_FE(imagebmp, arginfo_imagebmp)
#endif
	ZEND_FE(imagedestroy, arginfo_imagedestroy)
	ZEND_FE(imagecolorallocate, arginfo_imagecolorallocate)
	ZEND_FE(imagepalettecopy, arginfo_imagepalettecopy)
	ZEND_FE(imagecolorat, arginfo_imagecolorat)
	ZEND_FE(imagecolorclosest, arginfo_imagecolorclosest)
	ZEND_FE(imagecolorclosesthwb, arginfo_imagecolorclosesthwb)
	ZEND_FE(imagecolordeallocate, arginfo_imagecolordeallocate)
	ZEND_FE(imagecolorresolve, arginfo_imagecolorresolve)
	ZEND_FE(imagecolorexact, arginfo_imagecolorexact)
	ZEND_FE(imagecolorset, arginfo_imagecolorset)
	ZEND_FE(imagecolorsforindex, arginfo_imagecolorsforindex)
	ZEND_FE(imagegammacorrect, arginfo_imagegammacorrect)
	ZEND_FE(imagesetpixel, arginfo_imagesetpixel)
	ZEND_FE(imageline, arginfo_imageline)
	ZEND_FE(imagedashedline, arginfo_imagedashedline)
	ZEND_FE(imagerectangle, arginfo_imagerectangle)
	ZEND_FE(imagefilledrectangle, arginfo_imagefilledrectangle)
	ZEND_FE(imagearc, arginfo_imagearc)
	ZEND_FE(imageellipse, arginfo_imageellipse)
	ZEND_FE(imagefilltoborder, arginfo_imagefilltoborder)
	ZEND_FE(imagefill, arginfo_imagefill)
	ZEND_FE(imagecolorstotal, arginfo_imagecolorstotal)
	ZEND_FE(imagecolortransparent, arginfo_imagecolortransparent)
	ZEND_FE(imageinterlace, arginfo_imageinterlace)
	ZEND_FE(imagepolygon, arginfo_imagepolygon)
	ZEND_FE(imageopenpolygon, arginfo_imageopenpolygon)
	ZEND_FE(imagefilledpolygon, arginfo_imagefilledpolygon)
	ZEND_FE(imagefontwidth, arginfo_imagefontwidth)
	ZEND_FE(imagefontheight, arginfo_imagefontheight)
	ZEND_FE(imagechar, arginfo_imagechar)
	ZEND_FE(imagecharup, arginfo_imagecharup)
	ZEND_FE(imagestring, arginfo_imagestring)
	ZEND_FE(imagestringup, arginfo_imagestringup)
	ZEND_FE(imagecopy, arginfo_imagecopy)
	ZEND_FE(imagecopymerge, arginfo_imagecopymerge)
	ZEND_FE(imagecopymergegray, arginfo_imagecopymergegray)
	ZEND_FE(imagecopyresized, arginfo_imagecopyresized)
	ZEND_FE(imagesx, arginfo_imagesx)
	ZEND_FE(imagesy, arginfo_imagesy)
	ZEND_FE(imagesetclip, arginfo_imagesetclip)
	ZEND_FE(imagegetclip, arginfo_imagegetclip)
#if defined(HAVE_GD_FREETYPE)
	ZEND_FE(imageftbbox, arginfo_imageftbbox)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_FE(imagefttext, arginfo_imagefttext)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_FALIAS(imagettfbbox, imageftbbox, arginfo_imagettfbbox)
#endif
#if defined(HAVE_GD_FREETYPE)
	ZEND_FALIAS(imagettftext, imagefttext, arginfo_imagettftext)
#endif
	ZEND_FE(imagefilter, arginfo_imagefilter)
	ZEND_FE(imageconvolution, arginfo_imageconvolution)
	ZEND_FE(imageflip, arginfo_imageflip)
	ZEND_FE(imageantialias, arginfo_imageantialias)
	ZEND_FE(imagecrop, arginfo_imagecrop)
	ZEND_FE(imagecropauto, arginfo_imagecropauto)
	ZEND_FE(imagescale, arginfo_imagescale)
	ZEND_FE(imageaffine, arginfo_imageaffine)
	ZEND_FE(imageaffinematrixget, arginfo_imageaffinematrixget)
	ZEND_FE(imageaffinematrixconcat, arginfo_imageaffinematrixconcat)
	ZEND_FE(imagegetinterpolation, arginfo_imagegetinterpolation)
	ZEND_FE(imagesetinterpolation, arginfo_imagesetinterpolation)
	ZEND_FE(imageresolution, arginfo_imageresolution)
	ZEND_FE_END
};


static const zend_function_entry class_GdImage_methods[] = {
	ZEND_FE_END
};
