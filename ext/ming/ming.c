/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Dave Hayden <dave@opaque.net>                               |
  |          Frank M. Kromann <fmk@php.net>                              |
  |          Stuart R. Anderson <anderson@netsweng.com>                  |
  |          Klaus Rechert <klaus@rechert.de>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "php_streams.h"

#if HAVE_MING

#include "php_ming.h"
#include <stdio.h>
#include <math.h>
#include <ming.h>

#ifndef HAVE_DESTROY_SWF_BLOCK
void destroySWFBlock(SWFBlock block);
#endif

static const zend_function_entry ming_functions[] = {
	PHP_FALIAS(ming_setcubicthreshold,  ming_setCubicThreshold,  NULL)
	PHP_FALIAS(ming_setscale,           ming_setScale,           NULL)
	PHP_FALIAS(ming_useswfversion,      ming_useSWFVersion,      NULL)
	PHP_FALIAS(ming_keypress,           ming_keypress,           NULL)
#ifdef HAVE_NEW_MING
	PHP_FALIAS(ming_useconstants,		ming_useConstants,       NULL)
	PHP_FALIAS(ming_setswfcompression,	ming_setSWFCompression,  NULL)
#endif
	{ NULL, NULL, NULL }
};

static SWFMovie getMovie(zval *id TSRMLS_DC);
static SWFFill getFill(zval *id TSRMLS_DC);
static SWFGradient getGradient(zval *id TSRMLS_DC);
static SWFBitmap getBitmap(zval *id TSRMLS_DC);
static SWFShape getShape(zval *id TSRMLS_DC);
static SWFFont getFont(zval *id TSRMLS_DC);
static SWFText getText(zval *id TSRMLS_DC);
static SWFTextField getTextField(zval *id TSRMLS_DC);
static SWFDisplayItem getDisplayItem(zval *id TSRMLS_DC);
static SWFButton getButton(zval *id TSRMLS_DC);
static SWFAction getAction(zval *id TSRMLS_DC);
static SWFInitAction getInitAction(zval *id TSRMLS_DC);
static SWFMorph getMorph(zval *id TSRMLS_DC);
static SWFMovieClip getSprite(zval *id TSRMLS_DC);
static SWFSound getSound(zval *id TSRMLS_DC);
static SWFInput getInput(zval *id TSRMLS_DC);
#ifdef HAVE_NEW_MING
static SWFFontCharacter getFontCharacter(zval *id TSRMLS_DC);
static SWFSoundInstance getSoundInstance(zval *id TSRMLS_DC);
static SWFVideoStream getVideoStream(zval *id TSRMLS_DC);
static SWFButtonRecord getButtonRecord(zval *id TSRMLS_DC);
static SWFPrebuiltClip getPrebuiltClip(zval *id TSRMLS_DC);
static SWFCharacter getCharacterClass(zval *id TSRMLS_DC);
static SWFBinaryData getBinaryData(zval *id TSRMLS_DC);
static SWFBlur getBlur(zval *id TSRMLS_DC);
static SWFShadow getShadow(zval *id TSRMLS_DC);
static SWFFilterMatrix getFilterMatrix(zval *id TSRMLS_DC);
static SWFFilter getFilter(zval *id TSRMLS_DC);
static SWFCXform getCXform(zval *id TSRMLS_DC);
static SWFMatrix getMatrix(zval *id TSRMLS_DC);
#endif

#define PHP_MING_FILE_CHK(file) \
	if ((PG(safe_mode) && !php_checkuid((file), NULL, CHECKUID_CHECK_FILE_AND_DIR)) || php_check_open_basedir((file) TSRMLS_CC)) {	\
		RETURN_FALSE;	\
	}	\

/* {{{ proto void ming_setcubicthreshold (int threshold)
   Set cubic threshold (?) */
PHP_FUNCTION(ming_setCubicThreshold)
{
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		return;
	}
	Ming_setCubicThreshold(num);
}
/* }}} */

/* {{{ proto void ming_setscale(float scale)
   Set scale (?) */
PHP_FUNCTION(ming_setScale)
{
	double num;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	Ming_setScale((float)num);
}
/* }}} */

/* {{{ proto void ming_useswfversion(int version)
   Use SWF version (?) */ 
PHP_FUNCTION(ming_useSWFVersion)
{
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		return;
	}
	Ming_useSWFVersion(num);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void ming_useconstants(int use)
	Use constant pool (?) */ 
PHP_FUNCTION(ming_useConstants)
{
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		return;
	}
	Ming_useConstants(num);
}
/* }}} */

/* {{{ set output compression */
PHP_FUNCTION(ming_setSWFCompression)
{  
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		return;
	}
	Ming_setSWFCompression(num);
}
/* }}} */

#endif

static int le_swfmoviep;
static int le_swfshapep;
static int le_swffillp;
static int le_swfgradientp;
static int le_swfbitmapp;
static int le_swffontp;
static int le_swftextp;
static int le_swftextfieldp;
static int le_swfdisplayitemp;
static int le_swfbuttonp;
static int le_swfactionp;
static int le_swfmorphp;
static int le_swfmovieclipp;
static int le_swfspritep;
static int le_swfinputp;
static int le_swfsoundp;
#ifdef HAVE_NEW_MING
static int le_swffontcharp;
static int le_swfsoundinstancep;
static int le_swfvideostreamp;
static int le_swfbuttonrecordp;
static int le_swfbinarydatap;
static int le_swfinitactionp;
static int le_swfprebuiltclipp;
static int le_swfsoundstreamp;
static int le_swffilterp;
static int le_swfblurp;
static int le_swfshadowp;
static int le_swffiltermatrixp;
static int le_swfcxformp;
static int le_swfmatrixp;
static int le_swfbrowserfontp;
static int le_swffontcollectionp;
#endif
static int le_swfcharacterp;

static zend_class_entry *movie_class_entry_ptr;
static zend_class_entry *shape_class_entry_ptr;
static zend_class_entry *fill_class_entry_ptr;
static zend_class_entry *gradient_class_entry_ptr;
static zend_class_entry *bitmap_class_entry_ptr;
static zend_class_entry *font_class_entry_ptr;
static zend_class_entry *text_class_entry_ptr;
static zend_class_entry *textfield_class_entry_ptr;
static zend_class_entry *displayitem_class_entry_ptr;
static zend_class_entry *button_class_entry_ptr;
static zend_class_entry *action_class_entry_ptr;
static zend_class_entry *morph_class_entry_ptr;
static zend_class_entry *movieclip_class_entry_ptr;
static zend_class_entry *sprite_class_entry_ptr;
static zend_class_entry *sound_class_entry_ptr;
static zend_class_entry *character_class_entry_ptr;
static zend_class_entry *input_class_entry_ptr;
#ifdef HAVE_NEW_MING
static zend_class_entry *fontchar_class_entry_ptr;
static zend_class_entry *soundinstance_class_entry_ptr;
static zend_class_entry *videostream_class_entry_ptr;
static zend_class_entry *buttonrecord_class_entry_ptr;
static zend_class_entry *binarydata_class_entry_ptr;
static zend_class_entry *initaction_class_entry_ptr;
static zend_class_entry *prebuiltclip_class_entry_ptr;
static zend_class_entry *soundstream_class_entry_ptr;
static zend_class_entry *filter_class_entry_ptr;
static zend_class_entry *blur_class_entry_ptr;
static zend_class_entry *shadow_class_entry_ptr;
static zend_class_entry *filtermatrix_class_entry_ptr;
static zend_class_entry *cxform_class_entry_ptr;
static zend_class_entry *matrix_class_entry_ptr;
static zend_class_entry *browserfont_class_entry_ptr;
static zend_class_entry *fontcollection_class_entry_ptr;
#endif
static zend_class_entry *character_class_entry_ptr;

/* {{{ internal function SWFgetProperty
*/
static void *SWFgetProperty(zval *id, char *name, int namelen, int proptype TSRMLS_DC)
{
	zval **tmp;
	int id_to_find;
	void *property;
	int type;

	if (id) {
		if (zend_hash_find(Z_OBJPROP_P(id), name, namelen+1, (void **)&tmp) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find property %s", name);
			return NULL;
		}
		id_to_find = Z_LVAL_PP(tmp);
	} else {
		return NULL;
	}

	property = zend_list_find(id_to_find, &type);

	if (!property || type != proptype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find identifier (%d)", id_to_find);
		return NULL;
	}

	return property;
}
/* }}} */

/* {{{ SWFCharacter 
*/

/* {{{ internal function SWFCharacter getCharacter(zval *id)
   Returns the SWFCharacter contained in zval *id */
static SWFCharacter getCharacter(zval *id TSRMLS_DC)
{
	if (Z_OBJCE_P(id) == shape_class_entry_ptr)
		return (SWFCharacter)getShape(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == font_class_entry_ptr)
		return (SWFCharacter)getFont(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == text_class_entry_ptr)
		return (SWFCharacter)getText(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == textfield_class_entry_ptr)
		return (SWFCharacter)getTextField(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == button_class_entry_ptr)
		return (SWFCharacter)getButton(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == morph_class_entry_ptr)
		return (SWFCharacter)getMorph(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == movieclip_class_entry_ptr)
		return (SWFCharacter)getSprite(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == sprite_class_entry_ptr)
		return (SWFCharacter)getSprite(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == bitmap_class_entry_ptr)
		return (SWFCharacter)getBitmap(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == sound_class_entry_ptr)
		return (SWFCharacter)getSound(id TSRMLS_CC);
#ifdef HAVE_NEW_MING
	else if(Z_OBJCE_P(id) == fontchar_class_entry_ptr)
		return (SWFCharacter)getFontCharacter(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == soundinstance_class_entry_ptr)
		return (SWFCharacter)getSoundInstance(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == videostream_class_entry_ptr)
		return (SWFCharacter)getVideoStream(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == buttonrecord_class_entry_ptr)
		return (SWFCharacter)getButtonRecord(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == prebuiltclip_class_entry_ptr)
		return (SWFCharacter)getPrebuiltClip(id TSRMLS_CC);
#endif
	else
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFCharacter");
		return NULL;
}
/* }}} */

static SWFCharacter getCharacterClass(zval *id TSRMLS_DC)
{
	void *character = SWFgetProperty(id, "character", strlen("character"), le_swfcharacterp TSRMLS_CC);

	if (!character) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFCharacter");
	}
	return (SWFCharacter)character;
}

