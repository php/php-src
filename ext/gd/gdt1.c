/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jouni Ahto <jah@mork.net>                                    |
   +----------------------------------------------------------------------+
 */

/* 	$Id$	 */

#if 0	/* Moved currently back to gd.c */

#include "php.h"
#include "php_gd.h"

#if HAVE_LIBT1

void php_free_ps_font(int font_id)
{
	T1_DeleteFont(font_id);
}

void php_free_ps_enc(char **enc)
{
	T1_DeleteEncoding(enc);
}

/* {{{ proto int imagepsloadfont(string pathname)
   Load a new font from specified file */
PHP_FUNCTION(imagepsloadfont)
{
	zval **file;
	int f_ind, l_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	f_ind = T1_AddFont(Z_STRVAL_PP(file));

	if (f_ind < 0) {
		switch (f_ind) {
		case -1:
			php_error(E_WARNING, "Couldn't find the font file");
			RETURN_FALSE;
			break;
		case -2:
		case -3:
			php_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	T1_LoadFont(f_ind);
	/*
	l_ind = zend_list_insert(f_ind, T1_GLOBAL(le_ps_font));
	RETURN_LONG(l_ind);
	*/
	zend_list_addref(f_ind);
	RETURN_LONG(f_ind);
}
/* }}} */

/* {{{ The function in t1lib which this function uses seem to be buggy...
proto int imagepscopyfont(int font_index)
Make a copy of a font for purposes like extending or reenconding */
/*
PHP_FUNCTION(imagepscopyfont)
{
	pval *fnt;
	int l_ind, type;
	gd_ps_font *nf_ind, *of_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);

	of_ind = zend_list_find(Z_LVAL_P(fnt), &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_P(fnt));
		RETURN_FALSE;
	}

	nf_ind = emalloc(sizeof(gd_ps_font));
	nf_ind->font_id = T1_CopyFont(of_ind->font_id);

	if (nf_ind->font_id < 0) {
		l_ind = nf_ind->font_id;
		efree(nf_ind);
		switch (l_ind) {
		case -1:
			php_error(E_WARNING, "FontID %d is not loaded in memory", l_ind);
			RETURN_FALSE;
			break;
		case -2:
			php_error(E_WARNING, "Tried to copy a logical font");
			RETURN_FALSE;
			break;
		case -3:
			php_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	nf_ind->extend = 1;
	l_ind = zend_list_insert(nf_ind, GD_GLOBAL(le_ps_font));
	RETURN_LONG(l_ind);
}
*/
/* }}} */

/* {{{ proto bool imagepsfreefont(int font_index)
   Free memory used by a font */
PHP_FUNCTION(imagepsfreefont)
{
	zval **fnt;
	int type;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(fnt);

	zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != T1_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	zend_list_delete(Z_LVAL_PP(fnt));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsencodefont(int font_index, string filename)
   To change a fonts character encoding vector */
PHP_FUNCTION(imagepsencodefont)
{
	zval **fnt, **enc;
	char **enc_vector;
	int type;
	int f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &enc) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(fnt);
	convert_to_string_ex(enc);

	f_ind = zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != T1_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	if ((enc_vector = T1_LoadEncoding(Z_STRVAL_PP(enc))) == NULL) {
		php_error(E_WARNING, "Couldn't load encoding vector from %s", Z_STRVAL_PP(enc));
		RETURN_FALSE;
	}

	T1_DeleteAllSizes(f_ind);
	if (T1_ReencodeFont(f_ind, enc_vector)) {
		T1_DeleteEncoding(enc_vector);
		php_error(E_WARNING, "Couldn't reencode font");
		RETURN_FALSE;
	}
	zend_list_insert(enc_vector, T1_GLOBAL(le_ps_enc));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsextendfont(int font_index, float extend)
   Extend or or condense (if extend < 1) a font */
PHP_FUNCTION(imagepsextendfont)
{
	zval **fnt, **ext;
	int type;
	int f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &ext) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(fnt);
	convert_to_double_ex(ext);

	f_ind = zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != T1_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	if (T1_ExtendFont(f_ind, Z_DVAL_PP(ext)) != 0) RETURN_FALSE;
	/*
	f_ind->extend = Z_DVAL_P(ext);
	*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsslantfont(int font_index, float slant)
   Slant a font */
PHP_FUNCTION(imagepsslantfont)
{
	zval **fnt, **slt;
	int type;
	int f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &slt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(fnt);
	convert_to_double_ex(slt);

	f_ind = zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != T1_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	if (T1_SlantFont(f_ind, Z_DVAL_PP(slt)) != 0) RETURN_FALSE;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imagepstext(int image, string text, int font, int size, int xcoord, int ycoord [, int space, int tightness, float angle, int antialias])
   Rasterize a string over an image */
PHP_FUNCTION(imagepstext)
{
	zval **img, **str, **fnt, **sz, **fg, **bg, **sp, **px, **py, **aas, **wd, **ang;
	int i, j, x, y;
	int space, type;
	int f_ind;
	int h_lines, v_lines, c_ind;
	int rd, gr, bl, fg_rd, fg_gr, fg_bl, bg_rd, bg_gr, bg_bl;
	int aa[16], aa_steps;
	int width, amount_kern, add_width;
	double angle;
	unsigned long aa_greys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	gdImagePtr bg_img;
	GLYPH *str_img;
	T1_OUTLINE *char_path, *str_path;
	T1_TMATRIX *transform = NULL;

	switch(ZEND_NUM_ARGS()) {
	case 8:
		if (zend_get_parameters_ex(8, &img, &str, &fnt, &sz, &fg, &bg, &px, &py) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string_ex(str);
		convert_to_long_ex(fnt);
		convert_to_long_ex(sz);
		convert_to_long_ex(fg);
		convert_to_long_ex(bg);
		convert_to_long_ex(px);
		convert_to_long_ex(py);
		x = Z_LVAL_PP(px);
		y = Z_LVAL_PP(py);
		space = 0;
		aa_steps = 4;
		width = 0;
		angle = 0;
		break;
	case 12:
		if (zend_get_parameters_ex(12, &img, &str, &fnt, &sz, &fg, &bg, &px, &py, &sp, &wd, &ang, &aas) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string_ex(str);
		convert_to_long_ex(fnt);
		convert_to_long_ex(sz);
		convert_to_long_ex(sp);
		convert_to_long_ex(fg);
		convert_to_long_ex(bg);
		convert_to_long_ex(px);
		convert_to_long_ex(py);
		x = Z_LVAL_PP(px);
		y = Z_LVAL_PP(py);
		convert_to_long_ex(sp);
		space = Z_LVAL_PP(sp);
		convert_to_long_ex(aas);
		aa_steps = Z_LVAL_PP(aas);
		convert_to_long_ex(wd);
		width = Z_LVAL_PP(wd);
		convert_to_double_ex(ang);
		angle = Z_DVAL_PP(ang);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	bg_img = zend_list_find(Z_LVAL_PP(img), &type);

	if (!bg_img || type != GD_GLOBAL(le_gd)) {
		php_error(E_WARNING, "%s(): Unable to find image pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	f_ind = zend_list_find(Z_LVAL_P(fnt), &type);

	if (!f_ind || type != GD_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%s(): %d is not a Type 1 font index", get_active_function_name(TSRMLS_C), Z_LVAL_P(fnt));
		RETURN_FALSE;
	}

	fg_rd = gdImageRed(bg_img, Z_LVAL_P(fg));
	fg_gr = gdImageGreen(bg_img, Z_LVAL_P(fg));
	fg_bl = gdImageBlue(bg_img, Z_LVAL_P(fg));
	bg_rd = gdImageRed(bg_img, Z_LVAL_P(bg));
	bg_gr = gdImageGreen(bg_img, Z_LVAL_P(bg));
	bg_bl = gdImageBlue(bg_img, Z_LVAL_P(bg));

	for (i = 0; i < aa_steps; i++) {
		rd = bg_rd+(double)(fg_rd-bg_rd)/aa_steps*(i+1);
		gr = bg_gr+(double)(fg_gr-bg_gr)/aa_steps*(i+1);
		bl = bg_bl+(double)(fg_bl-bg_bl)/aa_steps*(i+1);
		aa[i] = gdImageColorResolve(bg_img, rd, gr, bl);
	}

	T1_AASetBitsPerPixel(8);

	switch (aa_steps) {
	case 4:
		T1_AASetGrayValues(0, 1, 2, 3, 4);
		T1_AASetLevel(T1_AA_LOW);
		break;
	case 16:
		T1_AAHSetGrayValues(aa_greys);
		T1_AASetLevel(T1_AA_HIGH);
		break;
	default:
		php_error(E_WARNING, "%s(): Invalid value %d as number of steps for antialiasing", get_active_function_name(TSRMLS_C), aa_steps);
		RETURN_FALSE;
	}

	if (angle) {
		transform = T1_RotateMatrix(NULL, angle);
	}

	if (width) {
#ifdef HAVE_LIBT1_OUTLINE
	str_path = T1_GetCharOutline(f_ind->font_id, Z_STRVAL_P(str)[0], Z_LVAL_P(sz), transform);

	for (i = 1; i < Z_STRLEN_P(str); i++) {
		amount_kern = (int) T1_GetKerning(f_ind->font_id, Z_STRVAL_P(str)[i-1], Z_STRVAL_P(str)[i]);
		amount_kern += Z_STRVAL_P(str)[i-1] == ' ' ? space : 0;
		add_width = (int) (amount_kern+width)/f_ind->extend;

		char_path = T1_GetMoveOutline(f_ind->font_id, add_width, 0, 0, Z_LVAL_P(sz), transform);
		str_path = T1_ConcatOutlines(str_path, char_path);

		char_path = T1_GetCharOutline(f_ind->font_id, Z_STRVAL_P(str)[i], Z_LVAL_P(sz), transform);
		str_path = T1_ConcatOutlines(str_path, char_path);
	}
	str_img = T1_AAFillOutline(str_path, 0);
#else
	php_error(E_WARNING, "%s(): Setting space between characters in function ImagePSText is supported only with t1lib version 0.9 or above", get_active_function_name(TSRMLS_C));
	RETURN_FALSE;
#endif
	} else {
		str_img = T1_AASetString(f_ind->font_id, Z_STRVAL_P(str),  Z_STRLEN_P(str),
								 space, T1_KERNING, Z_LVAL_P(sz), transform);
	}

	if (T1_errno) RETURN_FALSE;

	h_lines = str_img->metrics.ascent -  str_img->metrics.descent;
	v_lines = str_img->metrics.rightSideBearing - str_img->metrics.leftSideBearing;

	for (i = 0; i < v_lines; i++) {
		for (j = 0; j < h_lines; j++) {
			switch (str_img->bits[j*v_lines+i]) {
			case 0:
				break;
			default:
				c_ind = aa[str_img->bits[j*v_lines+i]-1];
				gdImageSetPixel(bg_img, x+str_img->metrics.leftSideBearing+i, y-str_img->metrics.ascent+j, c_ind);
			}
		}
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	add_next_index_long(return_value, str_img->metrics.leftSideBearing);
	add_next_index_long(return_value, str_img->metrics.descent);
	add_next_index_long(return_value, str_img->metrics.rightSideBearing);
	add_next_index_long(return_value, str_img->metrics.ascent);

}
/* }}} */

/* {{{ proto array imagepsbbox(string text, int font, int size [, int space, int tightness, int angle])
   Return the bounding box needed by a string if rasterized */
PHP_FUNCTION(imagepsbbox)
{
	pval *str, *fnt, *sz, *sp, *wd, *ang;
	int i, space, add_width, char_width, amount_kern, type;
	int cur_x, cur_y, dx, dy;
	int x1, y1, x2, y2, x3, y3, x4, y4;
	gd_ps_font *f_ind;
	int per_char = 0;
	double angle, sin_a, cos_a;
	BBox char_bbox, str_bbox = {0, 0, 0, 0};

	switch(ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters(ht, 3, &str, &fnt, &sz) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		space = 0;
		break;
	case 6:
		if (zend_get_parameters(ht, 6, &str, &fnt, &sz, &sp, &wd, &ang) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		convert_to_long(sp);
		space = Z_LVAL_P(sp);
		convert_to_long(wd);
		add_width = Z_LVAL_P(wd);
		convert_to_double(ang);
		angle = Z_DVAL_P(ang) * M_PI / 180;
		sin_a = sin(angle);
		cos_a = cos(angle);
		per_char =  add_width || angle ? 1 : 0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	f_ind = zend_list_find(Z_LVAL_P(fnt), &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php_error(E_WARNING, "%s(): %d is not a Type 1 font index", get_active_function_name(TSRMLS_C), Z_LVAL_P(fnt));
		RETURN_FALSE;
	}

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#define new_x(a, b) (int) ((a) * cos_a - (b) * sin_a)
#define new_y(a, b) (int) ((a) * sin_a + (b) * cos_a)

	if (per_char) {
		space += T1_GetCharWidth(f_ind->font_id, ' ');
		cur_x = cur_y = 0;

		for (i = 0; i < Z_STRLEN_P(str); i++) {
			if (Z_STRVAL_P(str)[i] == ' ') {
				char_bbox.llx = char_bbox.lly = char_bbox.ury = 0;
				char_bbox.urx = char_width = space;
			} else {
				char_bbox = T1_GetCharBBox(f_ind->font_id, Z_STRVAL_P(str)[i]);
				char_width = T1_GetCharWidth(f_ind->font_id, Z_STRVAL_P(str)[i]);
			}
			amount_kern = i ? T1_GetKerning(f_ind->font_id, Z_STRVAL_P(str)[i-1], Z_STRVAL_P(str)[i]) : 0;

			/* Transfer character bounding box to right place */
			x1 = new_x(char_bbox.llx, char_bbox.lly) + cur_x;
			y1 = new_y(char_bbox.llx, char_bbox.lly) + cur_y;
			x2 = new_x(char_bbox.llx, char_bbox.ury) + cur_x;
			y2 = new_y(char_bbox.llx, char_bbox.ury) + cur_y;
			x3 = new_x(char_bbox.urx, char_bbox.ury) + cur_x;
			y3 = new_y(char_bbox.urx, char_bbox.ury) + cur_y;
			x4 = new_x(char_bbox.urx, char_bbox.lly) + cur_x;
			y4 = new_y(char_bbox.urx, char_bbox.lly) + cur_y;

			/* Find min & max values and compare them with current bounding box */
			str_bbox.llx = min(str_bbox.llx, min(x1, min(x2, min(x3, x4))));
			str_bbox.lly = min(str_bbox.lly, min(y1, min(y2, min(y3, y4))));
			str_bbox.urx = max(str_bbox.urx, max(x1, max(x2, max(x3, x4))));
			str_bbox.ury = max(str_bbox.ury, max(y1, max(y2, max(y3, y4))));

			/* Move to the next base point */
			dx = new_x(char_width + add_width + amount_kern, 0);
			dy = new_y(char_width + add_width + amount_kern, 0);
			cur_x += dx;
			cur_y += dy;
			/*
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", x1, y1, x2, y2, x3, y3, x4, y4, char_bbox.llx, char_bbox.lly, char_bbox.urx, char_bbox.ury, char_width, amount_kern, cur_x, cur_y, dx, dy);
			*/
		}

	} else {
		str_bbox = T1_GetStringBBox(f_ind->font_id, Z_STRVAL_P(str), Z_STRLEN_P(str), space, T1_KERNING);
	}
	if (T1_errno) RETURN_FALSE;
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	/*
	printf("%d %d %d %d\n", str_bbox.llx, str_bbox.lly, str_bbox.urx, str_bbox.ury);
	*/
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.llx)*Z_LVAL_P(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.lly)*Z_LVAL_P(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.urx)*Z_LVAL_P(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.ury)*Z_LVAL_P(sz)/1000));
}
/* }}} */

#endif	/* HAVE_LIBT1 */

#endif	/* 0 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
