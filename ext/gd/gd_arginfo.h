/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gd_info, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageloadfont, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetstyle, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, im)
	ZEND_ARG_TYPE_INFO(0, styles, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatetruecolor, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, x_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y_size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageistruecolor, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagetruecolortopalette, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, ditherFlag, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, colorWanted, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagepalettetotruecolor arginfo_imageistruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolormatch, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, im1)
	ZEND_ARG_INFO(0, im2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetthickness, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, thickness, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilledellipse, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, cx, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cy, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, h, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilledarc, 0, 9, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, cx, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cy, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, h, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, e, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagealphablending, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, blend, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesavealpha, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, save, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagelayereffect, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, effect, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorallocatealpha, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alpha, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorresolvealpha arginfo_imagecolorallocatealpha

#define arginfo_imagecolorclosestalpha arginfo_imagecolorallocatealpha

#define arginfo_imagecolorexactalpha arginfo_imagecolorallocatealpha

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopyresampled, 0, 10, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_im, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_h, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_h, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagegrabwindow, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, client_area, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagegrabscreen, 0, 0, GdImage, MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagerotate, 0, 3, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, bgdcolor, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ignoretransparent, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesettile, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, tile)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetbrush, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, brush)
ZEND_END_ARG_INFO()

#define arginfo_imagecreate arginfo_imagecreatetruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagetypes, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecreatefromstring, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, image, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, srcX, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, srcY, IS_LONG, 0)
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
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, foreground, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegif, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_PNG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepng, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, quality, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, filters, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_WEBP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagewebp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, quality, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_JPG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagejpeg, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, quality, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagewbmp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, foreground, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagegd arginfo_imagegif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegd2, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, chunk_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_BMP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagebmp, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO(0, compressed, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_imagedestroy arginfo_imageistruecolor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorallocate, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepalettecopy, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dst, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorat, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorclosest arginfo_imagecolorallocate

#define arginfo_imagecolorclosesthwb arginfo_imagecolorallocate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolordeallocate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecolorresolve arginfo_imagecolorallocate

#define arginfo_imagecolorexact arginfo_imagecolorallocate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorset, 0, 5, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, red, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, green, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, blue, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alpha, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imagecolorsforindex, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegammacorrect, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, inputgamma, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, outputgamma, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetpixel, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageline, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagedashedline arginfo_imageline

#define arginfo_imagerectangle arginfo_imageline

#define arginfo_imagefilledrectangle arginfo_imageline

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagearc, 0, 8, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, cx, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cy, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, h, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, e, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imageellipse arginfo_imagefilledellipse

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilltoborder, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, border, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagefill arginfo_imagesetpixel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolorstotal, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecolortransparent, 0, 1, IS_LONG, 1)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageinterlace, 0, 1, IS_LONG, 1)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, interlace, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagepolygon, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, points, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num_points_or_col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imageopenpolygon arginfo_imagepolygon

#define arginfo_imagefilledpolygon arginfo_imagepolygon

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefontwidth, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagefontheight arginfo_imagefontwidth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagechar, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, c, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecharup arginfo_imagechar

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagestring, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, font, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagestringup arginfo_imagestring

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopy, 0, 8, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_im, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_h, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagecopymerge, 0, 9, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dst_im, GdImage, 0)
	ZEND_ARG_OBJ_INFO(0, src_im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, dst_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, src_h, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pct, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imagecopymergegray arginfo_imagecopymerge

#define arginfo_imagecopyresized arginfo_imagecopyresampled

#define arginfo_imagesx arginfo_imagecolorstotal

#define arginfo_imagesy arginfo_imagecolorstotal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetclip, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, x1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y2, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagegetclip, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageftbbox, 0, 4, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, font_file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extrainfo, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_INFO_EX(arginfo_imagefttext, 0, 0, 8)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, font_file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extrainfo, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_INFO_EX(arginfo_imagettfbbox, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, font_file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GD_FREETYPE)
ZEND_BEGIN_ARG_INFO_EX(arginfo_imagettftext, 0, 0, 8)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, angle, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, font_file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagefilter, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, filtertype, IS_LONG, 0)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
	ZEND_ARG_INFO(0, arg4)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageconvolution, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, matrix3x3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, div, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageflip, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imageantialias, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, on, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecrop, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, rect, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagecropauto, 0, 1, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, threshold, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imagescale, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, new_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new_height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imageaffine, 0, 2, GdImage, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, affine, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, clip, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageaffinematrixget, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageaffinematrixconcat, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, m1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, m2, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imagesetinterpolation, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imageresolution, 0, 1, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, im, GdImage, 0)
	ZEND_ARG_TYPE_INFO(0, res_x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, res_y, IS_LONG, 0)
ZEND_END_ARG_INFO()