/* {{{ proto swfcharacter::getWidth() */
PHP_METHOD(swfcharacter, getWidth)
{
        RETURN_DOUBLE(SWFCharacter_getWidth(getCharacter(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto swfcharacter::getHeight() */
PHP_METHOD(swfcharacter, getHeight)
{
        RETURN_DOUBLE(SWFCharacter_getHeight(getCharacter(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swfcharacter_functions[] = {
        PHP_ME(swfcharacter, getWidth,          NULL, 0)
        PHP_ME(swfcharacter, getHeight,         NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

static void destroy_SWFInput_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFInput((SWFInput)resource->ptr);
}

/* {{{ getInput_fromFileResource - utility func for making an SWFInput from an fopened resource */
static SWFInput getInput_fromFileResource(zval *zfile TSRMLS_DC)
{
	FILE *file;
	php_stream *stream;
	SWFInput input;

	php_stream_from_zval_no_verify(stream, &zfile);

	if (stream == NULL) {
		return NULL;
	}

	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void *) &file, REPORT_ERRORS) != SUCCESS) {
		return NULL;
	}

	input = newSWFInput_file(file);
	zend_list_addref(Z_LVAL_P(zfile));
	zend_list_addref(zend_list_insert(input, le_swfinputp));
	return input;
}
/* }}} */

/* {{{ internal function getInput */
static SWFInput getInput(zval *id TSRMLS_DC)
{
	void *in = SWFgetProperty(id, "input", strlen("input"), le_swfinputp TSRMLS_CC);

	if (!in) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFInput");
	}
	return (SWFInput)in;
}
/* }}} */


/* {{{ proto void swfinput::__construct(string data) */
PHP_METHOD(swfinput, __construct)
{
	SWFInput input;
	char *data;
	long data_len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		return;
	}

	input = newSWFInput_bufferCopy((unsigned char *)data, data_len);

	ret = zend_list_insert(input, le_swfinputp);
	object_init_ex(getThis(), input_class_entry_ptr);
	add_property_resource(getThis(), "input", ret);
	zend_list_addref(ret);
}

static zend_function_entry swfinput_functions[] = {
	PHP_ME(swfinput, __construct,          NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFFontCollection */
static SWFFontCollection getFontCollection(zval *id TSRMLS_DC)
{
	void *fc = SWFgetProperty(id, "fontcollection", strlen("fontcollection"), 
			le_swffontcollectionp TSRMLS_CC);

	if (!fc) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, 
			"Called object is not an SWFFontCollection");
	}
	return (SWFFontCollection)fc;
}

/* {{{ proto swffontcollection::init(filename) */
PHP_METHOD(swffontcollection, __construct)
{
	char *filename;
	long filename_len;
	int ret;
	SWFFontCollection fc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	fc = newSWFFontCollection_fromFile(filename);
	if(fc) 	{
		ret = zend_list_insert(fc, le_swffontcollectionp);
		object_init_ex(getThis(), fontcollection_class_entry_ptr);
		add_property_resource(getThis(), "fontcollection", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto long swffontcollection::getFontCount() */
PHP_METHOD(swffontcollection, getFontCount)
{
	RETURN_LONG(SWFFontCollection_getFontCount(
		getFontCollection(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto SWFFont swffontcollection::getFont(int index) */
PHP_METHOD(swffontcollection, getFont)
{
	long index;
	int ret;
	SWFFont font;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
		return;
	}

	font = SWFFontCollection_getFont(getFontCollection(getThis() TSRMLS_CC), index);

	if(font != NULL) {
		ret = zend_list_insert(font, le_swffontp);
		object_init_ex(return_value, font_class_entry_ptr);
		add_property_resource(return_value, "font", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

static void destroy_SWFFontCollection_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFFontCollection((SWFFontCollection)resource->ptr);
}

static zend_function_entry swffontcollection_functions[] = {
	PHP_ME(swffontcollection, __construct,          NULL, 0)
	PHP_ME(swffontcollection, getFont,              NULL, 0)
	PHP_ME(swffontcollection, getFontCount,         NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ SWFBrowserFont */
static SWFBrowserFont getBrowserFont(zval *id TSRMLS_DC)
{
	void *bf = SWFgetProperty(id, "browserfont", strlen("browserfont"), 
			le_swfbrowserfontp TSRMLS_CC);

	if (!bf) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, 
			"Called object is not an SWFBrowserFont");
	}
	return (SWFBrowserFont)bf;
}

/* {{{ proto swfbrowserfont::_construct(fontname) */
PHP_METHOD(swfbrowserfont, __construct)
{
	char *name;
	long name_len;
	int ret;
	SWFBrowserFont font;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	font = newSWFBrowserFont(name);
	if(font) {
		ret = zend_list_insert(font, le_swfbrowserfontp);
		object_init_ex(getThis(), browserfont_class_entry_ptr);
		add_property_resource(getThis(), "browserfont", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

static void destroy_SWFBrowserFont_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBrowserFont((SWFBrowserFont)resource->ptr);
}

static zend_function_entry swfbrowserfont_functions[] = {
	PHP_ME(swfbrowserfont, __construct,          NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */
/* {{{ SWFCXform
*/
/* {{{ proto void swfcxform::__construct([rAdd, gAdd, bAdd, aAdd, rMult, gMult, bMult, aMult]) */
PHP_METHOD(swfcxform, __construct)
{
	SWFCXform cx;
	int rAdd, gAdd, bAdd, aAdd;
	double rMult, gMult, bMult, aMult;
	int ret;

	switch(ZEND_NUM_ARGS())
	{
	case 0:
		cx = newSWFCXform(0, 0, 0, 0, 1.0, 1.0, 1.0, 1.0);
		break;
	case 8:
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllldddd", 
						&rAdd, &gAdd, &bAdd, &aAdd,
						&rMult, &gMult, &bMult, &aMult) == FAILURE) {
			return;
		}
		cx = newSWFCXform(rAdd, gAdd, bAdd, aAdd, rMult, gMult, bMult, aMult);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ret = zend_list_insert(cx, le_swfcxformp);
	object_init_ex(getThis(), cxform_class_entry_ptr);
	add_property_resource(getThis(), "cx", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto void setColorAdd(r, g, b, a) */
PHP_METHOD(swfcxform, setColorAdd)
{
	int rAdd, gAdd, bAdd, aAdd;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &rAdd, &gAdd, &bAdd, &aAdd) == FAILURE) {
		return;
	}

	SWFCXform_setColorAdd(getCXform(getThis() TSRMLS_CC), rAdd, gAdd, bAdd, aAdd);
}
/* }}} */

/* {{{ proto void setColorMult(r, g, b, a) */
PHP_METHOD(swfcxform, setColorMult)
{
	double rMult, gMult, bMult, aMult;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddd", &rMult, &gMult, &bMult, &aMult) == FAILURE) {
		return;
	}

	SWFCXform_setColorMult(getCXform(getThis() TSRMLS_CC), rMult, gMult, bMult, aMult);
}
/* }}} */

static void destroy_SWFCXform_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFCXform((SWFCXform)resource->ptr);
}

/* {{{ internal function getCXform */
static SWFCXform getCXform(zval *id TSRMLS_DC)
{
	void *cx = SWFgetProperty(id, "cx", strlen("cx"), le_swfcxformp TSRMLS_CC);

	if (!cx) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFCXform");
	}
	return (SWFCXform)cx;
}
/* }}} */

static zend_function_entry swfcxform_functions[] = {
	PHP_ME(swfcxform, __construct,          NULL, 0)
	PHP_ME(swfcxform, setColorAdd,          NULL, 0)
	PHP_ME(swfcxform, setColorMult,		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */




/* {{{ SWFMatrix */

static SWFMatrix getMatrix(zval *id TSRMLS_DC)
{
	void *matrix = SWFgetProperty(id, "matrix", strlen("matrix"), le_swfmatrixp TSRMLS_CC);

	if(!matrix)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFMatrix");
	}
	return (SWFMatrix)matrix;
}

/* {{{ proto double swfmatrix::getScaleX */
PHP_METHOD(swfmatrix, getScaleX)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getScaleX(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfmatrix::getScaleX */
PHP_METHOD(swfmatrix, getScaleY)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getScaleY(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfmatrix::getRotate0 */
PHP_METHOD(swfmatrix, getRotate0)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getRotate0(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfmatrix::getRotate0 */
PHP_METHOD(swfmatrix, getRotate1)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getRotate1(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfmatrix::getTranslateX */
PHP_METHOD(swfmatrix, getTranslateX)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getTranslateX(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfmatrix::getTranslateY */
PHP_METHOD(swfmatrix, getTranslateY)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFMatrix_getTranslateY(getMatrix(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swfmatrix_functions[] = {
	PHP_ME(swfmatrix, getScaleX,		NULL, 0)
	PHP_ME(swfmatrix, getScaleY,		NULL, 0)
	PHP_ME(swfmatrix, getRotate0,		NULL, 0)
	PHP_ME(swfmatrix, getRotate1, 		NULL, 0)
	PHP_ME(swfmatrix, getTranslateX,	NULL, 0)
	PHP_ME(swfmatrix, getTranslateY,	NULL, 0)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ SWFInitAction
*/
/* {{{ proto void swfinitaction::__construct(action)
   Creates a new SWFInitAction object */
PHP_METHOD(swfinitaction, __construct)
{
	SWFInitAction init;
	zval *zaction;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zaction) == FAILURE) {
		return;
	}
	
	init = newSWFInitAction(getAction(zaction TSRMLS_CC));

	ret = zend_list_insert(init, le_swfinitactionp);
	object_init_ex(getThis(), initaction_class_entry_ptr);
	add_property_resource(getThis(), "initaction", ret);
	zend_list_addref(ret);
}
/* no destructor for SWFInitAction, it's not a character */
/* }}} */

/* {{{ internal function getInitAction
   Returns the SWFInitAction object contained in zval *id */
static inline SWFInitAction getInitAction(zval *id TSRMLS_DC)
{
	void *action = SWFgetProperty(id, "initaction", 
		strlen("initaction"), le_swfinitactionp TSRMLS_CC);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFInitAction");
	}
	return (SWFInitAction)action;
}
/* }}} */

static zend_function_entry swfinitaction_functions[] = {
	PHP_ME(swfinitaction, __construct,          NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ SWFAction
*/
/* {{{ proto void swfaction::__construct(string)
   Creates a new SWFAction object, compiling the given script */
PHP_METHOD(swfaction, __construct)
{
	SWFAction action;
	char *script;
	int script_len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &script, &script_len) == FAILURE) {
		return;
	}
	
	action = newSWFAction(script);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Couldn't compile actionscript");
	}
	
	ret = zend_list_insert(action, le_swfactionp);

	object_init_ex(getThis(), action_class_entry_ptr);
	add_property_resource(getThis(), "action", ret);
	zend_list_addref(ret);
}
/* no destructor for SWFAction, it's not a character */
/* }}} */

/* {{{ proto long swfaction::compile(version) */
PHP_METHOD(swfaction, compile)
{
	int version;
	int len, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &version) == FAILURE) {
		return;
	}
	
	ret = SWFAction_compile(getAction(getThis() TSRMLS_CC), version, &len);

	if (ret != 0) {
		RETURN_LONG(-1);
	}
	else {
		RETURN_LONG(len);
	}
}
/* }}} */

/* {{{ internal function getAction
   Returns the SWFAction object contained in zval *id */
static SWFAction getAction(zval *id TSRMLS_DC)
{
	void *action = SWFgetProperty(id, "action", 6, le_swfactionp TSRMLS_CC);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFAction");
	}
	return (SWFAction)action;
}
/* }}} */

static zend_function_entry swfaction_functions[] = {
	PHP_ME(swfaction, __construct,          NULL, 0)
	PHP_ME(swfaction, compile,	 	NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFBitmap 
*/
/* {{{ proto void swfbitmap::__construct(filename or SWFInput [, maskfilename / SWFInput])
   Creates a new SWFBitmap object from jpg (with optional mask) or dbl file */
PHP_METHOD(swfbitmap, __construct)
{
	zval *zfile, *zmask = NULL;
	SWFBitmap bitmap;
	SWFJpegWithAlpha bitmap_alpha;
	SWFInput input = NULL, maskinput = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &zfile, &zmask) == FAILURE) {
		return;
	}

	switch(Z_TYPE_P(zfile))
	{
	case IS_RESOURCE:
		input = getInput_fromFileResource(zfile TSRMLS_CC);
		break;
	case IS_OBJECT:
		input = getInput(zfile TSRMLS_CC);
		break;	
	case IS_STRING:
		input = newSWFInput_filename(Z_STRVAL_P(zfile));
		if(input == NULL)
			php_error(E_ERROR, "opening bitmap file failed");
		zend_list_addref(zend_list_insert(input, le_swfinputp));
		break;

	default:
		php_error(E_ERROR, "swfbitmap::__construct: need either a filename, "
		                   "a file ressource or SWFInput buffer.");
	}

	if (zmask != NULL) {
		switch(Z_TYPE_P(zmask))
		{
		case IS_RESOURCE:
			maskinput = getInput_fromFileResource(zmask TSRMLS_CC);
			break;
		case IS_OBJECT:
			maskinput = getInput(zmask TSRMLS_CC);
			break;	
		case IS_STRING:
			maskinput = newSWFInput_filename(Z_STRVAL_P(zmask));
			if(maskinput == NULL)
				php_error(E_ERROR, "opening mask file failed");
			zend_list_addref(zend_list_insert(maskinput, le_swfinputp));
			break;

		default:
			php_error(E_ERROR, "swfbitmap::__construct: need either a filename, "
			                   "a file ressource or SWFInput buffer.");
		}

		/* XXX: this is very optimistic! is it really a JPEG ?!? */
		bitmap_alpha = newSWFJpegWithAlpha_fromInput(input, maskinput);
		if(bitmap_alpha) {
			ret = zend_list_insert(bitmap_alpha, le_swfbitmapp);
			object_init_ex(getThis(), bitmap_class_entry_ptr);
			add_property_resource(getThis(), "bitmap", ret);
			zend_list_addref(ret);
		}
	} else {
		bitmap = newSWFBitmap_fromInput(input);
		if(bitmap) {
			ret = zend_list_insert(bitmap, le_swfbitmapp);
			object_init_ex(getThis(), bitmap_class_entry_ptr);
			add_property_resource(getThis(), "bitmap", ret);
			zend_list_addref(ret);
		}
	}
}

static void destroy_SWFBitmap_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBitmap((SWFBitmap)resource->ptr);
}
/* }}} */

/* {{{ internal function getBitmap
   Returns the SWFBitmap object contained in zval *id */
static SWFBitmap getBitmap(zval *id TSRMLS_DC)
{
	void *bitmap = SWFgetProperty(id, "bitmap", 6, le_swfbitmapp TSRMLS_CC);

	if (!bitmap) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFBitmap");
	}
	return (SWFBitmap)bitmap;
}
/* }}} */

/* {{{ proto float swfbitmap::getWidth()
   Returns the width of this bitmap */
PHP_METHOD(swfbitmap, getWidth)
{
	if (ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFBitmap_getWidth(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swfbitmap::getHeight()
   Returns the height of this bitmap */
PHP_METHOD(swfbitmap, getHeight)
{
	if (ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFBitmap_getHeight(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swfbitmap_functions[] = {
	PHP_ME(swfbitmap, __construct,         NULL, 0)
	PHP_ME(swfbitmap, getWidth,            NULL, 0)
	PHP_ME(swfbitmap, getHeight,           NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ internal function */
static
SWFButtonRecord getButtonRecord(zval *id TSRMLS_DC)
{
	void *record = SWFgetProperty(id, "buttonrecord", strlen("buttonrecord"), le_swfbuttonrecordp TSRMLS_CC);

	if(!record)
		php_error(E_ERROR, "called object is not an SWFButtonRecord!");

	return (SWFButtonRecord)record;
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::setDepth(int depth)
   sets a button characters depth */
PHP_METHOD(swfbuttonrecord, setDepth)
{
	int depth;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &depth) == FAILURE) {
		return;
	}
	SWFButtonRecord_setDepth(getButtonRecord(getThis() TSRMLS_CC), depth);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::setBlendMode(int mode)
   assigns a blend mode to a button's character */
PHP_METHOD(swfbuttonrecord, setBlendMode)
{
	int mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &mode) == FAILURE) {
		return;
	}
	SWFButtonRecord_setBlendMode(getButtonRecord(getThis() TSRMLS_CC), mode);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::move(double x, double y)
   relative placement */
PHP_METHOD(swfbuttonrecord, move)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_move(getButtonRecord(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::moveTo(double x, double y)
   absolute placement */
PHP_METHOD(swfbuttonrecord, moveTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_moveTo(getButtonRecord(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::rotate(double deg)
   relative rotation */
PHP_METHOD(swfbuttonrecord, rotate)
{
	double deg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &deg) == FAILURE) {
		return;
	}
	SWFButtonRecord_rotate(getButtonRecord(getThis() TSRMLS_CC), deg);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::rotateTo(double deg)
   absolute rotation */
PHP_METHOD(swfbuttonrecord, rotateTo)
{
	double deg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &deg) == FAILURE) {
		return;
	}
	SWFButtonRecord_rotateTo(getButtonRecord(getThis() TSRMLS_CC), deg);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::scale(double x, double y)
   relative scaling */
PHP_METHOD(swfbuttonrecord, scale)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_scale(getButtonRecord(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::scaleTo(double x, double y)
   absolute scaling */
PHP_METHOD(swfbuttonrecord, scaleTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_scaleTo(getButtonRecord(getThis() TSRMLS_CC), x, y);
}
/* }}} */


/* {{{ proto void swfbuttoncharacter::skewX(double x) */
PHP_METHOD(swfbuttonrecord, skewX)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}
	SWFButtonRecord_skewX(getButtonRecord(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewXTo(double x) */
PHP_METHOD(swfbuttonrecord, skewXTo)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}
	SWFButtonRecord_skewXTo(getButtonRecord(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewY(double y) */
PHP_METHOD(swfbuttonrecord, skewY)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_skewY(getButtonRecord(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewYTo(double y) */
PHP_METHOD(swfbuttonrecord, skewYTo)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	SWFButtonRecord_skewYTo(getButtonRecord(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::addFilter(SWFFilter f) */
PHP_METHOD(swfbuttonrecord, addFilter)
{
	zval *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &filter) == FAILURE) {
		return;
	}
	SWFButtonRecord_addFilter(getButtonRecord(getThis() TSRMLS_CC), getFilter(filter TSRMLS_CC)); 
}
/* }}} */

static zend_function_entry swfbuttonrecord_functions[] = {
	PHP_ME(swfbuttonrecord, setDepth,   NULL, 0)
	PHP_ME(swfbuttonrecord, setBlendMode,  NULL, 0)
	PHP_ME(swfbuttonrecord, move,  NULL, 0)
	PHP_ME(swfbuttonrecord, moveTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, rotate,  NULL, 0)
	PHP_ME(swfbuttonrecord, rotateTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, scale,  NULL, 0)
	PHP_ME(swfbuttonrecord, scaleTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewX,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewXTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewY,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewYTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, addFilter, NULL, 0)
	{ NULL, NULL, NULL }
};
#endif

/* {{{ SWFButton
*/
/* {{{ proto void swfbutton::__construct()
   Creates a new SWFButton object */
PHP_METHOD(swfbutton, __construct)
{
	SWFButton button = newSWFButton();
	int ret = zend_list_insert(button, le_swfbuttonp);

	object_init_ex(getThis(), button_class_entry_ptr);
	add_property_resource(getThis(), "button", ret);
	zend_list_addref(ret);
}

static void destroy_SWFButton_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFButton((SWFButton)resource->ptr);
}
/* }}} */

/* {{{ internal function getButton
   Returns the SWFButton object contained in zval *id */
static SWFButton getButton(zval *id TSRMLS_DC)
{
	void *button = SWFgetProperty(id, "button", 6, le_swfbuttonp TSRMLS_CC);

	if (!button) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFButton");
	}
	return (SWFButton)button;
}
/* }}} */

/* {{{ proto void swfbutton::setHit(object SWFCharacter)
   Sets the character for this button's hit test state */
PHP_METHOD(swfbutton, setHit)
{
	zval *zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}

	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_HITSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setOver(object SWFCharacter)
   Sets the character for this button's over state */
PHP_METHOD(swfbutton, setOver)
{
	zval *zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}
	
	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_OVERSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setUp(object SWFCharacter)
   Sets the character for this button's up state */
PHP_METHOD(swfbutton, setUp)
{
	zval *zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}
	
	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_UPSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setDown(object SWFCharacter)
   Sets the character for this button's down state */
PHP_METHOD(swfbutton, setDown)
{
	zval *zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}
	
	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_DOWNSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::addShape(object SWFCharacter, int flags)
   Sets the character to display for the condition described in flags */
PHP_METHOD(swfbutton, addShape)
{
	zval *zchar;
	long flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ol", &zchar, &flags) == FAILURE) {
		return;
	}
	
	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, flags);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto swfbuttonrecord swfbutton::addCharacter(object SWFCharacter, int flags)
   Sets the character to display for the condition described in flags */
PHP_METHOD(swfbutton, addCharacter)
{
	zval *zchar;
	long flags;
	SWFButtonRecord record;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ol", &zchar, &flags) == FAILURE) {
		return;
	}
	
	character = getCharacter(zchar TSRMLS_CC);
	record = SWFButton_addCharacter(button, character, flags);

	if(record != NULL)
	{
		ret = zend_list_insert(record, le_swfbuttonrecordp);
		object_init_ex(return_value, buttonrecord_class_entry_ptr);
		add_property_resource(return_value, "buttonrecord", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfbutton::setMenu(int flag)
	enable track as menu button behaviour */
PHP_METHOD(swfbutton, setMenu)
{
	long flag;
	SWFButton button = getButton(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {
		return;
	}
	SWFButton_setMenu(button, flag);
}
/* }}} */

/* {{{ proto void swfbutton::setScalingGrid(int x, int y, int w, int h) */
PHP_METHOD(swfbutton, setScalingGrid)
{
	long x, y, w, h;
	SWFButton button = getButton(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &x, &y, &w, &h) == FAILURE) {
		return;
	}
	SWFButton_setScalingGrid(button, x, y, w, h);
}
/* }}} */

/* {{{ proto void swfbutton::removeScalingGrid() */
PHP_METHOD(swfbutton, removeScalingGrid)
{
	SWFButton button = getButton(getThis() TSRMLS_CC);
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	SWFButton_removeScalingGrid(button);
}
/* }}} */
#endif

/* {{{ proto void swfbutton::setAction(object SWFAction)
   Sets the action to perform when button is pressed */
PHP_METHOD(swfbutton, setAction)
{
	zval *zaction;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zaction) == FAILURE) {
		return;
	}
	
	action = getAction(zaction TSRMLS_CC);
	SWFButton_addAction(button, action, SWFBUTTON_OVERDOWNTOOVERUP);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto SWFSoundInstance swfbutton::addASound(SWFSound sound, int flags)
   associates a sound with a button transition
	NOTE: the transitions are all wrong _UP, _OVER, _DOWN _HIT  */

PHP_METHOD(swfbutton, addSound)
{
	zval *zsound;
	long flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFSound sound;
	SWFSoundInstance item;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ol", &zsound, &flags) == FAILURE) {
		return;
	}

	sound = getSound(zsound TSRMLS_CC);
	item = SWFButton_addSound(button, sound, flags);

	if(item != NULL) {
		/* try and create a soundinstance object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}
/* }}} */
#endif

/* {{{ proto void swfbutton::addAction(object SWFAction, int flags)
   Sets the action to perform when conditions described in flags is met */
PHP_METHOD(swfbutton, addAction)
{
	zval *zaction;
	long flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ol", &zaction, &flags) == FAILURE) {
		return;
	}
	
	action = getAction(zaction TSRMLS_CC);
	SWFButton_addAction(button, action, flags);
}
/* }}} */

/* {{{ proto int ming_keypress(string str)
   Returns the action flag for keyPress(char) */
PHP_FUNCTION(ming_keypress)
{
	char *key;
	long key_len;
	char c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
		return;
	}

	if (key_len > 1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Only one character expected");
	}
	
	c = key[0];
	RETURN_LONG((c&0x7f)<<9);
}
/* }}} */

static zend_function_entry swfbutton_functions[] = {
	PHP_ME(swfbutton, __construct,   NULL, 0)
	PHP_ME(swfbutton, setHit,        NULL, 0)
	PHP_ME(swfbutton, setOver,       NULL, 0)
	PHP_ME(swfbutton, setUp,         NULL, 0)
	PHP_ME(swfbutton, setDown,       NULL, 0)
	PHP_ME(swfbutton, setAction,     NULL, 0)
	PHP_ME(swfbutton, addShape,      NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfbutton, setMenu,       NULL, 0)
	PHP_ME(swfbutton, setScalingGrid, NULL, 0)
	PHP_ME(swfbutton, removeScalingGrid, NULL, 0)
#endif
	PHP_ME(swfbutton, addAction,     NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfbutton, addSound,      NULL, 0)
	PHP_ME(swfbutton, addCharacter,  NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFDisplayItem
*/
/* {{{ internal function getDisplayItem
   Returns the SWFDisplayItem contained in zval *id */
static SWFDisplayItem getDisplayItem(zval *id TSRMLS_DC)
{
	void *item = SWFgetProperty(id, "displayitem", 11, le_swfdisplayitemp TSRMLS_CC);

	if (!item) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFDisplayItem");
	}

	return (SWFDisplayItem)item;
}
/* }}} */

/* {{{ proto void swfdisplayitem::moveTo(double x, double y)
   Moves this SWFDisplayItem to movie coordinates (x, y) */
PHP_METHOD(swfdisplayitem, moveTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_moveTo(getDisplayItem(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::move(double dx, double dy)
   Displaces this SWFDisplayItem by (dx, dy) in movie coordinates */
PHP_METHOD(swfdisplayitem, move)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_move(getDisplayItem(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::scaleTo(double xScale [, double yScale])
   Scales this SWFDisplayItem by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swfdisplayitem, scaleTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|d", &x, &y) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 1) {
		y = x;
	}
	SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::scale(double xScale, double yScale)
   Multiplies this SWFDisplayItem's current x scale by xScale, its y scale by yScale */
PHP_METHOD(swfdisplayitem, scale)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_scale(getDisplayItem(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotateTo(double degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its original orientation */
PHP_METHOD(swfdisplayitem, rotateTo)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}

	SWFDisplayItem_rotateTo(getDisplayItem(getThis() TSRMLS_CC), degrees);
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotate(double degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its current orientation */
PHP_METHOD(swfdisplayitem, rotate)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_rotate(getDisplayItem(getThis() TSRMLS_CC), degrees);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewXTo(double xSkew)
   Sets this SWFDisplayItem's x skew value to xSkew */
PHP_METHOD(swfdisplayitem, skewXTo)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}

	SWFDisplayItem_skewXTo(getDisplayItem(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewX(double xSkew)
   Adds xSkew to this SWFDisplayItem's x skew value */
PHP_METHOD(swfdisplayitem, skewX)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewX(getDisplayItem(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewYTo(double ySkew)
   Sets this SWFDisplayItem's y skew value to ySkew */
PHP_METHOD(swfdisplayitem, skewYTo)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewYTo(getDisplayItem(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewY(double ySkew)
   Adds ySkew to this SWFDisplayItem's y skew value */
PHP_METHOD(swfdisplayitem, skewY)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewY(getDisplayItem(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMatrix(double a, double b, double c, double d, double x, double y)
   Sets the item's transform matrix */
PHP_METHOD(swfdisplayitem, setMatrix)
{
	double a, b, c, d, x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &a, &b, &c, &d, &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_setMatrix( getDisplayItem(getThis() TSRMLS_CC), a, b, c, d, x, y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setDepth(int depth)
   Sets this SWFDisplayItem's z-depth to depth.  Items with higher depth values are drawn on top of those with lower values */
PHP_METHOD(swfdisplayitem, setDepth)
{
	long depth;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &depth) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_setDepth(getDisplayItem(getThis() TSRMLS_CC), depth);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setRatio(float ratio)
   Sets this SWFDisplayItem's ratio to ratio.  Obviously only does anything if displayitem was created from an SWFMorph */
PHP_METHOD(swfdisplayitem, setRatio)
{
	double ratio;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &ratio) == FAILURE) {
		return;
	}

	SWFDisplayItem_setRatio(getDisplayItem(getThis() TSRMLS_CC), ratio);
}
/* }}} */

/* {{{ proto void swfdisplayitem::addColor(int r, int g, int b [, int a])
   Sets the add color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 0 */
PHP_METHOD(swfdisplayitem, addColor)
{
	long r, g, b, a = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFDisplayItem_setColorAdd(getDisplayItem(getThis() TSRMLS_CC), r, g, b, (int)a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::multColor(float r, float g, float b [, float a])
   Sets the multiply color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 1.0 */
PHP_METHOD(swfdisplayitem, multColor)
{
	double r, g, b, a = 1.0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ddd|d", &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFDisplayItem_setColorMult(getDisplayItem(getThis() TSRMLS_CC), r, g, b, a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setName(string name)
   Sets this SWFDisplayItem's name to name */
PHP_METHOD(swfdisplayitem, setName)
{
	char *name;
	int name_len;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_setName(item, name);
}
/* }}} */

/* {{{ proto void swfdisplayitem::addAction(object SWFAction, int flags)
   Adds this SWFAction to the given SWFSprite instance */
PHP_METHOD(swfdisplayitem, addAction)
{
	zval *zaction;
	long flags;
	SWFAction action;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ol", &zaction, &flags) == FAILURE) {
		return;
	}

	action = getAction(zaction TSRMLS_CC);
	SWFDisplayItem_addAction(item, action, flags);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ swfdisplayitem_remove */

PHP_METHOD(swfdisplayitem, remove)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_remove(item);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMaskLevel(int level)
   defines a MASK layer at level */

PHP_METHOD(swfdisplayitem, setMaskLevel)
{
	long level;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &level) == FAILURE) {
		return;
	}

	SWFDisplayItem_setMaskLevel(getDisplayItem(getThis() TSRMLS_CC), level);
}
/* }}} */

/* {{{ proto void swfdisplayitem::endMask()
   another way of defining a MASK layer */

PHP_METHOD(swfdisplayitem, endMask)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_endMask(item);
}
/* }}} */

/* {{{ swfdisplayitem_getX */

PHP_METHOD(swfdisplayitem, getX)
{
	double x, y;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getPosition(item, &x, &y);
	RETURN_DOUBLE(x);
}
/* }}} */

/* {{{ swfdisplayitem_getY */

PHP_METHOD(swfdisplayitem, getY)
{
	double x, y;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getPosition(item, &x, &y);
	RETURN_DOUBLE(y);
}
/* }}} */

/* {{{ swfdisplayitem_getXScale */

PHP_METHOD(swfdisplayitem, getXScale)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getScale(item, &sx, &sy);
	RETURN_DOUBLE(sx);
}
/* }}} */

/* {{{ swfdisplayitem_getYScale */

PHP_METHOD(swfdisplayitem, getYScale)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getScale(item, &sx, &sy);
	RETURN_DOUBLE(sy);
}
/* }}} */

/* {{{ swfdisplayitem_getXSkew */

PHP_METHOD(swfdisplayitem, getXSkew)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getSkew(item, &sx, &sy);
	RETURN_DOUBLE(sx);
}
/* }}} */

/* {{{ swfdisplayitem_getYSkew */

PHP_METHOD(swfdisplayitem, getYSkew)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getSkew(item, &sx, &sy);
	RETURN_DOUBLE(sy);
}
/* }}} */

/* {{{ swfdisplayitem_getRot */

PHP_METHOD(swfdisplayitem, getRot)
{
	double ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_getRotation(item, &ret);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ proto void swfdisplayitem::cacheAsBitmap(int flag)
   caches item as bitmap. can improve rendering speed */

PHP_METHOD(swfdisplayitem, cacheAsBitmap)
{
	long flag;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {
		return;
	}

	SWFDisplayItem_cacheAsBitmap(getDisplayItem(getThis() TSRMLS_CC), flag);
}
/* }}} */


/* {{{ proto void swfdisplayitem::setBlendMode(int mode)
   adds blending to item */
PHP_METHOD(swfdisplayitem, setBlendMode)
{
	long mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &mode) == FAILURE) {
		return;
	}

	SWFDisplayItem_cacheAsBitmap(getDisplayItem(getThis() TSRMLS_CC), mode);
}
/* }}} */

/* {{{ proto int swfdisplayitem::setDepth()
   gets the items depth */
PHP_METHOD(swfdisplayitem, getDepth)
{
	int ret;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	ret = SWFDisplayItem_getDepth(getDisplayItem(getThis() TSRMLS_CC));
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int swfdisplayitem::flush() */
PHP_METHOD(swfdisplayitem, flush)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFDisplayItem_flush(getDisplayItem(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addFilter(SWFFilter filter) */
PHP_METHOD(swfdisplayitem, addFilter)
{
	zval *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &filter) == FAILURE) {
		return;
	}

	SWFDisplayItem_addFilter(getDisplayItem(getThis() TSRMLS_CC), getFilter(filter TSRMLS_CC)); 

}
/* }}} */

/* {{{ proto void swfdisplayitem::setCXform(cx) */
PHP_METHOD(swfdisplayitem, setCXform)
{
	zval *cx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &cx) == FAILURE) {
		return;
	}

	SWFDisplayItem_setCXform(getDisplayItem(getThis() TSRMLS_CC), getCXform(cx TSRMLS_CC));
}
/* }}} */

/** {{{ proto void swfdisplayitem::getMatrix() */
PHP_METHOD(swfdisplayitem, getMatrix)
{
	SWFMatrix m;
	int ret;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	m = SWFDisplayItem_getMatrix(getDisplayItem(getThis() TSRMLS_CC));
	if(m != NULL)
	{
		ret = zend_list_insert(m, le_swfmatrixp);
		object_init_ex(return_value, matrix_class_entry_ptr);
		add_property_resource(return_value, "matrix", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/** {{{ proto void swfdisplayitem::getCharacter() */
PHP_METHOD(swfdisplayitem, getCharacter)
{
	SWFCharacter c;
	int ret;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	c = SWFDisplayItem_getCharacter(getDisplayItem(getThis() TSRMLS_CC));
	if(c != NULL)
	{
		ret = zend_list_insert(c, le_swfcharacterp);
		object_init_ex(return_value, character_class_entry_ptr);
		add_property_resource(return_value, "character", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

#endif

static zend_function_entry swfdisplayitem_functions[] = {
	PHP_ME(swfdisplayitem, moveTo,      NULL, 0)
	PHP_ME(swfdisplayitem, move,        NULL, 0)
	PHP_ME(swfdisplayitem, scaleTo,     NULL, 0)
	PHP_ME(swfdisplayitem, scale,       NULL, 0)
	PHP_ME(swfdisplayitem, rotateTo,    NULL, 0)
	PHP_ME(swfdisplayitem, rotate,      NULL, 0)
	PHP_ME(swfdisplayitem, skewXTo,     NULL, 0)
	PHP_ME(swfdisplayitem, skewX,       NULL, 0)
	PHP_ME(swfdisplayitem, skewYTo,     NULL, 0)
	PHP_ME(swfdisplayitem, skewY,       NULL, 0)
	PHP_ME(swfdisplayitem, setMatrix,   NULL, 0)
	PHP_ME(swfdisplayitem, setDepth,    NULL, 0)
	PHP_ME(swfdisplayitem, setRatio,    NULL, 0)
	PHP_ME(swfdisplayitem, addColor,    NULL, 0)
	PHP_ME(swfdisplayitem, multColor,   NULL, 0)
	PHP_ME(swfdisplayitem, setName,     NULL, 0)
	PHP_ME(swfdisplayitem, addAction,   NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfdisplayitem, remove,      NULL, 0)
	PHP_ME(swfdisplayitem, setMaskLevel,NULL, 0)
	PHP_ME(swfdisplayitem, endMask,     NULL, 0)
	PHP_ME(swfdisplayitem, getX,        NULL, 0)
	PHP_ME(swfdisplayitem, getY,        NULL, 0)
	PHP_ME(swfdisplayitem, getXScale,   NULL, 0)
	PHP_ME(swfdisplayitem, getYScale,   NULL, 0)
	PHP_ME(swfdisplayitem, getXSkew,    NULL, 0)
	PHP_ME(swfdisplayitem, getYSkew,    NULL, 0)
	PHP_ME(swfdisplayitem, getRot,      NULL, 0)
	PHP_ME(swfdisplayitem, cacheAsBitmap, NULL, 0)
	PHP_ME(swfdisplayitem, setBlendMode, NULL, 0)
	PHP_ME(swfdisplayitem, getDepth,    NULL, 0)
	PHP_ME(swfdisplayitem, flush,    NULL, 0)
	PHP_ME(swfdisplayitem, addFilter,    NULL, 0)
	PHP_ME(swfdisplayitem, setCXform,    NULL, 0)
	PHP_ME(swfdisplayitem, getCharacter,    NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFFill
*/
/* {{{ proto void swffill::__construct()
   Creates a new SWFFill object */
PHP_METHOD(swffill, __construct)
{
  php_error_docref(NULL TSRMLS_CC, E_ERROR, "Instantiating SWFFill won't do any good- use SWFShape::addFill() instead");
}

static void destroy_SWFFill_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	/* this only destroys the shallow wrapper for SWFFillStyle,
	   which SWFShape destroys.  So everything's okay.  I hope. */

	destroySWFFill((SWFFill)resource->ptr);
}
/* }}} */

/* {{{ internal function getFill
   Returns the SWFFill object contained in zval *id */
static SWFFill getFill(zval *id TSRMLS_DC)
{
	void *fill = SWFgetProperty(id, "fill", 4, le_swffillp TSRMLS_CC);

	if (!fill) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFill");
	}
	return (SWFFill)fill;
}
/* }}} */

/* {{{ proto void swffill::moveTo(float x, float y)
   Moves this SWFFill to shape coordinates (x,y) */
PHP_METHOD(swffill, moveTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	
	SWFFill_moveTo(getFill(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swffill::move(float dx, float dy)
   Moves this SWFFill by (dx,dy) */
PHP_METHOD(swffill, move)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	
	SWFFill_move(getFill(getThis() TSRMLS_CC), x, y);
}
/* }}} */


/* {{{ proto void swffill::scaleTo(float xScale [, float yScale])
   Scales this SWFFill by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swffill, scaleTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|d", &x, &y) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 1) {
		y = x;
	} 
	SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swffill::scale(float xScale [, float yScale])
   Scales this SWFFill by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swffill, scale)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|d", &x, &y) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 1) {
		y = x;
	}
	SWFFill_scaleXY(getFill(getThis() TSRMLS_CC), x, y);
}
/* }}} */


/* {{{ proto void swffill::rotateTo(float degrees)
   Rotates this SWFFill the given (clockwise) degrees from its original orientation */
PHP_METHOD(swffill, rotateTo)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}

	SWFFill_rotateTo(getFill(getThis() TSRMLS_CC), degrees);
}
/* }}} */

/* {{{ proto void swffill::rotate(float degrees)
   Rotates this SWFFill the given (clockwise) degrees from its current orientation */
PHP_METHOD(swffill, rotate)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}

	SWFFill_rotate(getFill(getThis() TSRMLS_CC), degrees);
}
/* }}} */


/* {{{ proto void swffill::skewXTo(float xSkew)
   Sets this SWFFill's x skew value to xSkew */
PHP_METHOD(swffill, skewXTo)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}

	SWFFill_skewXTo(getFill(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swffill::skewX(float xSkew)
   Sets this SWFFill's x skew value to xSkew */
PHP_METHOD(swffill, skewX)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}

	SWFFill_skewX(getFill(getThis() TSRMLS_CC), x);
}
/* }}} */

/* {{{ proto void swffill::skewYTo(float ySkew)
   Sets this SWFFill's y skew value to ySkew */
PHP_METHOD(swffill, skewYTo)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}

	SWFFill_skewYTo(getFill(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swffill::skewY(float ySkew)
   Sets this SWFFill's y skew value to ySkew */
PHP_METHOD(swffill, skewY)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}

	SWFFill_skewY(getFill(getThis() TSRMLS_CC), y);
}
/* }}} */

/* {{{ proto void swffill::setMatrix(float a, float b, float c, float d, float x, float y)
   Sets this SWFFill's y matrix values */
PHP_METHOD(swffill, setMatrix)
{
	double a, b, c, d, x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &a, &b, &c, &d, &x, &y) == FAILURE) {
		return;
	}

	SWFFill_setMatrix(getFill(getThis() TSRMLS_CC), a, b, c, d, x, y);
}
/* }}} */

static zend_function_entry swffill_functions[] = {
	PHP_ME(swffill, __construct, NULL, 0)
	PHP_ME(swffill, moveTo,      NULL, 0)
	PHP_ME(swffill, move,        NULL, 0)
	PHP_ME(swffill, scaleTo,     NULL, 0)
	PHP_ME(swffill, scale,       NULL, 0)
	PHP_ME(swffill, rotateTo,    NULL, 0)
	PHP_ME(swffill, rotate,      NULL, 0)
	PHP_ME(swffill, skewXTo,     NULL, 0)
	PHP_ME(swffill, skewX,       NULL, 0)
	PHP_ME(swffill, skewYTo,     NULL, 0)
	PHP_ME(swffill, skewY,       NULL, 0)
	PHP_ME(swffill, setMatrix,   NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFFontCharacter */
#ifdef HAVE_NEW_MING

/* {{{ internal function SWFText getFont(zval *id)
   Returns the Font object in zval *id */
static
SWFFontCharacter getFontCharacter(zval *id TSRMLS_DC)
{
	void *font = SWFgetProperty(id, "fontcharacter", 13, le_swffontcharp TSRMLS_CC);

	if(!font)
		php_error(E_ERROR, "called object is not an SWFFontCharacter!");

	return (SWFFontCharacter)font;
}
/* }}} */

/* {{{ proto void swffontcha::raddChars(string)
   adds characters to a font for exporting font */
PHP_METHOD(swffontchar, addChars)
{
	char *string;
	long string_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	SWFFontCharacter_addChars(getFontCharacter(getThis() TSRMLS_CC), string);
}
/* }}} */

/* {{{ proto void swffontchar::addChars(string)
   adds characters to a font for exporting font */

PHP_METHOD(swffontchar, addUTF8Chars)
{
	char *string;
	long string_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	SWFFontCharacter_addUTF8Chars(getFontCharacter(getThis() TSRMLS_CC), string);
}
/* }}} */

/* {{{ proto void swffontchar::addAllChars()
 *    adds all characters to a font for exporting font */

PHP_METHOD(swffontchar, addAllChars)
{
	if (ZEND_NUM_ARGS() != 0 ) {
		WRONG_PARAM_COUNT;
	}

	SWFFontCharacter_addAllChars(getFontCharacter(getThis() TSRMLS_CC));
}
/* }}} */

static zend_function_entry swffontchar_functions[] = {
	PHP_ME(swffontchar, addChars,   NULL, 0)
	PHP_ME(swffontchar, addUTF8Chars,  NULL, 0)
	PHP_ME(swffontchar, addAllChars, NULL, 0)
	{ NULL, NULL, NULL }
};

#endif
/* }}} */

/* {{{ SWFFont
*/
/* {{{ internal function SWFText getFont(zval *id)
   Returns the Font object in zval *id */
static SWFFont getFont(zval *id TSRMLS_DC)
{
	void *font = SWFgetProperty(id, "font", 4, le_swffontp TSRMLS_CC);

	if (!font) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFont");
	}
	return (SWFFont)font;
}
/* }}} */

/* {{{ proto void swffont::__construct(string filename)
   Creates a new SWFFont object from given file */
PHP_METHOD(swffont, __construct)
{
	char *file;
	long file_len;
	SWFFont font;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		return;
	}

	PHP_MING_FILE_CHK(file);
	font = newSWFFont_fromFile(file);

	if(font == NULL)
	{
		php_error(E_ERROR, "Loading font failed! "
		                   "Please use new SWFBrowserFont(string:fontname) "
		                   "for player/browser fonts.");
	
	}
	
	if(font)
	{
		ret = zend_list_insert(font, le_swffontp);
		object_init_ex(getThis(), font_class_entry_ptr);
		add_property_resource(getThis(), "font", ret);
		zend_list_addref(ret);
	}
}

static void destroy_SWFFont_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlock((SWFBlock)resource->ptr);
}
/* }}} */

/* {{{ proto float swffont::getWidth(string str)
   Calculates the width of the given string in this font at full height */
PHP_METHOD(swffont, getWidth)
{
	char *string;
	long string_len;
	float width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	width = SWFFont_getStringWidth(getFont(getThis() TSRMLS_CC), string);
	RETURN_DOUBLE(width);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto int swffont::getUTF8Width(string)
   Calculates the width of the given string in this font at full height */

PHP_METHOD(swffont, getUTF8Width)
{
	char *string;
	long string_len;
	float width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	width = SWFFont_getUTF8StringWidth(getFont(getThis() TSRMLS_CC), string);
	RETURN_DOUBLE(width);
}

/* {{{ proto int swffont::getglyphcount() */
PHP_METHOD(swffont, getGlyphCount)
{
        RETURN_LONG(SWFFont_getGlyphCount(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto string swffont::getname() */
PHP_METHOD(swffont, getName)
{
        RETURN_STRING(SWFFont_getName(getFont(getThis() TSRMLS_CC)), 0);
}
/* }}} */
#endif

/* {{{ proto float swffont::getAscent()
   Returns the ascent of the font, or 0 if not available */
PHP_METHOD(swffont, getAscent)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getAscent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getDescent()
   Returns the descent of the font, or 0 if not available */
PHP_METHOD(swffont, getDescent)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getDescent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getLeading()
   Returns the leading of the font, or 0 if not available */
PHP_METHOD(swffont, getLeading)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getLeading(getFont(getThis() TSRMLS_CC)));
}
/* }}} */


static zend_function_entry swffont_functions[] = {
	PHP_ME(swffont, __construct,       NULL, 0)
	PHP_ME(swffont, getWidth,          NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swffont, getUTF8Width,      NULL, 0)
#endif
	PHP_ME(swffont, getAscent,         NULL, 0)
	PHP_ME(swffont, getDescent,        NULL, 0)
	PHP_ME(swffont, getLeading,        NULL, 0)
#ifdef HAVE_NEW_MING
        PHP_ME(swffont, getGlyphCount,    NULL, 0)
        PHP_ME(swffont, getName,           NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */


/* {{{ SWFFilterMatrix
*/
/* {{{ proto void swffiltermatrix::__construct(cols, rows, array:vals)
   Creates a new SWFFilterMatrix object */
PHP_METHOD(swffiltermatrix, __construct)
{
	long cols, rows; 
	zval *vals, **data;
	SWFFilterMatrix matrix;
	HashTable *arr_hash;
	int ret, items, i;
	float *values;
	HashPosition pointer;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lla", &cols, &rows, &vals) == FAILURE) {
		return;
	}

	arr_hash = Z_ARRVAL_P(vals);
	items = zend_hash_num_elements(arr_hash);
	if(items != cols * rows)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Can't create FilterMatrix."
		"Not enough / too many items it array");
	}
	values = (float *)malloc(items * sizeof(float));
	for(i = 0, zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); 
	    zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; 
	    zend_hash_move_forward_ex(arr_hash, &pointer), i++) 
	{
		zval temp;
		temp = **data;
		zval_copy_ctor(&temp);
		convert_to_double(&temp);
		values[i] = Z_DVAL(temp);
		zval_dtor(&temp);
	}
	
	matrix = newSWFFilterMatrix(cols, rows, values);
	free(values); // array is copied by libming
	ret = zend_list_insert(matrix, le_swffiltermatrixp);
	object_init_ex(getThis(), filtermatrix_class_entry_ptr);
	add_property_resource(getThis(), "filtermatrix", ret);
	zend_list_addref(ret);
}

static void destroy_SWFFilterMatrix_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFFilterMatrix((SWFFilterMatrix)resource->ptr);
}
/* }}} */

/* {{{ internal function getFilterMatrix
   Returns the SWFFilterMatrix object contained in zval *id */
static SWFFilterMatrix getFilterMatrix(zval *id TSRMLS_DC)
{
	void *matrix = SWFgetProperty(id, "filtermatrix", strlen("filtermatrix"), le_swffiltermatrixp TSRMLS_CC);

	if (!matrix) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFilterMatrix");
	}
	return (SWFFilterMatrix)matrix;
}
/* }}} */
static zend_function_entry swffiltermatrix_functions[] = {
	PHP_ME(swffiltermatrix, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFShadow
*/
/* {{{ proto void swfshadow::__construct(angle, distance, strength)
   Creates a new SWFShadow object */
PHP_METHOD(swfshadow, __construct)
{
	double angle, distance, strength;
	SWFShadow shadow;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ddd", &angle, &distance, &strength) == FAILURE) {
		return;
	}

	shadow = newSWFShadow(angle, distance, strength);
	ret = zend_list_insert(shadow, le_swfshadowp);
	object_init_ex(getThis(), shadow_class_entry_ptr);
	add_property_resource(getThis(), "shadow", ret);
	zend_list_addref(ret);
}

static void destroy_SWFShadow_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFShadow((SWFShadow)resource->ptr);
}
/* }}} */

/* {{{ internal function getShadow
   Returns the SWFShadow object contained in zval *id */
static SWFShadow getShadow(zval *id TSRMLS_DC)
{
	void *shadow = SWFgetProperty(id, "shadow", strlen("shadow"), le_swfshadowp TSRMLS_CC);

	if (!shadow) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFShadow");
	}
	return (SWFShadow)shadow;
}
/* }}} */
static zend_function_entry swfshadow_functions[] = {
	PHP_ME(swfshadow, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFBlur
*/
/* {{{ proto void swfblur::__construct(blurX, blurY, passes)
   Creates a new SWFBlur object */
PHP_METHOD(swfblur, __construct)
{
	double blurX, blurY;
	long passes;
	SWFBlur blur;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ddd", &blurX, &blurY, &passes) == FAILURE) {
		return;
	}

	blur = newSWFBlur(blurX, blurY, passes);
	ret = zend_list_insert(blur, le_swfblurp);
	object_init_ex(getThis(), blur_class_entry_ptr);
	add_property_resource(getThis(), "blur", ret);
	zend_list_addref(ret);
}

static void destroy_SWFBlur_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlur((SWFBlur)resource->ptr);
}
/* }}} */

/* {{{ internal function getBlur
   Returns the SWFBlur object contained in zval *id */
static SWFBlur getBlur(zval *id TSRMLS_DC)
{
	void *blur = SWFgetProperty(id, "blur", strlen("blur"), le_swfblurp TSRMLS_CC);

	if (!blur) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFBlur");
	}
	return (SWFBlur)blur;
}
/* }}} */
static zend_function_entry swfblur_functions[] = {
	PHP_ME(swfblur, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ SWFGradient
*/
/* {{{ proto void swfgradient::__construct()
   Creates a new SWFGradient object */
PHP_METHOD(swfgradient, __construct)
{
	SWFGradient gradient = newSWFGradient();
	int ret = zend_list_insert(gradient, le_swfgradientp);

	object_init_ex(getThis(), gradient_class_entry_ptr);
	add_property_resource(getThis(), "gradient", ret);
	zend_list_addref(ret);
}

static void destroy_SWFGradient_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFGradient((SWFGradient)resource->ptr);
}
/* }}} */

/* {{{ internal function getGradient
   Returns the SWFGradient object contained in zval *id */
static SWFGradient getGradient(zval *id TSRMLS_DC)
{
	void *gradient = SWFgetProperty(id, "gradient", 8, le_swfgradientp TSRMLS_CC);

	if (!gradient) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFGradient");
	}
	return (SWFGradient)gradient;
}
/* }}} */

/* {{{ proto void swfgradient::addEntry(float ratio, int r, int g, int b [, int a])
   Adds given entry to the gradient */
PHP_METHOD(swfgradient, addEntry)
{
	double ratio;
	long r, g, b, a = 0xff;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dlll|l", &ratio, &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFGradient_addEntry( getGradient(getThis() TSRMLS_CC), ratio, (byte)r, (byte)g, (byte)b, (byte)a);
}
/* }}} */

/* {{{ proto void swfgradient::setSpreadMode(mode)
   supported mode values:
   * SWF_GRADIENT_PAD  
   * SWF_GRADIENT_REFLECT
   * SWF_GRADIENT_REPEAT
*/
PHP_METHOD(swfgradient, setSpreadMode)
{
	long val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
		return;
	}

	SWFGradient_setSpreadMode(getGradient(getThis() TSRMLS_CC), val); 
}
/* }}} */

/* {{{ proto void swfgradient::setInterpolationMode(mode)
 * supported mode values:
   * SWF_GRADIENT_NORMAL
   * SWF_GRADIENT_LINEAR
*/
PHP_METHOD(swfgradient, setInterpolationMode)
{
	long val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
		return;
	}

	SWFGradient_setInterpolationMode(getGradient(getThis() TSRMLS_CC), val); 
}
/* }}} */

/* {{{ proto void swfgradient::setFocalPoint(mode) */
PHP_METHOD(swfgradient, setFocalPoint)
{
	long val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
		return;
	}

	SWFGradient_setFocalPoint(getGradient(getThis() TSRMLS_CC), val); 
}
/* }}} */

static zend_function_entry swfgradient_functions[] = {
	PHP_ME(swfgradient, __construct, 		NULL, 0)
	PHP_ME(swfgradient, addEntry,    		NULL, 0)
	PHP_ME(swfgradient, setSpreadMode,		NULL, 0)
	PHP_ME(swfgradient, setInterpolationMode,	NULL, 0)
	PHP_ME(swfgradient, setFocalPoint,		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFFilter
*/

/* helper functions */
static SWFColor hashToColor(zval **colorHash)
{
	zval **data;
	HashPosition pointer;
	HashTable *arr_hash;
	SWFColor c;

	c.alpha = 0xff;
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	
	arr_hash = Z_ARRVAL_PP(colorHash);
	if(zend_hash_num_elements(arr_hash) < 3 || zend_hash_num_elements(arr_hash) > 4)
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "not a valid colorHash\n");

	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); 
	    zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; 
	    zend_hash_move_forward_ex(arr_hash, &pointer)) 
	{
		zval temp;
		char *key;
		unsigned int key_len;
		unsigned long index;
		
		temp = **data;
		if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) 
			== HASH_KEY_IS_STRING)
		{
			zval_copy_ctor(&temp);
			convert_to_long(&temp);
			if(strcmp(key, "red") == 0)
				c.red = Z_LVAL(temp);
			else if (strcmp(key, "green") == 0)
				c.green = Z_LVAL(temp);
			else if (strcmp(key, "blue") == 0)
				c.blue = Z_LVAL(temp);
			else if (strcmp(key, "alpha") == 0)
				c.alpha = Z_LVAL(temp);
			else
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "not a valid colorHash\n");

			zval_dtor(&temp);
		}
	}
	return c;
}

static SWFFilter createDropShadowFilter(int argc, zval **argv[])
{
	zval **colorHash, **blur, **shadow, **flags;
	SWFColor c;
	
	if(argc != 5)
		return NULL;

	colorHash = argv[1];
	convert_to_array_ex(colorHash);
	c = hashToColor(colorHash); 
		
	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newDropShadowFilter(c, getBlur(*blur TSRMLS_CC), 
		getShadow(*shadow TSRMLS_CC), Z_LVAL_PP(flags));
}

static SWFFilter createBlurFilter(int argc, zval **argv[])
{
	zval **blur;
	
	if(argc != 2)
		return NULL;

	blur = argv[1];
	convert_to_object_ex(blur);
	
	return newBlurFilter(getBlur(*blur TSRMLS_CC));
}

static SWFFilter createGlowFilter(int argc, zval **argv[])
{
	zval **color, **blur, **strength, **flags;
	SWFColor c;
	
	if(argc != 5)
		return NULL;
	
	color = argv[1];
	convert_to_array_ex(color);
	c = hashToColor(color);

	blur = argv[2];
	convert_to_object_ex(blur);

	strength = argv[3];
	convert_to_double_ex(strength);
	
	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGlowFilter(c, getBlur(*blur TSRMLS_CC), 
		Z_DVAL_PP(strength), Z_LVAL_PP(flags));
}

static SWFFilter createBevelFilter(int argc, zval **argv[])
{
	zval **sColor, **hColor, **blur, **shadow, **flags;
	SWFColor hc, sc;

	if(argc != 6)
		return NULL;

	sColor = argv[1];
	convert_to_array_ex(sColor);
	sc = hashToColor(sColor);

	hColor = argv[2];
	convert_to_array_ex(hColor);
	hc = hashToColor(hColor);

	blur = argv[3];
	convert_to_object_ex(blur);

	shadow = argv[4];
	convert_to_object_ex(shadow);

	flags = argv[5]; 
	convert_to_long_ex(flags);

	return newBevelFilter(sc, hc, getBlur(*blur TSRMLS_CC),
		getShadow(*shadow TSRMLS_CC), Z_LVAL_PP(flags));
}

static SWFFilter createGradientGlowFilter(int argc, zval **argv[])
{
	zval **gradient, **blur, **shadow, **flags;

	if(argc != 5)
		return NULL;		

	gradient = argv[1];
	convert_to_object_ex(gradient);

	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGradientGlowFilter(getGradient(*gradient TSRMLS_CC), 
		getBlur(*blur TSRMLS_CC), getShadow(*shadow TSRMLS_CC), 
		Z_LVAL_PP(flags));
}

static SWFFilter createConvolutionFilter(int argc, zval **argv[])
{
	zval **matrix, **div, **bias, **color, **flags;
	SWFColor c;

	if(argc != 6)
		return NULL;

	matrix = argv[1];
	convert_to_object_ex(matrix);

	div = argv[2];
	convert_to_double_ex(div);

	bias = argv[3];
	convert_to_double_ex(bias);

	color = argv[4];
	convert_to_array_ex(color);
	c = hashToColor(color);

	flags = argv[5];
	convert_to_long_ex(flags);
	
	return newConvolutionFilter(getFilterMatrix(*matrix TSRMLS_CC), Z_DVAL_PP(div),
		Z_DVAL_PP(bias), c, Z_LVAL_PP(flags));	
}

static SWFFilter createColorMatrixFilter(int argc, zval **argv[])
{
	zval **matrix;

	if(argc != 2)
		return NULL;

	matrix = argv[1];
	convert_to_object_ex(matrix);

	return newColorMatrixFilter(getFilterMatrix(*matrix TSRMLS_CC));
}

static SWFFilter createGradientBevelFilter(int argc, zval **argv[])
{
	zval **gradient, **blur, **shadow, **flags;

	if(argc != 5)
		return NULL;		

	gradient = argv[1];
	convert_to_object_ex(gradient);

	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGradientBevelFilter(getGradient(*gradient TSRMLS_CC), 
		getBlur(*blur TSRMLS_CC), getShadow(*shadow TSRMLS_CC), 
		Z_LVAL_PP(flags));
}

/* {{{ proto void swffilter::__construct(type, ...)
   Creates a new SWFFilter object:
   
   Supported filter types:
   * SWFFILTER_TYPE_DROPSHADOW
     new SWFFilter(SWFFILTER_TYPE_DROPSHADOW, colorHash, blur, shadow, flags);
   * SWFFILTER_TYPE_BLUR
     new SWFFilter(SWFFILTER_TYPE_BLUR, blur);
   * SWFFILTER_TYPE_GLOW
     new SWFFilter(SWFFILTER_TYPE_GLOW, colorHash, blur, strenght:float, flags); 
   * SWFFILTER_TYPE_BEVEL
     new SWFFilter(SWFFILTER_TYPE_BEVEL, colorHash_shadow, colorHash_highlight, blur, shadow, flags); 
   * SWFFILTER_TYPE_GRADIENTGLOW
     new SWFFilter(SWFFILTER_TYPE_GRADIENTGLOW, gradient, blur, shadow, flags);
   * SWFFILTER_TYPE_CONVOLUTION
     new SWFFilter(SWFFILTER_TYPE_CONVOLUTION, filterMatrix, divisor:float, bias:float, colorHash, flags);
   * SWFFILTER_TYPE_COLORMATRIX
     new SWFFilter(SWFFILTER_TYPE_COLORMATRIX, filterMatrix);
   * SWFFILTER_TYPE_GRADIENTBEVEL
     new SWFFilter(SWFFILTER_TYPE_GRADIENTBEVEL, gradient, blur, shadow, flags);

   Supported flags are:
   * SWFFILTER_FLAG_CLAMP
   * SWFFILTER_FLAG_PRESERVE_ALPHA
   * SWFFILTER_MODE_INNER
   * SWFFILTER_MODE_KO

   A colorHash must have the following entries:
   'red'   => 0...255, 
   'green' =>  0...255,
   'blue'  => 0...255,
   'alpha' => 0...255 (optional)
*/
PHP_METHOD(swffilter, __construct)
{
	zval **argv[6];
	int argc = ZEND_NUM_ARGS();
	int type, ret;
	SWFFilter filter = NULL;
	
	if (argc > 6 || argc < 2 || zend_get_parameters_array_ex(argc, argv) == FAILURE) 
		WRONG_PARAM_COUNT;
	

	convert_to_long_ex(argv[0]);
	type = Z_LVAL_PP(argv[0]);
	switch(type)
	{
	case SWFFILTER_TYPE_DROPSHADOW:
		filter = createDropShadowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_BLUR:
		filter = createBlurFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GLOW:
		filter = createGlowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_BEVEL:
		filter = createBevelFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GRADIENTGLOW:
		filter = createGradientGlowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_CONVOLUTION:
		filter = createConvolutionFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_COLORMATRIX:
		filter  = createColorMatrixFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GRADIENTBEVEL:
		filter = createGradientBevelFilter(argc, argv);
		break;
	default:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "new SWFFilter: unknown type");	
	}
	
	if(filter == NULL)
		WRONG_PARAM_COUNT;

	ret = zend_list_insert(filter, le_swffilterp);
	object_init_ex(getThis(), filter_class_entry_ptr);
	add_property_resource(getThis(), "filter", ret);
	zend_list_addref(ret);	
}
/* }}} */

/* {{{ internal function getFilter
   Returns the SWFFilter object contained in zval *id */
static SWFFilter getFilter(zval *id TSRMLS_DC)
{
	void *filter = SWFgetProperty(id, "filter", 
		strlen("filter"), le_swffilterp TSRMLS_CC);

	if (!filter) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFilter");
	}
	return (SWFFilter)filter;
}
/* }}} */

static void destroy_SWFFilter_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFFilter((SWFFilter)resource->ptr);
}

static zend_function_entry swffilter_functions[] = {
	PHP_ME(swffilter, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */



/* {{{ SWFMorph 
*/
/* {{{ proto void swfmorph::__construct()
   Creates a new SWFMorph object */
PHP_METHOD(swfmorph, __construct)
{
	SWFMorph morph = newSWFMorphShape();
	int ret = zend_list_insert(morph, le_swfmorphp);

	object_init_ex(getThis(), morph_class_entry_ptr);
	add_property_resource(getThis(), "morph", ret);
	zend_list_addref(ret);
}

static void destroy_SWFMorph_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMorph((SWFMorph)resource->ptr);
}
/* }}} */

/* {{{ internal function getMorph
   Returns the SWFMorph object contained in zval *id */
static SWFMorph getMorph(zval *id TSRMLS_DC)
{
	void *morph = SWFgetProperty(id, "morph", 5, le_swfmorphp TSRMLS_CC);

	if (!morph) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFMorph");
	}
	return (SWFMorph)morph;
}
/* }}} */

/* {{{ proto object swfmorph::getShape1()
   Return's this SWFMorph's start shape object */
PHP_METHOD(swfmorph, getShape1)
{
	SWFMorph morph = getMorph(getThis() TSRMLS_CC);
	SWFShape shape = SWFMorph_getShape1(morph);
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(return_value, shape_class_entry_ptr);
	add_property_resource(return_value, "shape", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto object swfmorph::getShape2()
   Return's this SWFMorph's start shape object */
PHP_METHOD(swfmorph, getShape2)
{
	SWFMorph morph = getMorph(getThis() TSRMLS_CC);
	SWFShape shape = SWFMorph_getShape2(morph);
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(return_value, shape_class_entry_ptr);
	add_property_resource(return_value, "shape", ret);
	zend_list_addref(ret);
}
/* }}} */

static zend_function_entry swfmorph_functions[] = {
	PHP_ME(swfmorph, __construct,     NULL, 0)
	PHP_ME(swfmorph, getShape1,       NULL, 0)
	PHP_ME(swfmorph, getShape2,       NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSoundStream
 */

/* {{{ proto class soundstream::init(file) */
PHP_METHOD(swfsoundstream, __construct)
{
	zval *zfile;
	SWFSoundStream sound = NULL;
	SWFInput input = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zfile) == FAILURE) {
		return;
	}
	
	switch(Z_TYPE_P(zfile))
	{
	case IS_RESOURCE:
		input = getInput_fromFileResource(zfile TSRMLS_CC);
		break;
	case IS_OBJECT:
		input = getInput(zfile TSRMLS_CC);
		break;	
	case IS_STRING:
		input = newSWFInput_filename(Z_STRVAL_P(zfile));
		if(input == NULL)
			php_error(E_ERROR, "opening sound file failed");
		zend_list_addref(zend_list_insert(input, le_swfinputp));
		break;

	default:
		php_error(E_ERROR, "soundstream::init: need either a filename, "
		                   "a file ressource or SWFInput buffer.");
	}
		
	sound = newSWFSoundStream_fromInput(input);
	
	if(sound) {
		ret = zend_list_insert(sound, le_swfsoundstreamp);
		object_init_ex(getThis(), soundstream_class_entry_ptr);
		add_property_resource(getThis(), "soundstream", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function getSoundStream
   Returns the SWFSoundStream object contained in zval *id */
static SWFSoundStream getSoundStream(zval *id TSRMLS_DC)
{
	void *sound = SWFgetProperty(id, "soundstream", strlen("soundstream"), 
	                             le_swfsoundstreamp TSRMLS_CC);
	if(!sound)
		php_error(E_ERROR, "called object is not an SWFSoundStream!");
	return (SWFSoundStream)sound;
}
/* }}} */

/* {{{ internal function destroy_SWFSoundStream */
static void destroy_SWFSoundStream_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFSoundStream((SWFSoundStream)resource->ptr);
}
/* }}} */

/* {{{
   returns the duration of the sound stream */
PHP_METHOD(swfsoundstream, getDuration) 
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(SWFSoundStream_getDuration(getSoundStream(getThis() TSRMLS_CC)));
}
/* }}} */


static zend_function_entry swfsoundstream_functions[] = {
	PHP_ME(swfsoundstream, __construct, NULL, 0)
	PHP_ME(swfsoundstream, getDuration, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */



/* {{{ SWFSound */
/* {{{ internal function SWFSound getSound(zval *id)
   Returns the Sound object in zval *id */

SWFSound getSound(zval *id TSRMLS_DC)
{
	void *sound = SWFgetProperty(id, "sound", 5, le_swfsoundp TSRMLS_CC);

	if(!sound)
		php_error(E_ERROR, "called object is not an SWFSound!");

	return (SWFSound)sound;
}

/* }}} */
/* {{{ proto void swfsound::__construct(string filename/SWFInput/SWFSoundstream[, int flags])
   Creates a new SWFSound object from given file 
   Takes either a Filename or SWFInput memory buffer AND flags or
   a SWFSoundStream object with NO flags.
*/
PHP_METHOD(swfsound, __construct)
{
	zval *zfile;
	long flags;
	SWFSound sound = NULL;
	SWFInput input = NULL;
	SWFSoundStream stream;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zfile, &flags) == FAILURE) {
		return;
	}
	
	if(ZEND_NUM_ARGS() == 1)
	{
		stream = getSoundStream(zfile TSRMLS_CC);
		sound = newSWFSound_fromSoundStream(stream);
	}
	else if(ZEND_NUM_ARGS() == 2)
	{
		switch(Z_TYPE_P(zfile))
		{
		case IS_RESOURCE:
			input = getInput_fromFileResource(zfile TSRMLS_CC);
			break;
		case IS_OBJECT:
			input = getInput(zfile TSRMLS_CC);
			break;	
		case IS_STRING:
			input = newSWFInput_filename(Z_STRVAL_P(zfile));
			if(input == NULL)
				php_error(E_ERROR, "opening sound file failed");
			zend_list_addref(zend_list_insert(input, le_swfinputp));
			break;

		default:
			php_error(E_ERROR, "swfsound::__construct: need either a filename, "
			                   "a file ressource or SWFInput buffer.");
		}
		sound = newSWFSound_fromInput(input, flags);
	}

	if(sound != NULL)
	{	
		ret = zend_list_insert(sound, le_swfsoundp);
		object_init_ex(getThis(), sound_class_entry_ptr);
		add_property_resource(getThis(), "sound", ret);
		zend_list_addref(ret);
	}
}

static void destroy_SWFSound_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlock((SWFBlock)resource->ptr);
}

/* }}} */

static zend_function_entry swfsound_functions[] = {
	PHP_ME(swfsound, __construct,             NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSoundInstance */
#ifdef HAVE_NEW_MING
/* should handle envelope functions */
/* {{{ internal function SWFSoundInstance getSoundInstance(zval *id)
   Returns the SoundInstance object in zval *id */

SWFSoundInstance getSoundInstance(zval *id TSRMLS_DC)
{
	void *inst = SWFgetProperty(id, "soundinstance", 13, le_swfsoundinstancep TSRMLS_CC);

	if (!inst)
		php_error(E_ERROR, "called object is not an SWFSoundInstance!");

	return (SWFSoundInstance)inst;
}
/* }}} */

/* {{{ swfsoundinstance_nomultiple */

PHP_METHOD(swfsoundinstance, noMultiple)
{
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFSoundInstance_setNoMultiple(inst);
}
/* }}} */

/* {{{ swfsoundinstance_loopinpoint(point) */

PHP_METHOD(swfsoundinstance, loopInPoint)
{
	long point;

	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &point) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopInPoint(inst, point);
}

/* }}} */
/* {{{ swfsoundinstance_loopoutpoint(point) */

PHP_METHOD(swfsoundinstance, loopOutPoint)
{
	long point;

	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &point) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopOutPoint(inst, point);
}
/* }}} */

/* {{{ swfsoundinstance_loopcount(point) */

PHP_METHOD(swfsoundinstance, loopCount)
{
	long count;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &count) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopCount(inst, count);
}
/* }}} */

static zend_function_entry swfsoundinstance_functions[] = {
	PHP_ME(swfsoundinstance, noMultiple,    NULL, 0)
	PHP_ME(swfsoundinstance, loopInPoint,   NULL, 0)
	PHP_ME(swfsoundinstance, loopOutPoint,  NULL, 0)
	PHP_ME(swfsoundinstance, loopCount,     NULL, 0)
	{ NULL, NULL, NULL }
};

/* {{{ SWFVideoStream */

/* {{{ proto class swfvideostream_init([filename])
   Returns a SWVideoStream object */
PHP_METHOD(swfvideostream, __construct)
{
	zval *zfile = NULL;
	SWFVideoStream stream;
	SWFInput input = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &zfile) == FAILURE) {
		return;
	}
	switch(ZEND_NUM_ARGS()) 
	{
	case 1:
	
		switch(Z_TYPE_P(zfile))
		{
		case IS_RESOURCE:
			input = getInput_fromFileResource(zfile TSRMLS_CC);
			break;
		case IS_OBJECT:
			input = getInput(zfile TSRMLS_CC);
			break;	
		case IS_STRING:
			input = newSWFInput_filename(Z_STRVAL_P(zfile));
			if(input == NULL)
				php_error(E_ERROR, "opening sound video failed");
			zend_list_addref(zend_list_insert(input, le_swfinputp));
			break;

		default:
			php_error(E_ERROR, "swfvideostream_init: need either a filename, "
			                   "a file ressource or SWFInput buffer.");
		}
		stream = newSWFVideoStream_fromInput(input);
		break;
	case 0:
		stream = newSWFVideoStream();
		break;
	}
	
	if(stream) {
		ret = zend_list_insert(stream, le_swfvideostreamp);
		object_init_ex(getThis(), videostream_class_entry_ptr);
		add_property_resource(getThis(), "videostream", ret);
		zend_list_addref(ret);
	}
}	

static void destroy_SWFVideoStream_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFVideoStream((SWFVideoStream)resource->ptr);
}
/* }}} */

/* {{{ internal function getVideoStream
   Returns the SWFVideoStream object contained in zval *id */
static SWFVideoStream getVideoStream(zval *id TSRMLS_DC)
{
	void *stream = SWFgetProperty(id, "videostream", 11, le_swfvideostreamp TSRMLS_CC);
	if(!stream)
		php_error(E_ERROR, "called object is not an SWFVideoStream!");
	return (SWFVideoStream)stream;
}

/* }}} */

/* {{{ setDimension */

PHP_METHOD(swfvideostream, setdimension)
{
	long x, y;
	SWFVideoStream stream = getVideoStream(getThis() TSRMLS_CC);
	if(!stream)
		 php_error(E_ERROR, "getVideoStream returned NULL");

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &x, &y) == FAILURE) {
		return;
	}

	SWFVideoStream_setDimension(stream, x, y);
}
/* }}} */

/* {{{ getNumFrames */
PHP_METHOD(swfvideostream, getnumframes) 
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(SWFVideoStream_getNumFrames(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ hasAudio */
PHP_METHOD(swfvideostream, hasaudio) 
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(SWFVideoStream_hasAudio(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */

		
		
static zend_function_entry swfvideostream_functions[] = {
	PHP_ME(swfvideostream, 	__construct,	NULL, 0)
	PHP_ME(swfvideostream, setdimension, NULL, 0)
	PHP_ME(swfvideostream, getnumframes, NULL, 0)
	PHP_ME(swfvideostream, hasaudio, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFBinaryData
 */

/* {{{ proto class swfbinarydata_init(string)
    Returns a SWFBinaryData object */
PHP_METHOD(swfbinarydata, __construct)
{
	char *data;
	long data_len;
	SWFBinaryData bd = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		return;
	}
	bd = newSWFBinaryData((unsigned char *)data, data_len);
	
	if(bd) {
		ret = zend_list_insert(bd, le_swfbinarydatap);
		object_init_ex(getThis(), binarydata_class_entry_ptr);
		add_property_resource(getThis(), "binarydata", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function getBinaryData
   Returns the SWFBinaryData object contained in zval *id */
static inline SWFBinaryData getBinaryData(zval *id TSRMLS_DC)
{
	void *bd = SWFgetProperty(id, "binarydata", strlen("binarydata"), le_swfbinarydatap TSRMLS_CC);
	if(!bd)
		php_error(E_ERROR, "called object is not an SWFBinaryData!");
	return (SWFBinaryData)bd;
}
/* }}} */

/* {{{ internal function destroy_SWFBinaryData */
static void destroy_SWFBinaryData_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBinaryData((SWFBinaryData)resource->ptr);
}
/* }}} */

static zend_function_entry swfbinarydata_functions[] = {
	PHP_ME(swfbinarydata, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFPrebuiltClip */
/* {{{ proto class swfprebuiltclip_init(filename / SWFInput )
    Returns a SWFPrebuiltClip object */
PHP_METHOD(swfprebuiltclip, __construct)
{
	zval *zfile = NULL;
	SWFPrebuiltClip clip;
	SWFInput input = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zfile) == FAILURE) {
		return;
	}
	
	switch(Z_TYPE_P(zfile))
	{
	case IS_RESOURCE:
		input = getInput_fromFileResource(zfile TSRMLS_CC);
		break;
	case IS_OBJECT:
		input = getInput(zfile TSRMLS_CC);
		break;	
	case IS_STRING:
		input = newSWFInput_filename(Z_STRVAL_P(zfile));
		if(input == NULL)
			php_error(E_ERROR, "opening prebuilt clip file failed");
		zend_list_addref(zend_list_insert(input, le_swfinputp));
		break;

	default:
		php_error(E_ERROR, "swfprebuiltclip_init: need either a filename, "
		                   "a file ressource or SWFInput buffer.");
	}
		
	clip = newSWFPrebuiltClip_fromInput(input);
	
	if(clip) {
		ret = zend_list_insert(clip, le_swfprebuiltclipp);
		object_init_ex(getThis(), prebuiltclip_class_entry_ptr);
		add_property_resource(getThis(), "prebuiltclip", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function destroy_SWFPrebuiltClip */
static void destroy_SWFPrebuiltClip_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFPrebuiltClip((SWFPrebuiltClip)resource->ptr);
}
/* }}} */

/* {{{ internal function getPrebuiltClip
   Returns the SWFPrebuiltClip object contained in zval *id */
static SWFPrebuiltClip getPrebuiltClip(zval *id TSRMLS_DC)
{
	void *clip = SWFgetProperty(id, "prebuiltclip", 12, le_swfprebuiltclipp TSRMLS_CC);
	if(!clip)
		php_error(E_ERROR, "called object is not an SWFPrebuiltClip!");
	return (SWFPrebuiltClip)clip;
}
/* }}} */

static zend_function_entry swfprebuiltclip_functions[] = {
	PHP_ME(swfprebuiltclip, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

/* {{{ SWFMovie
*/
/* {{{ proto void swfmovie::__construct(int version) 
  Creates swfmovie object according to the passed version */
PHP_METHOD(swfmovie, __construct)
{
	long version;
	SWFMovie movie;
	int ret;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &version) == FAILURE) {
			return;
		}
		movie = newSWFMovieWithVersion(version);
	} else {
		movie = newSWFMovie(); /* default version 4 */
	}
	
	ret = zend_list_insert(movie, le_swfmoviep);
	
	object_init_ex(getThis(), movie_class_entry_ptr);
	add_property_resource(getThis(), "movie", ret);
	zend_list_addref(ret);
}

static void destroy_SWFMovie_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMovie((SWFMovie)resource->ptr);
}
/* }}} */

/* {{{ getMovie
*/
static SWFMovie getMovie(zval *id TSRMLS_DC)
{
	void *movie = SWFgetProperty(id, "movie", 5, le_swfmoviep TSRMLS_CC);

	if (!movie) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFMovie");
	}
	return (SWFMovie)movie;
}
/* }}} */

/* {{{ proto void swfmovie::nextframe()  
*/
PHP_METHOD(swfmovie, nextFrame)
{
	SWFMovie_nextFrame(getMovie(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfmovie::labelframe(string label)
   Labels frame */
PHP_METHOD(swfmovie, labelFrame)
{
	char *label;
	long label_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &label, &label_len) == FAILURE) {
		return;
	}

	SWFMovie_labelFrame(getMovie(getThis() TSRMLS_CC), label);
}
/* }}} */

/* {{{ proto void swfmovie::namedanchor()
*/
PHP_METHOD(swfmovie, namedAnchor)
{
	char *label;
	long label_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &label, &label_len) == FAILURE) {
		return;
	}

	SWFMovie_namedAnchor(getMovie(getThis() TSRMLS_CC), label);
}
/* }}} */

/* {{{ proto void swfmovie::protect([ string pasword])
*/
PHP_METHOD(swfmovie, protect)
{
	char *passwd = NULL;
	long passwd_len;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &passwd, &passwd_len) == FAILURE) {
		return;
	}
	SWFMovie_protect(movie, passwd);
}
/* }}} */

/* {{{ proto object swfmovie::add(object SWFBlock) 
*/
PHP_METHOD(swfmovie, add)
{
	zval *zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}

	/* XXX - SWFMovie_add deals w/ all block types.  Probably will need to add that.. */
	if (Z_OBJCE_P(zchar) == action_class_entry_ptr) {
		block = (SWFBlock) getAction(zchar TSRMLS_CC);
	} 
	else if(Z_OBJCE_P(zchar) == character_class_entry_ptr) {
		block = (SWFBlock) getCharacterClass(zchar TSRMLS_CC);
	}
	else {
		block = (SWFBlock) getCharacter(zchar TSRMLS_CC);
	}

	item = SWFMovie_add_internal(movie, (SWFMovieBlockType)block);
	if (item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfdisplayitemp);
		object_init_ex(return_value, displayitem_class_entry_ptr);
		add_property_resource(return_value, "displayitem", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfmovie::remove(object SWFBlock)
*/
PHP_METHOD(swfmovie, remove)
{
	zval *zchar;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}
	item = getDisplayItem(zchar TSRMLS_CC);
	SWFMovie_remove(movie, item);
}
/* }}} */

/* {{{ proto int swfmovie::output([int compression])
*/
static void phpByteOutputMethod(byte b, void *data)
{
	TSRMLS_FETCH();

	php_write(&b, 1 TSRMLS_CC);
}

PHP_METHOD(swfmovie, output)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
#if !defined(HAVE_NEW_MING)
	int limit = -1;
	int oldval = INT_MIN;
	long out; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &limit) == FAILURE) {
		return;
	}
	oldval = Ming_setSWFCompression(limit);			
	out = SWFMovie_output(movie, &phpByteOutputMethod, NULL);
	Ming_setSWFCompression(oldval);
		
	RETURN_LONG(out);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	int limit = -1;
	int argc = ZEND_NUM_ARGS();

	if(argc) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &limit) == FAILURE) {
			return;
		}

		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
	}
	RETURN_LONG(SWFMovie_output(movie, &phpByteOutputMethod, NULL, limit));
#else
	RETURN_LONG(SWFMovie_output(movie, &phpByteOutputMethod, NULL));
#endif

}
/* }}} */

/* {{{ proto int swfmovie::saveToFile(stream x [, int compression])
*/
static void phpStreamOutputMethod(byte b, void * data)
{
	TSRMLS_FETCH();

	php_stream_write((php_stream*)data, &b, 1);
}

/* I'm not sure about the logic here as Ming_setSWFCompression() should be
 * used with current Ming. I'll have to add some tests to the test suite to
 * verify this functionallity before I can say for sure
 */
PHP_METHOD(swfmovie, saveToFile)
{
	zval *x;
	long zlimit = 0;
	int limit = -1;
#if !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long out;
	
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	php_stream *what;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &x, &limit) == FAILURE) {
		return;
	}

	if ((limit < 0) || (limit > 9)) {
		php_error(E_WARNING,"compression level must be within 0..9");
		RETURN_FALSE;
	}
#if !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif

	ZEND_FETCH_RESOURCE(what, php_stream *, &x, -1,"File-Handle",php_file_le_stream());
#if !defined(HAVE_NEW_MING)
	out = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, what);
	Ming_setSWFCompression(oldval);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	out = SWFMovie_output(movie, &phpStreamOutputMethod, what, limit);
#else
	out = SWFMovie_output(movie, &phpStreamOutputMethod, what);
#endif
	RETURN_LONG(out);
}
/* }}} */

/* {{{ proto int swfmovie::save(mixed where [, int compression])
  Saves the movie. 'where' can be stream and the movie will be saved there otherwise it is treated as string and written in file with that name */
PHP_METHOD(swfmovie, save)
{
	zval *x, *zlimit = NULL;
	int limit = -1;
#if !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long retval;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &x, &limit) == FAILURE) {
		return;
	}

#if !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
		  
	if (Z_TYPE_P(x) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(stream, php_stream *, &x, -1,"File-Handle",php_file_le_stream());
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
		RETURN_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream, limit));
#else
		RETVAL_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream));
#if !defined(HAVE_NEW_MING)
		Ming_setSWFCompression(oldval);
#endif
		return;
#endif
	}

	convert_to_string(x);
	stream = php_stream_open_wrapper(Z_STRVAL_P(x), "wb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}
	
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream, limit);
#else
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream);
#endif
	php_stream_close(stream);
#if!defined(HAVE_NEW_MING)
	Ming_setSWFCompression(oldval);
#endif
    
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto void swfmovie::setBackground(int r, int g, int b)
   Sets background color (r,g,b) */
PHP_METHOD(swfmovie, setBackground)
{
	long r, g, b;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &r, &g, &b) == FAILURE) {
		return;
	}

	SWFMovie_setBackground(movie, r, g, b);
}
/* }}} */

/* {{{ proto void swfmovie::setRate(float rate)
   Sets movie rate */
PHP_METHOD(swfmovie, setRate)
{
	double rate;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &rate) == FAILURE) {
		return;
	}

	SWFMovie_setRate(movie, rate);
}
/* }}} */

/* {{{ proto void swfmovie::setDimension(float x, float y)
   Sets movie dimension */
PHP_METHOD(swfmovie, setDimension)
{
	double x, y;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFMovie_setDimension(movie, x, y);
}
/* }}} */

/* {{{ proto void swfmovie::setFrames(int frames)
   Sets number of frames */
PHP_METHOD(swfmovie, setFrames)
{
	long frames;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &frames) == FAILURE) {
		return;
	}

	SWFMovie_setNumberOfFrames(movie, frames);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swfmovie::addMetadata(string xml)
 * */
PHP_METHOD(swfmovie, addMetadata)
{ 
	char *xml;
	long xml_len;
	
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
  
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xml, &xml_len) == FAILURE) {
		return;
	}

	SWFMovie_addMetadata(movie, xml);
}
/* }}} */

/* {{{ proto void swfmovie::setScriptLimits(int maxRecursion, int timeout)
*/
PHP_METHOD(swfmovie, setScriptLimits)
{
	long maxRecursion, timeout;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &maxRecursion, &timeout) == FAILURE) {
		return;
	}

	SWFMovie_setScriptLimits(movie, maxRecursion, timeout);  
}
/* }}} */

/* {{{ proto void swfmovie::setTabIndex(int depth, int index)
 * */
PHP_METHOD(swfmovie, setTabIndex)
{
	long depth, index;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &depth, &index) == FAILURE) {
		return;
	}

	SWFMovie_setTabIndex(movie, depth, index);
}
/* }}} */

/* {{{ proto void swfmovie::assignSymbol(SWFCharacter character, string name)
 * */
PHP_METHOD(swfmovie, assignSymbol)
{
	zval *zchar;
	char *name;
	long name_len;
	SWFCharacter character;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &zchar, &name, &name_len) == FAILURE) {
		return;
	}

	character = getCharacter(zchar TSRMLS_CC);
	SWFMovie_assignSymbol(movie, character, name);
}
/* }}} */

/* {{{ proto void swfmovie::defineScene(int offset, string name)
 * */
PHP_METHOD(swfmovie, defineScene)
{
	long offset;
	char *name;
	long name_len;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &offset, &name, &name_len) == FAILURE) {
		return;
	}

	SWFMovie_defineScene(movie, offset, name);
}
/* }}} */

/* {{{ proto void swfmovie::setNetworkAccess(int flag)
 * */  
PHP_METHOD(swfmovie, setNetworkAccess)
{   
	long flag;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {
		return;
	}

	SWFMovie_setNetworkAccess(movie, flag);
} 
/* }}} */

/* {{{ proto long swfmovie::streamMP3(mixed filename / SWFInput [, double skip])
   Sets sound stream of the SWF movie. The parameter can be stream or string. */
PHP_METHOD(swfmovie, streamMP3)
{
	zval *zfile;
	double skip = 0;
	SWFSoundStream sound;
	SWFInput input = NULL;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|d", &zfile, &skip) == FAILURE) {
		return;
	}

	switch(Z_TYPE_P(zfile))
	{
	case IS_RESOURCE:
		input = getInput_fromFileResource(zfile TSRMLS_CC);
		break;
	case IS_OBJECT:
		input = getInput(zfile TSRMLS_CC);
		break;	
	case IS_STRING:
		input = newSWFInput_filename(Z_STRVAL_P(zfile));
		if (input == NULL) {
			php_error(E_ERROR, "opening mp3 file failed");
		}

		zend_list_addref(zend_list_insert(input, le_swfinputp));
		break;

	default:
		php_error(E_ERROR, "swfmovie::streamMP3: need either a filename, "
		                   "a file ressource or SWFInput buffer.");
	}

	sound = newSWFSoundStream_fromInput(input);
	SWFMovie_setSoundStreamAt(movie, sound, skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / SWFMovie_getRate(movie));
}
/* }}} */

/* {{{ proto long swfmovie::setSoundStream
   Sets sound stream of the SWF movie. The parameter must be a SoundStream object */
PHP_METHOD(swfmovie, setSoundStream)
{
	zval *zstream;
	double skip = 0;
	SWFSoundStream sound;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|d", &zstream, &skip) == FAILURE) {
		return;
	}

	sound = getSoundStream(zstream TSRMLS_CC);	
	SWFMovie_setSoundStreamAt(movie, sound, skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / SWFMovie_getRate(movie));
}
/* }}} */

/* {{{ swfmovie_addexport */

PHP_METHOD(swfmovie, addExport)
{
	zval *zchar;
	char *name;
	long name_len;
	SWFBlock block;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &zchar, &name, &name_len) == FAILURE) {
		return;
	}

	block = (SWFBlock)getCharacter(zchar TSRMLS_CC);

	SWFMovie_addExport(movie, block, name);
}

/* }}} */

/* {{{ swfmovie_writeexports */
			
PHP_METHOD(swfmovie, writeExports)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	SWFMovie_writeExports(movie);
}

/* }}} */
/* {{{ SWFSoundInstance swfmovie_startsound */

PHP_METHOD(swfmovie, startSound)
{
	zval *zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zsound) == FAILURE) {
		return;
	}

	sound = (SWFSound)getSound(zsound TSRMLS_CC);

	item = SWFMovie_startSound(movie, sound);

	if(item != NULL)
	{
		/* try and create a soundinstance object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ void swfmovie::stopsound(sound) */
PHP_METHOD(swfmovie, stopSound)
{
	zval *zsound;
	SWFSound sound;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zsound) == FAILURE) {
		return;
	}

	sound = (SWFSound)getSound(zsound TSRMLS_CC);

	SWFMovie_stopSound(movie, sound);
}
/* }}} */

/* {{{ void swfmovie::importChar(filename, importname) */
PHP_METHOD(swfmovie, importChar)
{
	SWFMovie movie;
	SWFCharacter character;
	int ret;
	char *libswf, *name;
	long libswf_len, name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &libswf, &libswf_len, &name, &name_len) == FAILURE) {
		return;
	}

	movie = getMovie(getThis() TSRMLS_CC);
	character = SWFMovie_importCharacter(movie, libswf, name);
	if(character != NULL)
	{
		ret = zend_list_insert(character, le_swfcharacterp);
		object_init_ex(return_value, character_class_entry_ptr);
		add_property_resource(return_value, "character", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ void swfmovie_importFont */
PHP_METHOD(swfmovie, importFont)
{
	SWFMovie movie;
	SWFFontCharacter res;
	int ret;
	char *libswf, *name;
	long libswf_len, name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &libswf, &libswf_len, &name, &name_len) == FAILURE) {
		return;
	}

	movie = getMovie(getThis() TSRMLS_CC);
	PHP_MING_FILE_CHK(libswf);
	res = SWFMovie_importFont(movie, libswf, name);

	if(res != NULL)
	{
		/* try and create a fontchar object */
		ret = zend_list_insert(res, le_swffontcharp);
		object_init_ex(return_value, fontchar_class_entry_ptr);
		add_property_resource(return_value, "fontcharacter", ret);
		zend_list_addref(ret);
	}	
}
/* }}} */

/* {{{ void swfmovie_addFont */
PHP_METHOD(swfmovie, addFont)
{
	SWFMovie movie;
	SWFFontCharacter res;
	int ret;
	SWFFont font;
	zval *zfont;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zfont) == FAILURE) {
		return;
	}

	movie = getMovie(getThis() TSRMLS_CC);
	font = getFont(zfont TSRMLS_CC);
	res = SWFMovie_addFont(movie, font);
	if(res != NULL)
	{
		/* try and create a fontchar object */
		ret = zend_list_insert(res, le_swffontcharp);
		object_init_ex(return_value, fontchar_class_entry_ptr);
		add_property_resource(return_value, "fontcharacter", ret);
		zend_list_addref(ret);
	}	
}
/* }}} */

/* {{{ void swfmovie_replace */
PHP_METHOD(swfmovie, replace)
{
	SWFMovie movie;
	zval *zitem, **zblock;
	SWFDisplayItem item;
	SWFBlock block;
	SWFMovieBlockType ublock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oo", &zitem, &zblock) == FAILURE) {
		return;
	}

	item = getDisplayItem(zitem TSRMLS_CC);

	block = (SWFBlock) getCharacter(*zblock TSRMLS_CC);
	movie =  getMovie(getThis() TSRMLS_CC);
	ublock.block = block;

	SWFMovie_replace_internal(movie, item, ublock);
}
/* }}} */

/* {{{ float swfmovie::getRate() */
PHP_METHOD(swfmovie, getRate)
{
	RETURN_DOUBLE(SWFMovie_getRate(getMovie(getThis() TSRMLS_CC)));
}
/* }}} */
#endif

static zend_function_entry swfmovie_functions[] = {
	PHP_ME(swfmovie, __construct,       NULL, 0)
	PHP_ME(swfmovie, nextFrame,         NULL, 0)
	PHP_ME(swfmovie, labelFrame,        NULL, 0)
	PHP_ME(swfmovie, add,               NULL, 0)
	PHP_ME(swfmovie, remove,            NULL, 0)
	PHP_ME(swfmovie, output,            NULL, 0)
	PHP_ME(swfmovie, save,              NULL, 0)
	PHP_ME(swfmovie, saveToFile,        NULL, 0)
	PHP_ME(swfmovie, setBackground,     NULL, 0)
	PHP_ME(swfmovie, setRate,           NULL, 0)
	PHP_ME(swfmovie, setDimension,      NULL, 0)
	PHP_ME(swfmovie, setFrames,         NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfmovie, streamMP3,         NULL, 0)
	PHP_ME(swfmovie, setSoundStream,    NULL, 0)
	PHP_ME(swfmovie, addExport,         NULL, 0)
	PHP_ME(swfmovie, writeExports,      NULL, 0)
	PHP_ME(swfmovie, startSound,        NULL, 0)
	PHP_ME(swfmovie, stopSound,         NULL, 0)
	PHP_ME(swfmovie, importChar,        NULL, 0)
	PHP_ME(swfmovie, importFont,        NULL, 0)
	PHP_ME(swfmovie, addFont,           NULL, 0)
	PHP_ME(swfmovie, protect,           NULL, 0)
	PHP_ME(swfmovie, addMetadata,		NULL, 0)
	PHP_ME(swfmovie, setNetworkAccess,	NULL, 0)
	PHP_ME(swfmovie, setScriptLimits,	NULL, 0)
	PHP_ME(swfmovie, setTabIndex,		NULL, 0)
	PHP_ME(swfmovie, assignSymbol,		NULL, 0)
	PHP_ME(swfmovie, defineScene,		NULL, 0)
	PHP_ME(swfmovie, namedAnchor,		NULL, 0)
	PHP_ME(swfmovie, replace,		NULL, 0)
	PHP_ME(swfmovie, getRate,		NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFShape
*/
/* {{{ proto void swfshape::__construct()
   Creates a new SWFShape object */
PHP_METHOD(swfshape, __construct)
{
	SWFShape shape = newSWFShape();
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(getThis(), shape_class_entry_ptr);
	add_property_resource(getThis(), "shape", ret);
	zend_list_addref(ret);
}

static void destroy_SWFShape_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFShape((SWFShape)resource->ptr);
}
/* }}} */

/* {{{ internal function getShape
   Returns the SWFShape object contained in zval *id */
static SWFShape getShape(zval *id TSRMLS_DC)
{
	void *shape = SWFgetProperty(id, "shape", 5, le_swfshapep TSRMLS_CC);

	if (!shape) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFShape");
	}

	return (SWFShape)shape;
}
/* }}} */

/* {{{ proto void swfshape::setline(int width, int r, int g, int b [, int a])
   Sets the current line style for this SWFShape */
PHP_METHOD(swfshape, setLine)
{
	long w, r, g, b, a = 0xff;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll|l", &w, &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFShape_setLine(getShape(getThis() TSRMLS_CC),	(unsigned short)w, (byte)r, (byte)g, (byte)b, (byte)a);
}
/* }}} */

/* {{{ proto object swfshape::addfill(mixed arg1, int arg2, [int b [, int a]])
   Returns a fill object, for use with swfshape_setleftfill and swfshape_setrightfill. If 1 or 2 parameter(s) is (are) passed first should be object (from gradient class) and the second int (flags). Gradient fill is performed. If 3 or 4 parameters are passed : r, g, b [, a]. Solid fill is performed. */
PHP_METHOD(swfshape, addFill)
{
	SWFFill fill=NULL;
	int ret;

	if (ZEND_NUM_ARGS() == 1 || ZEND_NUM_ARGS() == 2) {
		/* it's a gradient or bitmap */
		zval *arg1;
		unsigned char flags = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|l", &arg1, &flags) == FAILURE) {
			return;
		}

		if (Z_OBJCE_P(arg1) == gradient_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_LINEAR_GRADIENT;
			}
			fill = SWFShape_addGradientFill(getShape(getThis() TSRMLS_CC), getGradient(arg1 TSRMLS_CC), flags);
		} else if (Z_OBJCE_P(arg1) == bitmap_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_TILED_BITMAP;
			}
			fill = SWFShape_addBitmapFill(getShape(getThis() TSRMLS_CC), getBitmap(arg1 TSRMLS_CC), flags);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Argument is not a bitmap nor a gradient");
		}

	} else if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		/* it's a solid fill */
		long r, g, b, a = 0xff;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
			return;
		}

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte)r, (byte)g, (byte)b, (byte)a);
	} else {
		WRONG_PARAM_COUNT;
	}
	
	if (!fill) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error adding fill to shape");
	}

	/* return an SWFFill object */
	ret = zend_list_insert(fill, le_swffillp);
	object_init_ex(return_value, fill_class_entry_ptr);
	add_property_resource(return_value, "fill", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto void swfshape::setleftfill(int arg1 [, int g ,int b [,int a]])
   Sets the left side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting left side fill type. */
PHP_METHOD(swfshape, setLeftFill)
{
	zval *zfill;
	long r, g, b, a = 0xff;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zfill) == FAILURE) {
			return;
		}
		fill = getFill(zfill TSRMLS_CC);
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
			return;
		}
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte)r, (byte)g, (byte)b, (byte)a);
	}

	SWFShape_setLeftFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::setleftfill(int arg1 [, int g ,int b [,int a]])
   Sets the right side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting right side fill type. */
PHP_METHOD(swfshape, setRightFill)
{
	zval *zfill;
	long r, g, b, a = 0xff;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zfill) == FAILURE) {
			return;
		}
		fill = getFill(zfill TSRMLS_CC);
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
			return;
		}
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte)r, (byte)g, (byte)b, (byte)a);
	}
	
	SWFShape_setRightFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::movepento(double x, double y)
   Moves the pen to shape coordinates (x, y) */
PHP_METHOD(swfshape, movePenTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_movePenTo(getShape(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfshape::movepen(double x, double y)
   Moves the pen from its current location by vector (x, y) */
PHP_METHOD(swfshape, movePen)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_movePen(getShape(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfshape::drawlineto(double x, double y)
   Draws a line from the current pen position to shape coordinates (x, y) in the current line style */
PHP_METHOD(swfshape, drawLineTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_drawLineTo(getShape(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfshape::drawline(double dx, double dy)
   Draws a line from the current pen position (x, y) to the point (x+dx, y+dy) in the current line style */
PHP_METHOD(swfshape, drawLine)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_drawLine(getShape(getThis() TSRMLS_CC), x, y);
}
/* }}} */

/* {{{ proto void swfshape::drawcurveto(double ax, double ay, double bx, double by [, double dx, double dy])
   Draws a curve from the current pen position (x,y) to the point (bx, by) in the current line style, using point (ax, ay) as a control point. Or draws a cubic bezier to point (dx, dy) with control points (ax, ay) and (bx, by) */
PHP_METHOD(swfshape, drawCurveTo)
{
	if (ZEND_NUM_ARGS() == 4) {
		double cx, cy, ax, ay;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddd", &cx, &cy, &ax, &ay) == FAILURE) {
			return;
		}

		SWFShape_drawCurveTo(getShape(getThis() TSRMLS_CC), cx, cy, ax, ay);	
	} else if (ZEND_NUM_ARGS() == 6) {
		double bx, by, cx, cy, dx, dy;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			return;
		}

		RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), bx, by, cx, cy, dx, dy));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawcurve(double adx, double ady, double bdx, double bdy [, double cdx, double cdy])
   Draws a curve from the current pen position (x, y) to the point (x+bdx, y+bdy) in the current line style, using point (x+adx, y+ady) as a control point or draws a cubic bezier to point (x+cdx, x+cdy) with control points (x+adx, y+ady) and (x+bdx, y+bdy) */
PHP_METHOD(swfshape, drawCurve)
{
	if (ZEND_NUM_ARGS() == 4) {
		double cx, cy, ax, ay;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddd", &cx, &cy, &ax, &ay) == FAILURE) {
			return;
		}

		SWFShape_drawCurve(getShape(getThis() TSRMLS_CC), cx, cy, ax, ay);	
	} else if (ZEND_NUM_ARGS() == 6) {
		double bx, by, cx, cy, dx, dy;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			return;
		}

		RETURN_LONG(SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), bx, by, cx, cy, dx, dy));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawglyph(SWFFont font, string character [, int size])
   Draws the first character in the given string into the shape using the glyph definition from the given font */
PHP_METHOD(swfshape, drawGlyph)
{
	zval *font;
	char *c;
	long c_len;
	int size = (int)(1024.0f/Ming_getScale());

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os|l", &font, &c, &c_len, &size) == FAILURE) {
		return;
	}
	SWFShape_drawSizedGlyph(getShape(getThis() TSRMLS_CC), getFont(font TSRMLS_CC), c[0], size);
}
/* }}} */

/* {{{ proto void swfshape::drawcircle(double r)
   Draws a circle of radius r centered at the current location, in a counter-clockwise fashion */
PHP_METHOD(swfshape, drawCircle)
{
	double r;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &r) == FAILURE) {
		return;
	}

	SWFShape_drawCircle(getShape(getThis() TSRMLS_CC), r);
}
/* }}} */

/* {{{ proto void swfshape::drawarc(double r, double startAngle, double endAngle)
   Draws an arc of radius r centered at the current location, from angle startAngle to angle endAngle measured clockwise from 12 o'clock */
PHP_METHOD(swfshape, drawArc)
{
	double r, start, end;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ddd", &r, &start, &end) == FAILURE) {
		return;
	}

	/* convert angles to radians, since that's what php uses elsewhere */
	SWFShape_drawArc(getShape(getThis() TSRMLS_CC), r, start, end);
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(double bx, double by, double cx, double cy, double dx, double dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubic)
{
	double bx, by, cx, cy, dx, dy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		return;
	}

	RETURN_LONG(SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), bx, by, cx, cy, dx, dy));
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(double bx, double by, double cx, double cy, double dx, double dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubicTo)
{
	double bx, by, cx, cy, dx, dy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		return;
	}

	RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), bx, by, cx, cy, dx, dy));
}
/* }}} */

/* {{{ proto void swfshape::end() */
PHP_METHOD(swfshape, end)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFShape_end(getShape(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::useVersion() 
 * SWF_SHAPE3
 * SWF_SHAPE4
 */
PHP_METHOD(swfshape, useVersion)
{
	long ver;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ver) == FAILURE) {
		return;
	}

	SWFShape_useVersion(getShape(getThis() TSRMLS_CC), ver);
}
/* }}} */

/* {{{ proto int swfshape::getVersion() */
PHP_METHOD(swfshape, getVersion)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	
	RETURN_LONG(SWFShape_getVersion(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto void swfshape::setRenderHintingFlags(flags) 
 * SWF_SHAPE_USESCALINGSTROKES
 * SWF_SHAPE_USENONSCALINGSTROKES
 */
PHP_METHOD(swfshape, setRenderHintingFlags)
{
	long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	SWFShape_setRenderHintingFlags(getShape(getThis() TSRMLS_CC), flags);
}
/* }}} */

/* {{{ proto double swfshape::getPenX() */
PHP_METHOD(swfshape, getPenX)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_DOUBLE(SWFShape_getPenX(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfshape::getPenY() */
PHP_METHOD(swfshape, getPenY)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_DOUBLE(SWFShape_getPenY(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto void swfshape::hideLine() */
PHP_METHOD(swfshape, hideLine)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFShape_hideLine(getShape(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::drawCharacterBounds(character) */
PHP_METHOD(swfshape, drawCharacterBounds)
{
	zval *character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &character) == FAILURE) {
		return;
	}

	SWFShape_drawCharacterBounds(getShape(getThis() TSRMLS_CC),
		getCharacter(character TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::setLine2(width, flags, miterLimit, r, g, b, [a]) 
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 * */
PHP_METHOD(swfshape, setLine2)
{
	long width, flags, r, g, b, a = 0xff;
	double limit;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lldlll|l", &width, &flags, &limit, &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFShape_setLine2(getShape(getThis() TSRMLS_CC), width, r, g, b, a, flags, limit);
}
/* }}} */

/* {{{ proto void swfshape::setLine2Filled(width, fill, flags, limit) 
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
PHP_METHOD(swfshape, setLine2Filled)
{
	long width, flags;
	zval *fill;
	double limit;
	SWFFill xfill;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lold", &width, &fill, &flags, &limit) == FAILURE) {
		return;
	}

	xfill = getFill(fill TSRMLS_CC);
	SWFShape_setLine2Filled(getShape(getThis() TSRMLS_CC), width, SWFFill_getFillStyle(xfill), flags, limit);
}
/* }}} */

static zend_function_entry swfshape_functions[] = {
	PHP_ME(swfshape, __construct,        NULL, 0)
	PHP_ME(swfshape, setLine,            NULL, 0)
	PHP_ME(swfshape, addFill,            NULL, 0)
	PHP_ME(swfshape, setLeftFill,        NULL, 0)
	PHP_ME(swfshape, setRightFill,       NULL, 0)
	PHP_ME(swfshape, movePenTo,          NULL, 0)
	PHP_ME(swfshape, movePen,            NULL, 0)
	PHP_ME(swfshape, drawLineTo,         NULL, 0)
	PHP_ME(swfshape, drawLine,           NULL, 0)
	PHP_ME(swfshape, drawCurveTo,        NULL, 0)
	PHP_ME(swfshape, drawCurve,          NULL, 0)
	PHP_ME(swfshape, drawGlyph,          NULL, 0)
	PHP_ME(swfshape, drawCircle,         NULL, 0)
	PHP_ME(swfshape, drawArc,            NULL, 0)
	PHP_ME(swfshape, drawCubic,          NULL, 0)
	PHP_ME(swfshape, drawCubicTo,        NULL, 0)
	PHP_ME(swfshape, end,                NULL, 0)
	PHP_ME(swfshape, useVersion,         NULL, 0)
	PHP_ME(swfshape, setRenderHintingFlags, NULL, 0)
	PHP_ME(swfshape, getPenX,            NULL, 0)
	PHP_ME(swfshape, getPenY,            NULL, 0)
	PHP_ME(swfshape, hideLine,           NULL, 0)
	PHP_ME(swfshape, drawCharacterBounds, NULL, 0)
	PHP_ME(swfshape, setLine2,           NULL, 0)
	PHP_ME(swfshape, setLine2Filled,     NULL, 0)	
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSprite
*/
/* {{{ proto void swfsprite::__construct()
   Creates a new SWFSprite object */
PHP_METHOD(swfsprite, __construct)
{
	SWFMovieClip sprite = newSWFMovieClip();
	int ret = zend_list_insert(sprite, le_swfspritep);

	object_init_ex(getThis(), sprite_class_entry_ptr);
	add_property_resource(getThis(), "sprite", ret);
	zend_list_addref(ret);
}

static void destroy_SWFSprite_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMovieClip((SWFMovieClip)resource->ptr);
}
/* }}} */

/* {{{ internal function SWFSprite getSprite(zval *id)
   Returns the SWFSprite object in zval *id */
static SWFMovieClip getSprite(zval *id TSRMLS_DC)
{
	void *sprite = SWFgetProperty(id, "sprite", 6, le_swfspritep TSRMLS_CC);

	if (!sprite) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFSprite");
	}
	return (SWFMovieClip)sprite;
}
/* }}} */

/* {{{ proto object swfsprite::add(object SWFCharacter)
   Adds the character to the sprite, returns a displayitem object */
PHP_METHOD(swfsprite, add)
{
	zval *zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}

	if (Z_OBJCE_P(zchar) == action_class_entry_ptr) {
		block = (SWFBlock)getAction(zchar TSRMLS_CC);
	} else {
		block = (SWFBlock)getCharacter(zchar TSRMLS_CC);
	}

	item = SWFMovieClip_add(sprite, block);

	if (item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfdisplayitemp);
		object_init_ex(return_value, displayitem_class_entry_ptr);
		add_property_resource(return_value, "displayitem", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfsprite::remove(object SWFDisplayItem)
   Remove the named character from the sprite's display list */
PHP_METHOD(swfsprite, remove)
{
	zval *zchar;
	SWFDisplayItem item;
	SWFMovieClip movie = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}

	item = getDisplayItem(zchar TSRMLS_CC);
	SWFMovieClip_remove(movie, item);
}
/* }}} */

/* {{{ proto void swfsprite::nextFrame()
   Moves the sprite to the next frame */
PHP_METHOD(swfsprite, nextFrame)
{
	SWFMovieClip_nextFrame(getSprite(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfsprite::labelFrame(string label)
   Labels frame */
PHP_METHOD(swfsprite, labelFrame)
{
	char *label;
	long label_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &label, &label_len) == FAILURE) {
		return;
	}

	SWFMovieClip_labelFrame(getSprite(getThis() TSRMLS_CC), label);
}
/* }}} */

/* {{{ proto void swfsprite::setFrames(int frames)
   Sets the number of frames in this SWFSprite */
PHP_METHOD(swfsprite, setFrames)
{
	long frames;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &frames) == FAILURE) {
		return;
	}

	SWFMovieClip_setNumberOfFrames(sprite, frames);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ SWFSoundInstance swfsprite_startsound */

PHP_METHOD(swfsprite, startSound)
{
	zval *zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zsound) == FAILURE) {
		return;
	}
	sound = (SWFSound)getSound(zsound TSRMLS_CC);

	item = SWFMovieClip_startSound(sprite, sound);

	if(item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}

/* }}} */
/* {{{ void swfsprite_stopsound */

PHP_METHOD(swfsprite, stopSound)
{
	zval *zsound;
	SWFSound sound;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zsound) == FAILURE) {
		return;
	}
	sound = (SWFSound)getSound(zsound TSRMLS_CC);

	SWFMovieClip_stopSound(sprite, sound);
}
/* }}} */

/* {{{ proto long swfmovieclip::setSoundStream(mixed file, rate[, skip])
   Sets sound stream of the SWF movieClip. The parameter can be stream or string. */
PHP_METHOD(swfsprite, setSoundStream)
{
	zval *zfile;
	double rate, skip = 0;
	SWFSoundStream sound;
	SWFInput input = NULL;
	SWFMovieClip mc = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zdd", &zfile, &rate, &skip) == FAILURE) {
		return;
	}

	switch(Z_TYPE_P(zfile))
	{
	case IS_RESOURCE:
		input = getInput_fromFileResource(zfile TSRMLS_CC);
		break;
	case IS_OBJECT:
		input = getInput(zfile TSRMLS_CC);
		break;	
	case IS_STRING:
		input = newSWFInput_filename(Z_STRVAL_P(zfile));
		if(input == NULL)
			php_error(E_ERROR, "opening sound file failed");
		zend_list_addref(zend_list_insert(input, le_swfinputp));
		break;

	default:
		php_error(E_ERROR, "swfmovieclip::setSoundStream: need either a filename, "
		                   "a file ressource or SWFInput buffer.");
	}
	sound = newSWFSoundStream_fromInput(input);
	SWFMovieClip_setSoundStreamAt(mc, sound, rate, skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / rate);
}
/* }}} */


/* {{{ proto void swfsprite::setScalingGrid(int x, int y, int w, int h) */
PHP_METHOD(swfsprite, setScalingGrid)
{
	long zx, zy, zw, zh;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &zx, &zy, &zw, &zh) == FAILURE) {
		return;
	}
	
	SWFMovieClip_setScalingGrid(sprite, zx, zy, zw, zh);
}
/* }}} */

/* {{{ proto void swfsprite::removeScalingGrid() */
PHP_METHOD(swfsprite, removeScalingGrid)
{
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	SWFMovieClip_removeScalingGrid(sprite);
}
/* }}} */

/* {{{ proto void swfsprite::addInitAction(action) */
PHP_METHOD(swfsprite, addInitAction)
{
	zval *zaction;

	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);
	SWFAction action;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zaction) == FAILURE) {
		return;
	}

	action = getAction(zaction TSRMLS_CC);
	SWFMovieClip_addInitAction(sprite, action);
}
/* }}} */
#endif

// workaround to support SWFSprite and SWFMovieclip objects
static zend_function_entry swfmovieclip_functions[] = {
	PHP_ME(swfsprite, __construct,  	NULL, 0)
	PHP_ME(swfsprite, add,			NULL, 0)
	PHP_ME(swfsprite, remove,		NULL, 0)
	PHP_ME(swfsprite, nextFrame,		NULL, 0)
	PHP_ME(swfsprite, labelFrame,		NULL, 0)
	PHP_ME(swfsprite, setFrames,		NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfsprite, startSound,		NULL, 0)
	PHP_ME(swfsprite, stopSound,		NULL, 0)
	PHP_ME(swfsprite, setScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, removeScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, setSoundStream, 	NULL, 0)
	PHP_ME(swfsprite, addInitAction,	NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

static zend_function_entry swfsprite_functions[] = {
	PHP_ME(swfsprite, __construct,  	NULL, 0)
	PHP_ME(swfsprite, add,			NULL, 0)
	PHP_ME(swfsprite, remove,		NULL, 0)
	PHP_ME(swfsprite, nextFrame,		NULL, 0)
	PHP_ME(swfsprite, labelFrame,		NULL, 0)
	PHP_ME(swfsprite, setFrames,		NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfsprite, startSound,		NULL, 0)
	PHP_ME(swfsprite, stopSound,		NULL, 0)
	PHP_ME(swfsprite, setScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, removeScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, setSoundStream, 	NULL, 0)
	PHP_ME(swfsprite, addInitAction,	NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFText
*/
/* {{{ proto void swftext::__construct([version])
   Creates new SWFText object */
PHP_METHOD(swftext, __construct)
{
	long version = 0;
	SWFText text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &version) == FAILURE) {
		return;
	}
	if (version == 1) {
		text = newSWFText();
	}
	else {
		text = newSWFText2();
	}

	int ret = zend_list_insert(text, le_swftextp);
	object_init_ex(getThis(), text_class_entry_ptr);
	add_property_resource(getThis(), "text", ret);
	zend_list_addref(ret);
}

static void destroy_SWFText_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFText((SWFText)resource->ptr);
}
/* }}} */

/* {{{ internal function SWFText getText(zval *id)
   Returns the SWFText contained in zval *id */
static SWFText getText(zval *id TSRMLS_DC)
{
	void *text = SWFgetProperty(id, "text", 4, le_swftextp TSRMLS_CC);

	if (!text) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFText");
	}
	return (SWFText)text;
}
/* }}} */

/* {{{ proto void swftext::setFont(SWFFont font)
   Sets this SWFText object's current font to given font */
PHP_METHOD(swftext, setFont)
{
	zval *zfont;
	SWFText text = getText(getThis() TSRMLS_CC);
	SWFFont font;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zfont) == FAILURE) {
		return;
	}

	font = getFont(zfont TSRMLS_CC);
	SWFText_setFont(text, font);
}
/* }}} */

/* {{{ proto void swftext::setHeight(float height)
   Sets this SWFText object's current height to given height */
PHP_METHOD(swftext, setHeight)
{
	double height;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &height) == FAILURE) {
		return;
	}

	SWFText_setHeight(text, height);
}
/* }}} */

/* {{{ proto void swftext::setSpacing(float spacing)
   Sets this SWFText object's current letterspacing to given spacing */
PHP_METHOD(swftext, setSpacing)
{
	double spacing;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &spacing) == FAILURE) {
		return;
	}

	SWFText_setSpacing(text, spacing);
}
/* }}} */

/* {{{ proto void swftext::setColor(int r, int g, int b [, int a])
   Sets this SWFText object's current color to the given color */
PHP_METHOD(swftext, setColor)
{
	long r, g, b, a = 0xff;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
		return;
	}

	SWFText_setColor(text, (byte)r, (byte)g, (byte)b, (byte)a);
}
/* }}} */

/* {{{ proto void swftext::moveTo(float x, float y)
   Moves this SWFText object's current pen position to (x, y) in text coordinates */
PHP_METHOD(swftext, moveTo)
{
	double x, y;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFText_setXY(text, x, y);
}
/* }}} */

/* {{{ proto void swftext::addString(string text)
   Writes the given text into this SWFText object at the current pen position, using the current font, height, spacing, and color */
PHP_METHOD(swftext, addString)
{
	char *s;
	long s_len;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s, &s_len) == FAILURE) {
		return;
	}

	SWFText_addString(text, s, NULL);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swftext::addUTF8String(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */

PHP_METHOD(swftext, addUTF8String)
{
	char *s;
	long s_len;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s, &s_len) == FAILURE) {
		return;
	}

	SWFText_addUTF8String(text, s, NULL);
}

/* }}} */
/* {{{ proto void swftext::addWideString(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */
/*
PHP_METHOD(swftext, addWideString)
{
	char *s;
	long s_len;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s, &s_len) == FAILURE) {
		return;
	}

	SWFText_addWideString(text, s, NULL);
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getWidth(string str)
   Calculates the width of the given string in this text objects current font and size */
PHP_METHOD(swftext, getWidth)
{
	char *string;
	long string_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	RETURN_DOUBLE(SWFText_getStringWidth(getText(getThis() TSRMLS_CC), string));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto double swftext::getUTF8Width(string)
   calculates the width of the given string in this text objects current font and size */

PHP_METHOD(swftext, getUTF8Width)
{
	char *string;
	long string_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	RETURN_DOUBLE(SWFText_getUTF8StringWidth(getText(getThis() TSRMLS_CC), string));
}

/* }}} */
/* {{{ proto double swftext::getWideWidth(string)
   calculates the width of the given string in this text objects current font and size */
/*
PHP_METHOD(swftext, getWideWidth)
{
	char *string;
	long string_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}
	
	RETURN_DOUBLE(SWFText_getWideStringWidth(getText(getThis() TSRMLS_CC), string));
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getAscent()
   Returns the ascent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getAscent)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFText_getAscent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getDescent()
   Returns the descent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getDescent)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFText_getDescent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getLeading()
   Returns the leading of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getLeading)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFText_getLeading(getText(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swftext_functions[] = {
	PHP_ME(swftext, __construct,       NULL, 0)
	PHP_ME(swftext, setFont,           NULL, 0)
	PHP_ME(swftext, setHeight,         NULL, 0)
	PHP_ME(swftext, setSpacing,        NULL, 0)
	PHP_ME(swftext, setColor,          NULL, 0)
	PHP_ME(swftext, moveTo,            NULL, 0)
	PHP_ME(swftext, addString,         NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftext, addUTF8String,     NULL, 0)
/*	PHP_ME(swftext, addwidestring,     NULL, 0)*/
#endif
	PHP_ME(swftext, getWidth,          NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftext, getUTF8Width,      NULL, 0)
/*	PHP_ME(swftext, getwidewidth,      NULL, 0)*/
#endif
	PHP_ME(swftext, getAscent,         NULL, 0)
	PHP_ME(swftext, getDescent,        NULL, 0)
	PHP_ME(swftext, getLeading,        NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFTextField
*/
/* {{{ proto void swftextfield::__construct([int flags])
   Creates a new SWFTextField object */
PHP_METHOD(swftextfield, __construct)
{
	long flags;
	SWFTextField field = newSWFTextField();
	int ret = zend_list_insert(field, le_swftextfieldp);

	object_init_ex(getThis(), textfield_class_entry_ptr);
	add_property_resource(getThis(), "textfield", ret);
	zend_list_addref(ret);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	SWFTextField_setFlags(field, flags);
}
/* }}} */

static void destroy_SWFTextField_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFTextField((SWFTextField)resource->ptr);
}

/* {{{ internal function getTextField
   Returns the SWFTextField object contained in zval *id */
static SWFTextField getTextField(zval *id TSRMLS_DC)
{
	void *field = SWFgetProperty(id, "textfield", 9, le_swftextfieldp TSRMLS_CC);

	if (!field) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFTextField");
	}
	return (SWFTextField)field;
}
/* }}} */

/* {{{ proto void swftextfield::setFont(object font)
   Sets the font for this textfield */
static
SWFBlock getFontOrFontChar(zval *id TSRMLS_DC)
{
	if(Z_OBJCE_P(id) == font_class_entry_ptr) {
		return (SWFBlock)getFont(id TSRMLS_CC);
	} else if(Z_OBJCE_P(id) == fontchar_class_entry_ptr) {
		return (SWFBlock)getFontCharacter(id TSRMLS_CC);
	} else {
		php_error(E_ERROR, "called object is not an SWFFont or SWFFontCharacter");
	}
	return NULL;
}

PHP_METHOD(swftextfield, setFont)
{
	zval *font;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &font) == FAILURE) {
		return;
	}

	if(Z_OBJCE_P(font) == font_class_entry_ptr 
		|| Z_OBJCE_P(font) == fontchar_class_entry_ptr)
	{
		SWFTextField_setFont(field, getFontOrFontChar(font TSRMLS_CC));
	}
	else if(Z_OBJCE_P(font) == browserfont_class_entry_ptr)
	{
		SWFTextField_setFont(field, (SWFBlock)getBrowserFont(font TSRMLS_CC));
	}
	else {
		php_error(E_ERROR, "not a font object\n");
	}		
}
/* }}} */

/* {{{ proto void swftextfield::setBounds(float width, float height)
   Sets the width and height of this textfield */
PHP_METHOD(swftextfield, setBounds)
{
	double width, height;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &width, &height) == FAILURE) {
		return;
	}

	SWFTextField_setBounds(getTextField(getThis() TSRMLS_CC), width, height);
}
/* }}} */

/* {{{ proto void swftextfield::align(int alignment)
   Sets the alignment of this textfield */
PHP_METHOD(swftextfield, align)
{
	long align;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &align) == FAILURE) {
		return;
	}

	SWFTextField_setAlignment(getTextField(getThis() TSRMLS_CC), align);
}
/* }}} */

/* {{{ proto void swftextfield::setHeight(float height)
   Sets the font height of this textfield */
PHP_METHOD(swftextfield, setHeight)
{
	double height;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &height) == FAILURE) {
		return;
	}

	SWFTextField_setHeight(getTextField(getThis() TSRMLS_CC), height);
}
/* }}} */

/* {{{ proto void swftextfield::setLeftMargin(float margin)
   Sets the left margin of this textfield */
PHP_METHOD(swftextfield, setLeftMargin)
{
	double margin;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &margin) == FAILURE) {
		return;
	}

	SWFTextField_setLeftMargin(getTextField(getThis() TSRMLS_CC), margin);
}
/* }}} */

/* {{{ proto void swftextfield::setRightMargin(float margin)
   Sets the right margin of this textfield */
PHP_METHOD(swftextfield, setRightMargin)
{
	double margin;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &margin) == FAILURE) {
		return;
	}

	SWFTextField_setRightMargin(getTextField(getThis() TSRMLS_CC), margin);
}
/* }}} */

/* {{{ proto void swftextfield::setMargins(float left, float right)
   Sets both margins of this textfield */
PHP_METHOD(swftextfield, setMargins)
{
	double left, right;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &left, &right) == FAILURE) {
		return;
	}

	SWFTextField_setLeftMargin(field, left);
	SWFTextField_setRightMargin(field, right);
}
/* }}} */

/* {{{ proto void swftextfield::setIndentation(float indentation)
   Sets the indentation of the first line of this textfield */
PHP_METHOD(swftextfield, setIndentation)
{
	double indent;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &indent) == FAILURE) {
		return;
	}

	SWFTextField_setIndentation(getTextField(getThis() TSRMLS_CC), indent);
}
/* }}} */

/* {{{ proto void swftextfield::setLineSpacing(float space)
   Sets the line spacing of this textfield */
PHP_METHOD(swftextfield, setLineSpacing)
{
	double spacing;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &spacing) == FAILURE) {
		return;
	}

	SWFTextField_setLineSpacing(getTextField(getThis() TSRMLS_CC), spacing);
}
/* }}} */

/* {{{ proto void swftextfield::setColor(int r, int g, int b [, int a])
   Sets the color of this textfield */
PHP_METHOD(swftextfield, setColor)
{
	long r, g, b, a = 0xff;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
		return;
	}
	SWFTextField_setColor(getTextField(getThis() TSRMLS_CC), (byte)r, (byte)g, (byte)b, (byte)a);
}
/* }}} */

/* {{{ proto void swftextfield::setName(string var_name)
   Sets the variable name of this textfield */
PHP_METHOD(swftextfield, setName)
{
	char *name;
	long name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	SWFTextField_setVariableName(getTextField(getThis() TSRMLS_CC), name);
}
/* }}} */

/* {{{ proto void swftextfield::addString(string str)
   Adds the given string to this textfield */
PHP_METHOD(swftextfield, addString)
{
	char *str;
	long str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	SWFTextField_addString(getTextField(getThis() TSRMLS_CC), str);
}
/* }}} */

/* {{{ proto void swftextfield::addUTF8String(string str)
   Adds the given UTF8 string to this textfield */
PHP_METHOD(swftextfield, addUTF8String)
{
	char *str;
	long str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	SWFTextField_addUTF8String(getTextField(getThis() TSRMLS_CC), str);
}
/* }}} */


#ifdef HAVE_NEW_MING
/* {{{ proto void swftextfield::setPadding(float padding)
   Sets the padding of this textfield */
PHP_METHOD(swftextfield, setPadding)
{
	double padding;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &padding) == FAILURE) {
		return;
	}

	SWFTextField_setPadding(getTextField(getThis() TSRMLS_CC), padding);
}
/* }}} */

/* {{{ proto void swftextfield::addChars(string)
   adds characters to a font that will be available within a textfield */
PHP_METHOD(swftextfield, addChars)
{
	char *str;
	long str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	SWFTextField_addChars(getTextField(getThis() TSRMLS_CC), str);

}
/* }}} */

/* {{{ proto void swftextfield::setLength(int len) */
PHP_METHOD(swftextfield, setLength)
{
	long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &len) == FAILURE) {
		return;
	}

	SWFTextField_setLength(getTextField(getThis() TSRMLS_CC), len);
}
/* }}} */

/* {{{ proto void swftextfield::setLength(len) */
PHP_METHOD(swftextfield, setFieldHeight)
{
	long h;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &h) == FAILURE) {
		return;
	}

	SWFTextField_setFieldHeight(getTextField(getThis() TSRMLS_CC), h);
}
/* }}} */

#endif

static zend_function_entry swftextfield_functions[] = {
	PHP_ME(swftextfield, __construct,     NULL, 0)
	PHP_ME(swftextfield, setFont,         NULL, 0)
	PHP_ME(swftextfield, setBounds,       NULL, 0)
	PHP_ME(swftextfield, align,           NULL, 0)
	PHP_ME(swftextfield, setHeight,       NULL, 0)
	PHP_ME(swftextfield, setLeftMargin,   NULL, 0)
	PHP_ME(swftextfield, setRightMargin,  NULL, 0)
	PHP_ME(swftextfield, setMargins,      NULL, 0)
	PHP_ME(swftextfield, setIndentation,  NULL, 0)
	PHP_ME(swftextfield, setLineSpacing,  NULL, 0)
	PHP_ME(swftextfield, setColor,        NULL, 0)
	PHP_ME(swftextfield, setName,         NULL, 0)
	PHP_ME(swftextfield, addString,       NULL, 0)
	PHP_ME(swftextfield, addUTF8String,   NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftextfield, setPadding,      NULL, 0)
	PHP_ME(swftextfield, addChars,        NULL, 0)
	PHP_ME(swftextfield, setLength,	      NULL, 0)
	PHP_ME(swftextfield, setFieldHeight,  NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

zend_module_entry ming_module_entry = {
	STANDARD_MODULE_HEADER,
	"ming",
	ming_functions,
	PHP_MINIT(ming), /* module init function */
	NULL,            /* module shutdown function */
	PHP_RINIT(ming), /* request init function */
	NULL,            /* request shutdown function */
	PHP_MINFO(ming), /* module info function */
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MING
ZEND_GET_MODULE(ming)
#endif

/* {{{ todo PHP_MINFO_FUNCTION(ming)
*/
PHP_MINFO_FUNCTION(ming)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Ming SWF output library", "enabled");
	php_info_print_table_row(2, "Version", MING_VERSION_TEXT);
	php_info_print_table_end();
}
/* }}} */

#if PHP_API_VERSION == 20020918
static php4_fix_funcnames(char *class_name, zend_function_entry *funcs)
{
	zend_function_entry *pf = funcs;
	char *pname;

	while(funcs->fname) {
		if( strcmp(funcs->fname,"__construct") == 0 ) {
			pname=strdup(class_name);
		} else {
			pname=strdup(funcs->fname);
		}
		funcs->fname=pname;
		while(*pname) { *pname=tolower(*pname);pname++;}
		funcs++;
	}
}
#endif

/* custom error handler propagates ming errors up to php */
static void php_ming_error(const char *msg, ...)
{
	va_list args;
	char *buffer;
	
	TSRMLS_FETCH();

	va_start(args, msg);
	vspprintf(&buffer, 0, msg, args);
	va_end(args);

	php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", buffer);
	efree(buffer);
}

PHP_RINIT_FUNCTION(ming)
{
	/* XXX - this didn't work so well last I tried.. */

	if (Ming_init() != 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error initializing Ming module");
		return FAILURE;
	}
	return SUCCESS;
}

/* {{{ todo PHP_MINIT_FUNCTION(ming)
*/
PHP_MINIT_FUNCTION(ming)
{
	zend_class_entry shape_class_entry;
	zend_class_entry fill_class_entry;
	zend_class_entry gradient_class_entry;
	zend_class_entry bitmap_class_entry;
	zend_class_entry text_class_entry;
	zend_class_entry textfield_class_entry;
	zend_class_entry font_class_entry;
	zend_class_entry displayitem_class_entry;
	zend_class_entry movie_class_entry;
	zend_class_entry button_class_entry;
	zend_class_entry action_class_entry;
	zend_class_entry morph_class_entry;
	zend_class_entry movieclip_class_entry;
	zend_class_entry sprite_class_entry;
	zend_class_entry sound_class_entry;
#ifdef HAVE_NEW_MING
	zend_class_entry fontchar_class_entry;
	zend_class_entry soundinstance_class_entry;
	zend_class_entry videostream_class_entry;
	zend_class_entry buttonrecord_class_entry;
	zend_class_entry binarydata_class_entry;
	zend_class_entry initaction_class_entry;
	zend_class_entry prebuiltclip_class_entry;
	zend_class_entry soundstream_class_entry;
	zend_class_entry filter_class_entry;
	zend_class_entry filtermatrix_class_entry;
	zend_class_entry blur_class_entry;
	zend_class_entry shadow_class_entry;
	zend_class_entry cxform_class_entry;
	zend_class_entry matrix_class_entry;
	zend_class_entry input_class_entry;
	zend_class_entry browserfont_class_entry;
	zend_class_entry fontcollection_class_entry;
#endif
	zend_class_entry character_class_entry;
	Ming_setErrorFunction((void *) php_ming_error);

#define CONSTANT(s,c) REGISTER_LONG_CONSTANT((s), (c), CONST_CS | CONST_PERSISTENT)

#ifdef HAVE_NEW_MING
	CONSTANT("MING_NEW",                    1);
#else
	CONSTANT("MING_NEW",                    0);
#endif
#ifdef HAVE_MING_ZLIB
	CONSTANT("MING_ZLIB",                   1);
#else
	CONSTANT("MING_ZLIB",                   0);
#endif
	/* flags for SWFButton_addShape */
	CONSTANT("SWFBUTTON_HIT",               SWFBUTTONRECORD_HITSTATE);
	CONSTANT("SWFBUTTON_DOWN",              SWFBUTTONRECORD_DOWNSTATE);
	CONSTANT("SWFBUTTON_OVER",              SWFBUTTONRECORD_OVERSTATE);
	CONSTANT("SWFBUTTON_UP",                SWFBUTTONRECORD_UPSTATE);

	/* flags for SWFButton_addAction */
	CONSTANT("SWFBUTTON_MOUSEUPOUTSIDE",    SWFBUTTON_MOUSEUPOUTSIDE);
	CONSTANT("SWFBUTTON_DRAGOVER",          SWFBUTTON_DRAGOVER);
	CONSTANT("SWFBUTTON_DRAGOUT",           SWFBUTTON_DRAGOUT);
	CONSTANT("SWFBUTTON_MOUSEUP",           SWFBUTTON_MOUSEUP);
	CONSTANT("SWFBUTTON_MOUSEDOWN",         SWFBUTTON_MOUSEDOWN);
	CONSTANT("SWFBUTTON_MOUSEOUT",          SWFBUTTON_MOUSEOUT);
	CONSTANT("SWFBUTTON_MOUSEOVER",         SWFBUTTON_MOUSEOVER);

	/* flags for SWFFill */
	CONSTANT("SWFFILL_RADIAL_GRADIENT",     SWFFILL_RADIAL_GRADIENT);
	CONSTANT("SWFFILL_LINEAR_GRADIENT",     SWFFILL_LINEAR_GRADIENT);
	CONSTANT("SWFFILL_TILED_BITMAP",        SWFFILL_TILED_BITMAP);
	CONSTANT("SWFFILL_CLIPPED_BITMAP",      SWFFILL_CLIPPED_BITMAP);

	/* flags for SWFTextField_init */
	CONSTANT("SWFTEXTFIELD_NOEDIT",         SWFTEXTFIELD_NOEDIT);
	CONSTANT("SWFTEXTFIELD_PASSWORD",       SWFTEXTFIELD_PASSWORD);
	CONSTANT("SWFTEXTFIELD_MULTILINE",      SWFTEXTFIELD_MULTILINE);
	CONSTANT("SWFTEXTFIELD_WORDWRAP",       SWFTEXTFIELD_WORDWRAP);
	CONSTANT("SWFTEXTFIELD_DRAWBOX",        SWFTEXTFIELD_DRAWBOX);
	CONSTANT("SWFTEXTFIELD_NOSELECT",       SWFTEXTFIELD_NOSELECT);
	CONSTANT("SWFTEXTFIELD_HTML",           SWFTEXTFIELD_HTML);
	CONSTANT("SWFTEXTFIELD_AUTOSIZE",       SWFTEXTFIELD_AUTOSIZE);

	/* flags for SWFTextField_align */
	CONSTANT("SWFTEXTFIELD_ALIGN_LEFT",     SWFTEXTFIELD_ALIGN_LEFT);
	CONSTANT("SWFTEXTFIELD_ALIGN_RIGHT",    SWFTEXTFIELD_ALIGN_RIGHT);
	CONSTANT("SWFTEXTFIELD_ALIGN_CENTER",   SWFTEXTFIELD_ALIGN_CENTER);
	CONSTANT("SWFTEXTFIELD_ALIGN_JUSTIFY",  SWFTEXTFIELD_ALIGN_JUSTIFY);

	/* flags for SWFDisplayItem_addAction */
	CONSTANT("SWFACTION_ONLOAD",            SWFACTION_ONLOAD);
	CONSTANT("SWFACTION_ENTERFRAME",        SWFACTION_ENTERFRAME);
	CONSTANT("SWFACTION_UNLOAD",            SWFACTION_UNLOAD);
	CONSTANT("SWFACTION_MOUSEMOVE",         SWFACTION_MOUSEMOVE);
	CONSTANT("SWFACTION_MOUSEDOWN",         SWFACTION_MOUSEDOWN);
	CONSTANT("SWFACTION_MOUSEUP",           SWFACTION_MOUSEUP);
	CONSTANT("SWFACTION_KEYDOWN",           SWFACTION_KEYDOWN);
	CONSTANT("SWFACTION_KEYUP",             SWFACTION_KEYUP);
	CONSTANT("SWFACTION_DATA",              SWFACTION_DATA);
	CONSTANT("SWFACTION_INIT",              SWFACTION_INIT);
	CONSTANT("SWFACTION_PRESS",             SWFACTION_PRESS);
	CONSTANT("SWFACTION_RELEASE",           SWFACTION_RELEASE);
	CONSTANT("SWFACTION_RELEASEOUTSIDE",    SWFACTION_RELEASEOUTSIDE);
	CONSTANT("SWFACTION_ROLLOVER",          SWFACTION_ROLLOVER);
	CONSTANT("SWFACTION_DRAGOVER",          SWFACTION_DRAGOVER);
	CONSTANT("SWFACTION_DRAGOUT",           SWFACTION_DRAGOUT);
	CONSTANT("SWFACTION_KEYPRESS",          SWFACTION_KEYPRESS);
	CONSTANT("SWFACTION_CONSTRUCT",         SWFACTION_CONSTRUCT);

  /* flags for SWFSound */
	CONSTANT("SWF_SOUND_NOT_COMPRESSED",    SWF_SOUND_NOT_COMPRESSED);
	CONSTANT("SWF_SOUND_ADPCM_COMPRESSED",  SWF_SOUND_ADPCM_COMPRESSED);
	CONSTANT("SWF_SOUND_MP3_COMPRESSED",    SWF_SOUND_MP3_COMPRESSED);
	CONSTANT("SWF_SOUND_NOT_COMPRESSED_LE", SWF_SOUND_NOT_COMPRESSED_LE);
	CONSTANT("SWF_SOUND_NELLY_COMPRESSED",  SWF_SOUND_NELLY_COMPRESSED);
	CONSTANT("SWF_SOUND_5KHZ",              SWF_SOUND_5KHZ);
	CONSTANT("SWF_SOUND_11KHZ",             SWF_SOUND_11KHZ);
	CONSTANT("SWF_SOUND_22KHZ",             SWF_SOUND_22KHZ);
	CONSTANT("SWF_SOUND_44KHZ",             SWF_SOUND_44KHZ);
	CONSTANT("SWF_SOUND_8BITS",             SWF_SOUND_8BITS);
	CONSTANT("SWF_SOUND_16BITS",            SWF_SOUND_16BITS);
	CONSTANT("SWF_SOUND_MONO",              SWF_SOUND_MONO);
	CONSTANT("SWF_SOUND_STEREO",            SWF_SOUND_STEREO);

#ifdef HAVE_NEW_MING
	/* Blend Modes */
	CONSTANT("SWFBLEND_MODE_NORMAL", 		SWFBLEND_MODE_NORMAL);
	CONSTANT("SWFBLEND_MODE_LAYER", 		SWFBLEND_MODE_LAYER);
	CONSTANT("SWFBLEND_MODE_MULT", 			SWFBLEND_MODE_MULT);
	CONSTANT("SWFBLEND_MODE_SCREEN",		SWFBLEND_MODE_SCREEN);
	CONSTANT("SWFBLEND_MODE_DARKEN",		SWFBLEND_MODE_DARKEN);
	CONSTANT("SWFBLEND_MODE_LIGHTEN",		SWFBLEND_MODE_LIGHTEN);
	CONSTANT("SWFBLEND_MODE_ADD", 			SWFBLEND_MODE_ADD);
	CONSTANT("SWFBLEND_MODE_SUB",			SWFBLEND_MODE_SUB);
	CONSTANT("SWFBLEND_MODE_DIFF", 			SWFBLEND_MODE_DIFF);
	CONSTANT("SWFBLEND_MODE_INV", 			SWFBLEND_MODE_INV);
	CONSTANT("SWFBLEND_MODE_ALPHA", 		SWFBLEND_MODE_ALPHA);
	CONSTANT("SWFBLEND_MODE_ERASE", 		SWFBLEND_MODE_ERASE);
	CONSTANT("SWFBLEND_MODE_OVERLAY", 		SWFBLEND_MODE_OVERLAY);
	CONSTANT("SWFBLEND_MODE_HARDLIGHT", 		SWFBLEND_MODE_HARDLIGHT);

	/* filter types */
	CONSTANT("SWFFILTER_TYPE_DROPSHADOW",		SWFFILTER_TYPE_DROPSHADOW);
	CONSTANT("SWFFILTER_TYPE_BLUR",			SWFFILTER_TYPE_BLUR);
	CONSTANT("SWFFILTER_TYPE_GLOW",			SWFFILTER_TYPE_GLOW);
	CONSTANT("SWFFILTER_TYPE_BEVEL",		SWFFILTER_TYPE_BEVEL);
	CONSTANT("SWFFILTER_TYPE_GRADIENTGLOW",		SWFFILTER_TYPE_GRADIENTGLOW);
	CONSTANT("SWFFILTER_TYPE_CONVOLUTION", 		SWFFILTER_TYPE_CONVOLUTION);
	CONSTANT("SWFFILTER_TYPE_COLORMATRIX",		SWFFILTER_TYPE_COLORMATRIX);
	CONSTANT("SWFFILTER_TYPE_GRADIENTBEVEL", 	SWFFILTER_TYPE_GRADIENTBEVEL);	

	/* filter flags */
	CONSTANT("SWFFILTER_FLAG_CLAMP",		FILTER_FLAG_CLAMP);
	CONSTANT("SWFFILTER_FLAG_PRESERVE_ALPHA",	FILTER_FLAG_PRESERVE_ALPHA);

	/* filter modes */
	CONSTANT("SWFFILTER_MODE_INNER",		FILTER_MODE_INNER);
	CONSTANT("SWFFILTER_MODE_KO",			FILTER_MODE_KO);
	CONSTANT("SWFFILTER_MODE_COMPOSITE",		FILTER_MODE_COMPOSITE);
	CONSTANT("SWFFILTER_MODE_ONTOP",		FILTER_MODE_ONTOP);

	/* new gradient features */
	CONSTANT("SWF_GRADIENT_PAD",			SWF_GRADIENT_PAD);
	CONSTANT("SWF_GRADIENT_REFLECT",		SWF_GRADIENT_REFLECT);
	CONSTANT("SWF_GRADIENT_REPEAT", 		SWF_GRADIENT_REPEAT);
	CONSTANT("SWF_GRADIENT_NORMAL",			SWF_GRADIENT_NORMAL);
	CONSTANT("SWF_GRADIENT_LINEAR",			SWF_GRADIENT_LINEAR);

	/* shape options */
	CONSTANT("SWF_SHAPE3",				SWF_SHAPE3);
	CONSTANT("SWF_SHAPE4", 				SWF_SHAPE4);
	CONSTANT("SWF_SHAPE_USESCALINGSTROKES",		SWF_SHAPE_USESCALINGSTROKES);
	CONSTANT("SWF_SHAPE_USENONSCALINGSTROKES", 	SWF_SHAPE_USENONSCALINGSTROKES);

	/* linestyle 2 flags */
	CONSTANT("SWF_LINESTYLE_CAP_ROUND", 		SWF_LINESTYLE_CAP_ROUND);
	CONSTANT("SWF_LINESTYLE_CAP_NONE", 		SWF_LINESTYLE_CAP_NONE);
	CONSTANT("SWF_LINESTYLE_CAP_SQUARE",		SWF_LINESTYLE_CAP_SQUARE);
	CONSTANT("SWF_LINESTYLE_JOIN_ROUND",		SWF_LINESTYLE_JOIN_ROUND);
	CONSTANT("SWF_LINESTYLE_JOIN_BEVEL", 		SWF_LINESTYLE_JOIN_BEVEL);
	CONSTANT("SWF_LINESTYLE_JOIN_MITER",		SWF_LINESTYLE_JOIN_MITER);
	CONSTANT("SWF_LINESTYLE_FLAG_NOHSCALE",		SWF_LINESTYLE_FLAG_NOHSCALE);
	CONSTANT("SWF_LINESTYLE_FLAG_NOVSCALE", 	SWF_LINESTYLE_FLAG_NOVSCALE);
	CONSTANT("SWF_LINESTYLE_FLAG_HINTING",		SWF_LINESTYLE_FLAG_HINTING);
	CONSTANT("SWF_LINESTYLE_FLAG_NOCLOSE",		SWF_LINESTYLE_FLAG_NOCLOSE);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_ROUND", 	SWF_LINESTYLE_FLAG_ENDCAP_ROUND);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_NONE", 	SWF_LINESTYLE_FLAG_ENDCAP_NONE);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_SQUARE", 	SWF_LINESTYLE_FLAG_ENDCAP_SQUARE);
#endif

	le_swfshapep = zend_register_list_destructors_ex(destroy_SWFShape_resource, NULL, "SWFShape", module_number);
	le_swffillp = zend_register_list_destructors_ex(destroy_SWFFill_resource, NULL, "SWFFill", module_number);
	le_swfgradientp = zend_register_list_destructors_ex(destroy_SWFGradient_resource, NULL, "SWFGradient", module_number);
	le_swfbitmapp = zend_register_list_destructors_ex(destroy_SWFBitmap_resource, NULL, "SWFBitmap", module_number);
	le_swftextp = zend_register_list_destructors_ex(destroy_SWFText_resource, NULL, "SWFText", module_number);
	le_swftextfieldp = zend_register_list_destructors_ex(destroy_SWFTextField_resource, NULL, "SWFTextField", module_number);
	le_swffontp = zend_register_list_destructors_ex(destroy_SWFFont_resource, NULL, "SWFFont", module_number);
	le_swfdisplayitemp = zend_register_list_destructors_ex(NULL, NULL, "SWFDisplayItem", module_number);
	le_swfmoviep = zend_register_list_destructors_ex(destroy_SWFMovie_resource, NULL, "SWFMovie", module_number);
	le_swfbuttonp = zend_register_list_destructors_ex(destroy_SWFButton_resource, NULL, "SWFButton", module_number);
	le_swfactionp = zend_register_list_destructors_ex(NULL, NULL, "SWFAction", module_number);
	le_swfmorphp = zend_register_list_destructors_ex(destroy_SWFMorph_resource, NULL, "SWFMorph", module_number);
	le_swfmovieclipp = zend_register_list_destructors_ex(destroy_SWFSprite_resource, NULL, "SWFMovieClip", module_number);
	le_swfspritep = zend_register_list_destructors_ex(destroy_SWFSprite_resource, NULL, "SWFSprite", module_number);
	le_swfinputp = zend_register_list_destructors_ex(destroy_SWFInput_resource, NULL, "SWFInput", module_number);

	le_swfsoundp = zend_register_list_destructors_ex(destroy_SWFSound_resource, NULL, "SWFSound", module_number);
#ifdef HAVE_NEW_MING
	le_swffontcharp = zend_register_list_destructors_ex(NULL, NULL, "SWFFontCharacter", module_number);
	le_swfbuttonrecordp = zend_register_list_destructors_ex(NULL, NULL, "SWFButtonRecord", module_number);
	le_swfsoundinstancep = zend_register_list_destructors_ex(NULL, NULL, "SWFSoundInstance", module_number);
	le_swfvideostreamp = zend_register_list_destructors_ex(destroy_SWFVideoStream_resource, NULL, "SWFVideoStream", module_number);
	le_swfbinarydatap = zend_register_list_destructors_ex(destroy_SWFBinaryData_resource, NULL, "SWFBinaryData", module_number);
	le_swfinitactionp = zend_register_list_destructors_ex(NULL, NULL, "SWFInitAction", module_number);
	le_swfprebuiltclipp = zend_register_list_destructors_ex(destroy_SWFPrebuiltClip_resource, NULL, "SWFPrebuiltClip", module_number);
	le_swfsoundstreamp = zend_register_list_destructors_ex(destroy_SWFSoundStream_resource, NULL, "SWFSoundStream", module_number);
	le_swffilterp = zend_register_list_destructors_ex(destroy_SWFFilter_resource, NULL, "SWFFilter", module_number);
	le_swfblurp = zend_register_list_destructors_ex(destroy_SWFBlur_resource, NULL, "SWFBlur", module_number);
	le_swfshadowp = zend_register_list_destructors_ex(destroy_SWFShadow_resource, NULL, "SWFShadow", module_number);
	le_swffiltermatrixp = zend_register_list_destructors_ex(destroy_SWFFilterMatrix_resource, NULL, "SWFFilterMatrix", module_number);
	le_swfcharacterp = zend_register_list_destructors_ex(NULL, NULL, "SWFCharacter", module_number);
	le_swfcxformp = zend_register_list_destructors_ex(destroy_SWFCXform_resource, NULL, "SWFCXform", module_number);
	le_swfmatrixp = zend_register_list_destructors_ex(NULL, NULL, "SWFMatrix", module_number);
	le_swfbrowserfontp = zend_register_list_destructors_ex(destroy_SWFBrowserFont_resource, NULL, "SWFBrowserFont", module_number);
	le_swffontcollectionp = zend_register_list_destructors_ex(destroy_SWFFontCollection_resource, NULL, "SWFFontCollection", module_number);
#endif

	INIT_CLASS_ENTRY(shape_class_entry, "SWFShape", swfshape_functions);
	INIT_CLASS_ENTRY(fill_class_entry, "SWFFill", swffill_functions);
	INIT_CLASS_ENTRY(gradient_class_entry, "SWFGradient", swfgradient_functions);
	INIT_CLASS_ENTRY(bitmap_class_entry, "SWFBitmap", swfbitmap_functions);
	INIT_CLASS_ENTRY(text_class_entry, "SWFText", swftext_functions);
	INIT_CLASS_ENTRY(textfield_class_entry, "SWFTextField", swftextfield_functions);
	INIT_CLASS_ENTRY(font_class_entry, "SWFFont", swffont_functions);
	INIT_CLASS_ENTRY(displayitem_class_entry, "SWFDisplayItem", swfdisplayitem_functions);
	INIT_CLASS_ENTRY(movie_class_entry, "SWFMovie", swfmovie_functions);
	INIT_CLASS_ENTRY(button_class_entry, "SWFButton", swfbutton_functions);
	INIT_CLASS_ENTRY(action_class_entry, "SWFAction", swfaction_functions);
	INIT_CLASS_ENTRY(morph_class_entry, "SWFMorph", swfmorph_functions);
	INIT_CLASS_ENTRY(movieclip_class_entry, "SWFMovieClip", swfmovieclip_functions);
	INIT_CLASS_ENTRY(sprite_class_entry, "SWFSprite", swfsprite_functions);
	INIT_CLASS_ENTRY(sound_class_entry, "SWFSound", swfsound_functions);
#ifdef HAVE_NEW_MING
	INIT_CLASS_ENTRY(fontchar_class_entry, "SWFFontChar", swffontchar_functions);
	INIT_CLASS_ENTRY(buttonrecord_class_entry, "SWFButtonRecord", swfbuttonrecord_functions);
	INIT_CLASS_ENTRY(soundinstance_class_entry, "SWFSoundInstance", swfsoundinstance_functions);
	INIT_CLASS_ENTRY(videostream_class_entry, "SWFVideoStream", swfvideostream_functions);
	INIT_CLASS_ENTRY(binarydata_class_entry, "SWFBinaryData", swfbinarydata_functions);
	INIT_CLASS_ENTRY(initaction_class_entry, "SWFInitAction", swfinitaction_functions);
	INIT_CLASS_ENTRY(prebuiltclip_class_entry, "SWFPrebuiltClip", swfprebuiltclip_functions);
	INIT_CLASS_ENTRY(soundstream_class_entry, "SWFSoundStream", swfsoundstream_functions);
	INIT_CLASS_ENTRY(filter_class_entry, "SWFFilter", swffilter_functions);
	INIT_CLASS_ENTRY(filtermatrix_class_entry, "SWFFilterMatrix", swffiltermatrix_functions);
	INIT_CLASS_ENTRY(shadow_class_entry, "SWFShadow", swfshadow_functions);
	INIT_CLASS_ENTRY(blur_class_entry, "SWFBlur", swfblur_functions);
	INIT_CLASS_ENTRY(cxform_class_entry, "SWFCXform", swfcxform_functions);
	INIT_CLASS_ENTRY(matrix_class_entry, "SWFMatrix", swfmatrix_functions);
	INIT_CLASS_ENTRY(input_class_entry, "SWFInput", swfinput_functions);
	INIT_CLASS_ENTRY(browserfont_class_entry, "SWFBrowserFont", swfbrowserfont_functions);
	INIT_CLASS_ENTRY(fontcollection_class_entry, "SWFFontCollection", swffontcollection_functions);
#endif
	INIT_CLASS_ENTRY(character_class_entry, "SWFCharacter", swfcharacter_functions);


	shape_class_entry_ptr = zend_register_internal_class(&shape_class_entry TSRMLS_CC);
	fill_class_entry_ptr = zend_register_internal_class(&fill_class_entry TSRMLS_CC);
	gradient_class_entry_ptr = zend_register_internal_class(&gradient_class_entry TSRMLS_CC);
	bitmap_class_entry_ptr = zend_register_internal_class(&bitmap_class_entry TSRMLS_CC);
	text_class_entry_ptr = zend_register_internal_class(&text_class_entry TSRMLS_CC);
	textfield_class_entry_ptr = zend_register_internal_class(&textfield_class_entry TSRMLS_CC);
	font_class_entry_ptr = zend_register_internal_class(&font_class_entry TSRMLS_CC);
	displayitem_class_entry_ptr = zend_register_internal_class(&displayitem_class_entry TSRMLS_CC);
	movie_class_entry_ptr = zend_register_internal_class(&movie_class_entry TSRMLS_CC);
	button_class_entry_ptr = zend_register_internal_class(&button_class_entry TSRMLS_CC);
	action_class_entry_ptr = zend_register_internal_class(&action_class_entry TSRMLS_CC);
	morph_class_entry_ptr = zend_register_internal_class(&morph_class_entry TSRMLS_CC);
	movieclip_class_entry_ptr = zend_register_internal_class(&movieclip_class_entry TSRMLS_CC);
	sprite_class_entry_ptr = zend_register_internal_class(&sprite_class_entry TSRMLS_CC);
	sound_class_entry_ptr = zend_register_internal_class(&sound_class_entry TSRMLS_CC);
#ifdef HAVE_NEW_MING
	fontchar_class_entry_ptr = zend_register_internal_class(&fontchar_class_entry TSRMLS_CC);
	buttonrecord_class_entry_ptr = zend_register_internal_class(&buttonrecord_class_entry TSRMLS_CC);
	soundinstance_class_entry_ptr = zend_register_internal_class(&soundinstance_class_entry TSRMLS_CC);
	videostream_class_entry_ptr = zend_register_internal_class(&videostream_class_entry TSRMLS_CC);
	binarydata_class_entry_ptr = zend_register_internal_class(&binarydata_class_entry TSRMLS_CC);
	initaction_class_entry_ptr = zend_register_internal_class(&initaction_class_entry TSRMLS_CC);
	prebuiltclip_class_entry_ptr = zend_register_internal_class(&prebuiltclip_class_entry TSRMLS_CC);
	soundstream_class_entry_ptr = zend_register_internal_class(&soundstream_class_entry TSRMLS_CC);
	filter_class_entry_ptr = zend_register_internal_class(&filter_class_entry TSRMLS_CC);
	filtermatrix_class_entry_ptr = zend_register_internal_class(&filtermatrix_class_entry TSRMLS_CC);
	shadow_class_entry_ptr = zend_register_internal_class(&shadow_class_entry TSRMLS_CC);
	blur_class_entry_ptr = zend_register_internal_class(&blur_class_entry TSRMLS_CC);
	cxform_class_entry_ptr = zend_register_internal_class(&cxform_class_entry TSRMLS_CC);
	matrix_class_entry_ptr = zend_register_internal_class(&matrix_class_entry TSRMLS_CC);
	input_class_entry_ptr = zend_register_internal_class(&input_class_entry TSRMLS_CC);
	browserfont_class_entry_ptr = zend_register_internal_class(&browserfont_class_entry TSRMLS_CC);
	fontcollection_class_entry_ptr = zend_register_internal_class(&fontcollection_class_entry TSRMLS_CC);
#endif
	character_class_entry_ptr = zend_register_internal_class(&character_class_entry TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
