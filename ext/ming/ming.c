/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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

#define FLOAT_Z_DVAL_PP(x) ((float)Z_DVAL_PP(x))
#define BYTE_Z_LVAL_PP(x)  ((byte)Z_LVAL_PP(x))

#ifndef HAVE_DESTROY_SWF_BLOCK
void destroySWFBlock(SWFBlock block);
#endif

static zend_function_entry ming_functions[] = {
	PHP_FALIAS(ming_setcubicthreshold,  ming_setCubicThreshold,  NULL)
	PHP_FALIAS(ming_setscale,           ming_setScale,           NULL)
	PHP_FALIAS(ming_useswfversion,      ming_useSWFVersion,      NULL)
	PHP_FALIAS(ming_keypress,           ming_keypress,           NULL)
#ifdef HAVE_NEW_MING
	PHP_FALIAS(ming_useconstants,		ming_useConstants,       NULL)
#endif
#ifdef HAVE_MING_SETSWFCOMPRESSION
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
static SWFMorph getMorph(zval *id TSRMLS_DC);
static SWFMovieClip getSprite(zval *id TSRMLS_DC);
static SWFSound getSound(zval *id TSRMLS_DC);
#ifdef HAVE_NEW_MING
static SWFFontCharacter getFontCharacter(zval *id TSRMLS_DC);
static SWFSoundInstance getSoundInstance(zval *id TSRMLS_DC);
static SWFVideoStream getVideoStream(zval *id TSRMLS_DC);
#endif
#ifdef HAVE_SWFPREBUILTCLIP
static SWFPrebuiltClip getPrebuiltClip(zval *id TSRMLS_DC);
#endif

#define PHP_MING_FILE_CHK(file) \
	if ((PG(safe_mode) && !php_checkuid((file), NULL, CHECKUID_CHECK_FILE_AND_DIR)) || php_check_open_basedir((file) TSRMLS_CC)) {	\
		RETURN_FALSE;	\
	}	\

/* {{{ proto void ming_setcubicthreshold (int threshold)
   Set cubic threshold (?) */
PHP_FUNCTION(ming_setCubicThreshold)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	Ming_setCubicThreshold(Z_LVAL_PP(num));
}
/* }}} */

/* {{{ proto void ming_setscale(int scale)
   Set scale (?) */
PHP_FUNCTION(ming_setScale)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Ming_setScale(FLOAT_Z_DVAL_PP(num));
}
/* }}} */

/* {{{ proto void ming_useswfversion(int version)
   Use SWF version (?) */ 
PHP_FUNCTION(ming_useSWFVersion)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	Ming_useSWFVersion(Z_LVAL_PP(num));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void ming_useconstants(int use)
	Use constant pool (?) */ 
PHP_FUNCTION(ming_useConstants)
{
	zval **num;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(num);

	Ming_useConstants(Z_LVAL_PP(num));
}
/* }}} */
#endif

#ifdef HAVE_MING_SETSWFCOMPRESSION
/* {{{ set output compression */
PHP_FUNCTION(ming_setSWFCompression)
{  
    zval **num;
    if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE)
        WRONG_PARAM_COUNT;
				   
    convert_to_long_ex(num);
    Ming_setSWFCompression(Z_LVAL_PP(num));
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
static int le_swfspritep;
static int le_swfinputp;
static int le_swfsoundp;
#ifdef HAVE_NEW_MING
static int le_swffontcharp;
static int le_swfsoundinstancep;
static int le_swfvideostreamp;
#endif
#ifdef HAVE_SWFPREBUILTCLIP
static int le_swfprebuiltclipp;
#endif

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
static zend_class_entry *sprite_class_entry_ptr;
static zend_class_entry *sound_class_entry_ptr;
#ifdef HAVE_NEW_MING
static zend_class_entry *fontchar_class_entry_ptr;
static zend_class_entry *soundinstance_class_entry_ptr;
static zend_class_entry *videostream_class_entry_ptr;
#endif
#ifdef HAVE_SWFPREBUILTCLIP
static zend_class_entry *prebuiltclip_class_entry_ptr;
#endif
 
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

/* {{{ SWFCharacter - not a real class
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
#endif
#ifdef HAVE_SWFPREBUILTCLIP
	else if(Z_OBJCE_P(id) == prebuiltclip_class_entry_ptr)
		return (SWFCharacter)getPrebuiltClip(id TSRMLS_CC);
#endif
	else
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFCharacter");
		return NULL;
}
/* }}} */
/* }}} */

/* {{{ getInput - utility func for making an SWFInput from an fopened resource
*/
static void destroy_SWFInput_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFInput((SWFInput)resource->ptr);
}

static SWFInput getInput(zval **zfile TSRMLS_DC)
{
	FILE *file;
	php_stream *stream;
	SWFInput input;

	php_stream_from_zval_no_verify(stream, zfile);

	if (stream == NULL)
		return NULL;

	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void *) &file, REPORT_ERRORS) != SUCCESS) {
		return NULL;
	}

	input = newSWFInput_file(file);
	zend_list_addref(Z_LVAL_PP(zfile));
	zend_list_addref(zend_list_insert(input, le_swfinputp));
	return input;
}
/* }}} */

/* {{{ SWFAction
*/
/* {{{ proto void swfaction::__construct(string)
   Creates a new SWFAction object, compiling the given script */
PHP_METHOD(swfaction, __construct)
{
	SWFAction action;
	zval **script;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &script) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(script);
	action = compileSWFActionCode(Z_STRVAL_PP(script));

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Couldn't compile actionscript");
	}
	
	ret = zend_list_insert(action, le_swfactionp);

	object_init_ex(getThis(), action_class_entry_ptr);
	add_property_resource(getThis(), "action", ret);
	zend_list_addref(ret);
}
/* no destructor for SWFAction, it's not a character */
/* }}} */

/* {{{ internal function getAction
   Returns the SWFAction object contained in zval *id */
static SWFAction getAction(zval *id TSRMLS_DC)
{
	void *action = SWFgetProperty(id, "action", 6, le_swfactionp TSRMLS_CC);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFAction");
	}
	return (SWFAction)action;
}
/* }}} */

static zend_function_entry swfaction_functions[] = {
	PHP_ME(swfaction, __construct,         NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFBitmap 
*/
/* {{{ proto void swfbitmap::__construct(mixed file [, mixed maskfile])
   Creates a new SWFBitmap object from jpg (with optional mask) or dbl file */
PHP_METHOD(swfbitmap, __construct)
{
	zval **zfile, **zmask = NULL;
	SWFBitmap bitmap;
	SWFJpegWithAlpha bitmap_alpha;
	SWFInput input, maskinput;
	int ret;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfile) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &zfile, &zmask) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
	
	if (zmask != NULL) {
		if (Z_TYPE_PP(zmask) != IS_RESOURCE) {
			convert_to_string_ex(zmask);
			maskinput = newSWFInput_buffer(Z_STRVAL_PP(zmask), Z_STRLEN_PP(zmask));
			zend_list_addref(zend_list_insert(maskinput, le_swfinputp));
		} else {
			maskinput = getInput(zmask TSRMLS_CC);
		}
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFBitmap");
	}
	return (SWFBitmap)bitmap;
}
/* }}} */

/* {{{ proto float swfbitmap::getWidth()
   Returns the width of this bitmap */
PHP_METHOD(swfbitmap, getWidth)
{
	if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFBitmap_getWidth(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swfbitmap::getHeight()
   Returns the height of this bitmap */
PHP_METHOD(swfbitmap, getHeight)
{
	if(ZEND_NUM_ARGS() != 0) {
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFButton");
	}
	return (SWFButton)button;
}
/* }}} */

/* {{{ proto void swfbutton::setHit(object SWFCharacter)
   Sets the character for this button's hit test state */
PHP_METHOD(swfbutton, setHit)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addShape(button, character, SWFBUTTONRECORD_HITSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setOver(object SWFCharacter)
   Sets the character for this button's over state */
PHP_METHOD(swfbutton, setOver)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addShape(button, character, SWFBUTTONRECORD_OVERSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setUp(object SWFCharacter)
   Sets the character for this button's up state */
PHP_METHOD(swfbutton, setUp)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addShape(button, character, SWFBUTTONRECORD_UPSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setDown(object SWFCharacter)
   Sets the character for this button's down state */
PHP_METHOD(swfbutton, setDown)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addShape(button, character, SWFBUTTONRECORD_DOWNSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::addShape(object SWFCharacter, int flags)
   Sets the character to display for the condition described in flags */
PHP_METHOD(swfbutton, addShape)
{
	zval **zchar, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	convert_to_long_ex(flags);
	SWFButton_addShape(button, character, BYTE_Z_LVAL_PP(flags));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swfbutton::setMenu(int flag)
	enable track as menu button behaviour */

PHP_METHOD(swfbutton, setMenu)
{
	zval **zflag;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zflag) == FAILURE)
		WRONG_PARAM_COUNT;	
	convert_to_long_ex(zflag);

	SWFButton_setMenu(button, Z_LVAL_PP(zflag));
}
/* }}} */
#endif

/* {{{ proto void swfbutton::setAction(object SWFAction)
   Sets the action to perform when button is pressed */
PHP_METHOD(swfbutton, setAction)
{
	zval **zaction;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zaction) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zaction);
	action = getAction(*zaction TSRMLS_CC);
	SWFButton_addAction(button, action, SWFBUTTON_OVERDOWNTOOVERUP);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto SWFSoundInstance swfbutton::addASound(SWFSound sound, int flags)
   associates a sound with a button transition
	NOTE: the transitions are all wrong _UP, _OVER, _DOWN _HIT  */

PHP_METHOD(swfbutton, addSound)
{
	zval **zsound, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFSound sound;
	SWFSoundInstance item;
	int ret;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zsound, &flags) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = getSound(*zsound TSRMLS_CC);

	convert_to_long_ex(flags);

	item = SWFButton_addSound(button, sound, Z_LVAL_PP(flags));

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
	zval **zaction, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zaction, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zaction);
	action = getAction(*zaction TSRMLS_CC);
	convert_to_long_ex(flags);
	SWFButton_addAction(button, action, Z_LVAL_PP(flags));
}
/* }}} */

/* {{{ proto int ming_keypress(string str)
   Returns the action flag for keyPress(char) */
PHP_FUNCTION(ming_keypress)
{
	zval **key;
	char c;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(key);

	if (Z_STRLEN_PP(key) > 1) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Only one character expected");
	}
	
	c = Z_STRVAL_PP(key)[0];
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
#endif
	PHP_ME(swfbutton, addAction,     NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfbutton, addSound,      NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFDisplayitem
*/
/* {{{ internal function getDisplayItem
   Returns the SWFDisplayItem contained in zval *id */
static SWFDisplayItem getDisplayItem(zval *id TSRMLS_DC)
{
	void *item = SWFgetProperty(id, "displayitem", 11, le_swfdisplayitemp TSRMLS_CC);

	if (!item) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFDisplayItem");
	}

	return (SWFDisplayItem)item;
}
/* }}} */

/* {{{ proto void swfdisplayitem::moveTo(int x, int y)
   Moves this SWFDisplayItem to movie coordinates (x, y) */
PHP_METHOD(swfdisplayitem, moveTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_moveTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::move(float dx, float dy)
   Displaces this SWFDisplayItem by (dx, dy) in movie coordinates */
PHP_METHOD(swfdisplayitem, move)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_move(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::scaleTo(float xScale [, float yScale])
   Scales this SWFDisplayItem by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swfdisplayitem, scaleTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(x));
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &x, &y) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		convert_to_double_ex(y);
		SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfdisplayitem::scale(float xScale, float yScale)
   Multiplies this SWFDisplayItem's current x scale by xScale, its y scale by yScale */
PHP_METHOD(swfdisplayitem, scale)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_scale(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotateTo(float degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its original orientation */
PHP_METHOD(swfdisplayitem, rotateTo)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(degrees);
	SWFDisplayItem_rotateTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotate(float degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its current orientation */
PHP_METHOD(swfdisplayitem, rotate)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(degrees);
	SWFDisplayItem_rotate(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewXTo(float xSkew)
   Sets this SWFDisplayItem's x skew value to xSkew */
PHP_METHOD(swfdisplayitem, skewXTo)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	SWFDisplayItem_skewXTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewX(float xSkew)
   Adds xSkew to this SWFDisplayItem's x skew value */
PHP_METHOD(swfdisplayitem, skewX)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	SWFDisplayItem_skewX(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewYTo(float ySkew)
   Sets this SWFDisplayItem's y skew value to ySkew */
PHP_METHOD(swfdisplayitem, skewYTo)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(y);
	SWFDisplayItem_skewYTo(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewY(float ySkew)
   Adds ySkew to this SWFDisplayItem's y skew value */
PHP_METHOD(swfdisplayitem, skewY)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(y);
	SWFDisplayItem_skewY(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMatrix(float a, float b, float c, float d, float x, float y)
   Sets the item's transform matrix */
PHP_METHOD(swfdisplayitem, setMatrix)
{
	zval **a, **b, **c, **d, **x, **y;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &a, &b, &c, &d, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(a);
	convert_to_double_ex(b);
	convert_to_double_ex(c);
	convert_to_double_ex(d);
	convert_to_double_ex(x);
	convert_to_double_ex(y);

	SWFDisplayItem_setMatrix( getDisplayItem(getThis() TSRMLS_CC), 
		FLOAT_Z_DVAL_PP(a), FLOAT_Z_DVAL_PP(b), FLOAT_Z_DVAL_PP(c), FLOAT_Z_DVAL_PP(d), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y)
	);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setDepth(int depth)
   Sets this SWFDisplayItem's z-depth to depth.  Items with higher depth values are drawn on top of those with lower values */
PHP_METHOD(swfdisplayitem, setDepth)
{
	zval **depth;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &depth) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(depth);
	SWFDisplayItem_setDepth(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto void swfdisplayitem::setRatio(float ratio)
   Sets this SWFDisplayItem's ratio to ratio.  Obviously only does anything if displayitem was created from an SWFMorph */
PHP_METHOD(swfdisplayitem, setRatio)
{
	zval **ratio;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &ratio) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(ratio);
	SWFDisplayItem_setRatio(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(ratio));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addColor(int r, int g, int b [, int a])
   Sets the add color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 0 */
PHP_METHOD(swfdisplayitem, addColor)
{
	zval **r, **g, **b, **za;
	int a = 0;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(za);
		a = Z_LVAL_PP(za);
	} else if (ZEND_NUM_ARGS() == 3) { 
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	SWFDisplayItem_setColorAdd(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(r), Z_LVAL_PP(g), Z_LVAL_PP(b), a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::multColor(float r, float g, float b [, float a])
   Sets the multiply color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 1.0 */
PHP_METHOD(swfdisplayitem, multColor)
{
	zval **r, **g, **b, **za;
	float a = 1.0f;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) { 
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(za);
		a = FLOAT_Z_DVAL_PP(za);
	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);

	SWFDisplayItem_setColorMult(getDisplayItem(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(r), FLOAT_Z_DVAL_PP(g), FLOAT_Z_DVAL_PP(b), a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setName(string name)
   Sets this SWFDisplayItem's name to name */
PHP_METHOD(swfdisplayitem, setName)
{
	zval **name;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(name);
	SWFDisplayItem_setName(item, Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addAction(object SWFAction, int flags)
   Adds this SWFAction to the given SWFSprite instance */
PHP_METHOD(swfdisplayitem, addAction)
{
	zval **zaction, **flags;
	SWFAction action;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zaction, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_object_ex(zaction);
	convert_to_long_ex(flags);
	action = getAction(*zaction TSRMLS_CC);
	SWFDisplayItem_addAction(item, action, Z_LVAL_PP(flags));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ swfdisplayitem_remove */

PHP_METHOD(swfdisplayitem, remove)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_remove(item);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMaskLevel(int level)
   defines a MASK layer at level */

PHP_METHOD(swfdisplayitem, setMaskLevel)
{
	zval **level;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &level) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(level);

	SWFDisplayItem_setMaskLevel(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(level));
}
/* }}} */

/* {{{ proto void swfdisplayitem::endMask()
   another way of defining a MASK layer */

PHP_METHOD(swfdisplayitem, endMask)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_endMask(item);
}
/* }}} */

/* {{{ swfdisplayitem_getX */

PHP_METHOD(swfdisplayitem, getX)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_x(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getY */

PHP_METHOD(swfdisplayitem, getY)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_y(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getXScale */

PHP_METHOD(swfdisplayitem, getXScale)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_xScale(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getYScale */

PHP_METHOD(swfdisplayitem, getYScale)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_yScale(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getXSkew */

PHP_METHOD(swfdisplayitem, getXSkew)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_xSkew(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getYSkew */

PHP_METHOD(swfdisplayitem, getYSkew)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_ySkew(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getRot */

PHP_METHOD(swfdisplayitem, getRot)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_get_rot(item);
	RETURN_DOUBLE(ret);
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
  php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Instantiating SWFFill won't do any good- use SWFShape::addFill() instead");
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFFill");
	}
	return (SWFFill)fill;
}
/* }}} */

/* {{{ proto void swffill::moveTo(float x, float y)
   Moves this SWFFill to shape coordinates (x,y) */
PHP_METHOD(swffill, moveTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFFill_moveTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swffill::scaleTo(float xScale [, float yScale])
   Scales this SWFFill by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swffill, scaleTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(x));
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &x, &y) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		convert_to_double_ex(y);
		SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swffill::rotateTo(float degrees)
   Rotates this SWFFill the given (clockwise) degrees from its original orientation */
PHP_METHOD(swffill, rotateTo)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(degrees);
	SWFFill_rotateTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swffill::skewXTo(float xSkew)
   Sets this SWFFill's x skew value to xSkew */
PHP_METHOD(swffill, skewXTo)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	SWFFill_skewXTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swffill::skewYTo(float ySkew)
   Sets this SWFFill's y skew value to ySkew */
PHP_METHOD(swffill, skewYTo)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(y);
	SWFFill_skewYTo(getFill(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(y));
}
/* }}} */
static zend_function_entry swffill_functions[] = {
	PHP_ME(swffill, __construct, NULL, 0)
	PHP_ME(swffill, moveTo,      NULL, 0)
	PHP_ME(swffill, scaleTo,     NULL, 0)
	PHP_ME(swffill, rotateTo,    NULL, 0)
	PHP_ME(swffill, skewXTo,     NULL, 0)
	PHP_ME(swffill, skewYTo,     NULL, 0)
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
		php_error(E_RECOVERABLE_ERROR, "called object is not an SWFFontCharacter!");

	return (SWFFontCharacter)font;
}
/* }}} */

static void destroy_SWFFontCharacter_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlock((SWFBlock)resource->ptr);
}

/* {{{ proto void swffontcha::raddChars(string)
   adds characters to a font for exporting font */
PHP_METHOD(swffontchar, addChars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFontCharacter_addChars(getFontCharacter(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
/* }}} */

/* {{{ proto void swffontchar::addChars(string)
   adds characters to a font for exporting font */

PHP_METHOD(swffontchar, addUTF8Chars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFontCharacter_addUTF8Chars(getFontCharacter(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
/* }}} */

static zend_function_entry swffontchar_functions[] = {
	PHP_ME(swffontchar, addChars,   NULL, 0)
	PHP_ME(swffontchar, addUTF8Chars,  NULL, 0)
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFFont");
	}
	return (SWFFont)font;
}
/* }}} */

/* {{{ proto void swffont::__construct(string filename)
   Creates a new SWFFont object from given file */
PHP_METHOD(swffont, __construct)
{
	zval **zfile;
	SWFFont font;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfile) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(zfile);

	if (strcmp(Z_STRVAL_PP(zfile)+Z_STRLEN_PP(zfile)-4, ".fdb") == 0) {
		php_stream * stream;
		FILE * file;
	
		stream = php_stream_open_wrapper(Z_STRVAL_PP(zfile), "rb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);

		if (stream == NULL) {
			RETURN_FALSE;
		}

		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void*)&file, REPORT_ERRORS)) {
			php_stream_close(stream);
			RETURN_FALSE;
		}
	
		font = loadSWFFontFromFile(file);
		php_stream_close(stream);
	} else {
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		font = (SWFFont)newSWFBrowserFont(Z_STRVAL_PP(zfile));
	}

	ret = zend_list_insert(font, le_swffontp);

	object_init_ex(getThis(), font_class_entry_ptr);
	add_property_resource(getThis(), "font", ret);
	zend_list_addref(ret);
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
	zval **zstring;
	float width;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zstring);
	width = SWFFont_getStringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
	RETURN_DOUBLE(width);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto int swffont::getUTF8Width(string)
   Calculates the width of the given string in this font at full height */

PHP_METHOD(swffont, getUTF8Width)
{
	zval **zstring;
	float width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFFont_getUTF8StringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}

/* }}} */
/* not sure about 0 bytes !!!!!!!!! */
/* {{{ proto int swffont::getWideWidth(string)
   Calculates the width of the given string in this font at full height */
/*
PHP_METHOD(swffont, getWideWidth)
{
	zval **zstring;
	float width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFFont_getWideStringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}
*/
/* }}} */
#endif

/* {{{ proto float swffont::getAscent()
   Returns the ascent of the font, or 0 if not available */
PHP_METHOD(swffont, getAscent)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getAscent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getDescent()
   Returns the descent of the font, or 0 if not available */
PHP_METHOD(swffont, getDescent)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getDescent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getLeading()
   Returns the leading of the font, or 0 if not available */
PHP_METHOD(swffont, getLeading)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getLeading(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swffont::addChars(string)
   adds characters to a font required within textfields */
/*
PHP_METHOD(swffont, addChars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFont_addChars(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
*/
/* }}} */

/* {{{ proto string swffont::getShape(code)
   Returns the glyph shape of a char as a text string */
PHP_METHOD(swffont, getShape)
{
	zval **zcode;
	char *result;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zcode) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zcode);

	result = SWFFont_getShape(getFont(getThis() TSRMLS_CC), Z_LVAL_PP(zcode));
	RETVAL_STRING(result, 1);
	free(result);
}
/* }}} */

#endif

static zend_function_entry swffont_functions[] = {
	PHP_ME(swffont, __construct,       NULL, 0)
	PHP_ME(swffont, getWidth,          NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swffont, getUTF8Width,      NULL, 0)
/*	PHP_ME(swffont, getwidewidth,      NULL, 0)*/
#endif
	PHP_ME(swffont, getAscent,         NULL, 0)
	PHP_ME(swffont, getDescent,        NULL, 0)
	PHP_ME(swffont, getLeading,        NULL, 0)
#ifdef HAVE_NEW_MING
/*	PHP_ME(swffont, addchars,          NULL, 0)*/
	PHP_ME(swffont, getShape,          NULL, 0)
#endif
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFGradient");
	}
	return (SWFGradient)gradient;
}
/* }}} */

/* {{{ proto void swfgradient::addEntry(float ratio, int r, int g, int b [, int a])
   Adds given entry to the gradient */
PHP_METHOD(swfgradient, addEntry)
{
	zval **ratio, **r, **g, **b;
	byte a = 0xff;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &ratio, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 5) {
		zval **za;

		if (zend_get_parameters_ex(5, &ratio, &r, &g, &b, &za) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(za);
		a = BYTE_Z_LVAL_PP(za);
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(ratio);
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	SWFGradient_addEntry( getGradient(getThis() TSRMLS_CC), 
		FLOAT_Z_DVAL_PP(ratio), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), a
	);
}
/* }}} */

static zend_function_entry swfgradient_functions[] = {
	PHP_ME(swfgradient, __construct, NULL, 0)
	PHP_ME(swfgradient, addEntry,    NULL, 0)
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFMorph");
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

/* {{{ SWFSound */
/* {{{ internal function SWFSound getSound(zval *id)
   Returns the Sound object in zval *id */

SWFSound getSound(zval *id TSRMLS_DC)
{
	void *sound = SWFgetProperty(id, "sound", 5, le_swfsoundp TSRMLS_CC);

	if(!sound)
		php_error(E_RECOVERABLE_ERROR, "called object is not an SWFSound!");

	return (SWFSound)sound;
}

/* }}} */
/* {{{ proto void swfsound::__construct(string filename, int flags)
   Creates a new SWFSound object from given file */
PHP_METHOD(swfsound, __construct)
{
	zval **zfile, **zflags;
	SWFSound sound;
	SWFInput input;
	int flags;
	int ret;

	if(ZEND_NUM_ARGS() == 1)
	{
		if(zend_get_parameters_ex(1, &zfile) == FAILURE)
			WRONG_PARAM_COUNT;
		flags = 0;
	}
	else if(ZEND_NUM_ARGS() == 2)
	{
		if(zend_get_parameters_ex(2, &zfile, &zflags) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zflags);
		flags = Z_LVAL_PP(zflags);
	}
	else
		WRONG_PARAM_COUNT;

	if(Z_TYPE_PP(zfile) != IS_RESOURCE)
	{
		convert_to_string_ex(zfile);
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	}
	else
		input = getInput(zfile TSRMLS_CC);

#ifdef HAVE_NEW_MING
	sound = newSWFSound_fromInput(input, flags);
#else
	sound = newSWFSound_fromInput(input);
#endif

	ret = zend_list_insert(sound, le_swfsoundp);

	object_init_ex(getThis(), sound_class_entry_ptr);
	add_property_resource(getThis(), "sound", ret);
	zend_list_addref(ret);
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
		php_error(E_RECOVERABLE_ERROR, "called object is not an SWFSoundInstance!");

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
	zval **zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zpoint) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zpoint);

	SWFSoundInstance_setLoopInPoint(inst, Z_LVAL_PP(zpoint));
}

/* }}} */
/* {{{ swfsoundinstance_loopoutpoint(point) */

PHP_METHOD(swfsoundinstance, loopOutPoint)
{
	zval **zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zpoint) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zpoint);

	SWFSoundInstance_setLoopOutPoint(inst, Z_LVAL_PP(zpoint));
}
/* }}} */

/* {{{ swfsoundinstance_loopcount(point) */

PHP_METHOD(swfsoundinstance, loopCount)
{
	zval **zcount;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zcount) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zcount);

	SWFSoundInstance_setLoopCount(inst, Z_LVAL_PP(zcount));
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

/* {{{ proto class swfvideostream_init([file])
   Returns a SWVideoStream object */

PHP_METHOD(swfvideostream, __construct)
{
	zval **zfile = NULL;
	SWFVideoStream stream;
	SWFInput input;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &zfile) == FAILURE)
				WRONG_PARAM_COUNT;
	
			if(Z_TYPE_PP(zfile) != IS_RESOURCE)
  			{
			    convert_to_string_ex(zfile);
			    input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
			    zend_list_addref(zend_list_insert(input, le_swfinputp));
  			}
  			else
			    input = getInput(zfile TSRMLS_CC);
		
			stream = newSWFVideoStream_fromInput(input);
			break;
		case 0:
			stream = newSWFVideoStream();
			break;
		default:
			WRONG_PARAM_COUNT;
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
		php_error(E_RECOVERABLE_ERROR, "called object is not an SWFVideoStream!");
	                                                                                                                                         
	return (SWFVideoStream)stream;
}

/* }}} */

/* {{{ setDimension */

PHP_METHOD(swfvideostream, setdimension)
{
	zval **x, **y;
	SWFVideoStream stream = getVideoStream(getThis() TSRMLS_CC);
	if(!stream)
		 php_error(E_RECOVERABLE_ERROR, "getVideoSTream returned NULL");

	if( ZEND_NUM_ARGS() != 2 
			|| zend_get_parameters_ex(2, &x, &y) == FAILURE )
		WRONG_PARAM_COUNT;

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	SWFVideoStream_setDimension(stream, Z_LVAL_PP(x), Z_LVAL_PP(y));
}
/* }}} */

/* {{{ getNumFrames */
PHP_METHOD(swfvideostream, getnumframes) 
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_LONG(SWFVideoStream_getNumFrames(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */
		
		
static zend_function_entry swfvideostream_functions[] = {
	PHP_ME(swfvideostream, 	__construct,	NULL, 0)
	PHP_ME(swfvideostream, setdimension, NULL, 0)
	PHP_ME(swfvideostream, getnumframes, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

#ifdef HAVE_SWFPREBUILTCLIP
/* {{{ SWFPrebuiltClip */
/* {{{ proto class swfprebuiltclip_init([file])
    Returns a SWFPrebuiltClip object */

PHP_METHOD(swfprebuiltclip, __construct)
{
	zval **zfile = NULL;
	SWFPrebuiltClip clip;
	SWFInput input;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &zfile) == FAILURE)
				WRONG_PARAM_COUNT;
	
			if(Z_TYPE_PP(zfile) != IS_RESOURCE)
   			{
			    convert_to_string_ex(zfile);
			    input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
			    zend_list_addref(zend_list_insert(input, le_swfinputp));
   			}
   			else
			    input = getInput(zfile TSRMLS_CC);
		
			clip = newSWFPrebuiltClip_fromInput(input);
			break;
/* not sure whether this makes sense
   there would have to be a function to add contents
		case 0:
			clip = newSWFPrebuiltClip();
			break; */
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
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
		php_error(E_RECOVERABLE_ERROR, "called object is not an SWFPrebuiltClip!");
                                                                                                                                             
	return (SWFPrebuiltClip)clip;
}

/* }}} */
static zend_function_entry swfprebuiltclip_functions[] = {
	PHP_ME(swfprebuiltclip, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

/* }}} */

/* {{{ SWFMovie
*/
/* {{{ proto void swfmovie::__construct(int version) 
  Creates swfmovie object according to the passed version */
PHP_METHOD(swfmovie, __construct)
{
	zval **version;
	SWFMovie movie;
	int ret;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &version) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(version);
		movie = newSWFMovieWithVersion(Z_LVAL_PP(version));
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFMovie");
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
	zval **label;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &label) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(label);
	SWFMovie_labelFrame(getMovie(getThis() TSRMLS_CC), Z_STRVAL_PP(label));
}
/* }}} */

#ifdef HAVE_SWFMOVIE_NAMEDANCHOR
/* {{{ proto void swfmovie::namedanchor(string name)
*/
PHP_METHOD(swfmovie, namedAnchor)
{
	zval **name;
	
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(name);
	
	SWFMovie_namedAnchor(getMovie(getThis() TSRMLS_CC), Z_STRVAL_PP(name));
}
/* }}} */
#endif

/* {{{ proto void swfmovie::protect([ string pasword])
*/
PHP_METHOD(swfmovie, protect)
{
	zval **zchar;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	switch(ZEND_NUM_ARGS() ) {
		case 0:
			SWFMovie_protect(movie, NULL);
			break;
		case 1:
			if( zend_get_parameters_ex(1, &zchar) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(zchar);
			SWFMovie_protect(movie,Z_STRVAL_PP(zchar));
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
}
/* }}} */


/* {{{ proto object swfmovie::add(object SWFBlock) 
*/
PHP_METHOD(swfmovie, add)
{
	zval **zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);

	/* XXX - SWFMovie_add deals w/ all block types.  Probably will need to add that.. */
	if (Z_OBJCE_PP(zchar) == action_class_entry_ptr) {
		block = (SWFBlock) getAction(*zchar TSRMLS_CC);
	} else {
		block = (SWFBlock) getCharacter(*zchar TSRMLS_CC);
	}

	item = SWFMovie_add(movie, block);

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
	zval **zchar;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zchar);
	item = getDisplayItem(*zchar TSRMLS_CC);
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
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	zval **zlimit = NULL;
	int limit = -1;
	int argc = ZEND_NUM_ARGS();
	int oldval = INT_MIN;
	long out; 

	if(argc) {
		if (zend_get_parameters_ex(1, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);

		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
	}
	oldval = Ming_setSWFCompression(limit);			
	out = SWFMovie_output(movie, &phpByteOutputMethod, NULL);
	if (oldval >= -1 && oldval <= 9) {
		Ming_setSWFCompression(oldval);
	}	
	RETURN_LONG(out);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	zval **zlimit = NULL;
	int limit = -1;
	int argc = ZEND_NUM_ARGS();

	if(argc) {
		if (zend_get_parameters_ex(1, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);

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
	zval **x;
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
	zval **zlimit = NULL;
	int limit = -1;
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
	long out;
#endif
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	php_stream *what;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &x) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 2:
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
		if (zend_get_parameters_ex(2, &x, &zlimit) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(what, php_stream *, x, -1,"File-Handle",php_file_le_stream());
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	out = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, what);
	if (oldval >= -1 && oldval <=9)
		Ming_setSWFCompression(oldval);
	RETURN_LONG(out);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	RETURN_LONG(SWFMovie_output(movie, &phpStreamOutputMethod, what, limit));
#else
	RETURN_LONG(SWFMovie_output(movie, &phpStreamOutputMethod, what));
#endif
}
/* }}} */

/* {{{ proto int swfmovie::save(mixed where [, int compression])
  Saves the movie. 'where' can be stream and the movie will be saved there otherwise it is treated as string and written in file with that name */
PHP_METHOD(swfmovie, save)
{
	zval **x;
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
	zval **zlimit = NULL;
	int limit = -1;
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long retval;
	php_stream *stream;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
		if (zend_get_parameters_ex(2, &x, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
		break;
	default:
		WRONG_PARAM_COUNT;
	}
		  
	if (Z_TYPE_PP(x) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(stream, php_stream *, x, -1,"File-Handle",php_file_le_stream());
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
		RETURN_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream, limit));
#else
		RETVAL_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream));
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
    if(oldval >= -1 && oldval <=9)
		Ming_setSWFCompression(oldval);
#endif
		return;
#endif
	}

	convert_to_string_ex(x);
	stream = php_stream_open_wrapper(Z_STRVAL_PP(x), "wb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}
	
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream, limit);
#else
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream);
#endif
	php_stream_close(stream);
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
    if(oldval >= -1 && oldval <=9)
		Ming_setSWFCompression(oldval);
#endif
    
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto void swfmovie::setBackground(int r, int g, int b)
   Sets background color (r,g,b) */
PHP_METHOD(swfmovie, setBackground)
{
	zval **r, **g, **b;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);
	SWFMovie_setBackground(movie, Z_LVAL_PP(r), Z_LVAL_PP(g), Z_LVAL_PP(b));
}
/* }}} */

/* {{{ proto void swfmovie::setRate(float rate)
   Sets movie rate */
PHP_METHOD(swfmovie, setRate)
{
	zval **rate;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &rate) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(rate);
	SWFMovie_setRate(movie, FLOAT_Z_DVAL_PP(rate));
}
/* }}} */

/* {{{ proto void swfmovie::setDimension(float x, float y)
   Sets movie dimension */
PHP_METHOD(swfmovie, setDimension)
{
	zval **x, **y;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFMovie_setDimension(movie, FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfmovie::setFrames(int frames)
   Sets number of frames */
PHP_METHOD(swfmovie, setFrames)
{
	zval **frames;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &frames) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(frames);
	SWFMovie_setNumberOfFrames(movie, Z_LVAL_PP(frames));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto int swfmovie::streamMP3(mixed file [, float skip])
   Sets sound stream of the SWF movie. The parameter can be stream or string. Retuens the number of frames. */
PHP_METHOD(swfmovie, streamMP3)
{
	zval **zfile, **zskip;
	float skip;
	SWFSoundStream sound;
	SWFInput input;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &zfile) == FAILURE)
				WRONG_PARAM_COUNT;
			skip = 0;
			break;
		case 2:
			if(zend_get_parameters_ex(2, &zfile, &zskip) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_double_ex(zskip);
			skip = Z_DVAL_PP(zskip);
			break;
		default:		
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
	
	sound = newSWFSoundStream_fromInput(input);
	SWFMovie_setSoundStreamAt(movie, sound, skip);
	RETURN_LONG(SWFSoundStream_getFrames(sound));
}
/* }}} */

/* {{{ swfmovie_addexport */

PHP_METHOD(swfmovie, addExport)
{
	zval **zchar, **zname;
	SWFBlock block;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &zname) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zchar);
	convert_to_string_ex(zname);

	block = (SWFBlock)getCharacter(*zchar TSRMLS_CC);

	SWFMovie_addExport(movie, block,  Z_STRVAL_PP(zname));
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
	zval **zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

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

/* {{{ void swfmovie_stopsound */

PHP_METHOD(swfmovie, stopSound)
{
	zval **zsound;
	SWFSound sound;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	SWFMovie_stopSound(movie, sound);
}
/* }}} */

/* {{{ void swfmovie_importChar */

PHP_METHOD(swfmovie, importChar)
{
	SWFMovie movie;
	SWFCharacter res;
	int ret;
	zval **libswf, **name;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &libswf, &name) == FAILURE)
		WRONG_PARAM_COUNT;
	convert_to_string_ex(libswf);
	convert_to_string_ex(name);
	movie = getMovie(getThis() TSRMLS_CC);
	res = SWFMovie_importCharacter(movie, Z_STRVAL_PP(libswf), Z_STRVAL_PP(name));

	if(res != NULL)
	{
		/* try and create a sprite object */
    	ret = zend_list_insert(res, le_swfspritep);
		object_init_ex(return_value, sprite_class_entry_ptr);
		add_property_resource(return_value, "sprite", ret);
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
	zval **libswf, **name;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &libswf, &name) == FAILURE)
		WRONG_PARAM_COUNT;
	convert_to_string_ex(libswf);
	convert_to_string_ex(name);
	movie = getMovie(getThis() TSRMLS_CC);
	PHP_MING_FILE_CHK(Z_STRVAL_PP(libswf));
	res = SWFMovie_importFont(movie, Z_STRVAL_PP(libswf), Z_STRVAL_PP(name));

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
	zval **zfont;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfont) == FAILURE)
	    WRONG_PARAM_COUNT;

	convert_to_object_ex(zfont);

	movie = getMovie(getThis() TSRMLS_CC);
	font = getFont(*zfont TSRMLS_CC);
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
	PHP_ME(swfmovie, addExport,         NULL, 0)
	PHP_ME(swfmovie, writeExports,      NULL, 0)
	PHP_ME(swfmovie, startSound,        NULL, 0)
	PHP_ME(swfmovie, stopSound,         NULL, 0)
	PHP_ME(swfmovie, importChar,        NULL, 0)
	PHP_ME(swfmovie, importFont,        NULL, 0)
	PHP_ME(swfmovie, addFont,           NULL, 0)
	PHP_ME(swfmovie, protect,           NULL, 0)
#endif
#ifdef HAVE_SWFMOVIE_NAMEDANCHOR
	PHP_ME(swfmovie, namedAnchor,       NULL, 0)
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFShape");
	}

	return (SWFShape)shape;
}
/* }}} */

/* {{{ proto void swfshape::setline(int width, int r, int g, int b [, int a])
   Sets the current line style for this SWFShape */
PHP_METHOD(swfshape, setLine)
{
	zval **w, **r, **g, **b, **a;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &w, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 5) {
		if (zend_get_parameters_ex(5, &w, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
	} else if (ZEND_NUM_ARGS() == 1) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC), 0, 0, 0, 0, 0);
		return;
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(w);
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	if (ZEND_NUM_ARGS() == 4) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC),
			(unsigned short)Z_LVAL_PP(w), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);
	} else {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC),
			(unsigned short)Z_LVAL_PP(w), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);
	}
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
		zval **arg1;
		unsigned char flags = 0;

		if (ZEND_NUM_ARGS() == 2) {
			zval **arg2;
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			flags = (unsigned char)Z_LVAL_PP(arg2);
		} else {
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
		}

		convert_to_object_ex(arg1);

		if (Z_OBJCE_PP(arg1) == gradient_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_LINEAR_GRADIENT;
			}
			fill = SWFShape_addGradientFill(getShape(getThis() TSRMLS_CC), getGradient(*arg1 TSRMLS_CC), flags);
		} else if (Z_OBJCE_PP(arg1) == bitmap_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_TILED_BITMAP;
			}
			fill = SWFShape_addBitmapFill(getShape(getThis() TSRMLS_CC), getBitmap(*arg1 TSRMLS_CC), flags);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Argument is not a bitmap nor a gradient");
		}

	} else if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		/* it's a solid fill */
		zval **r, **g, **b, **za;
		byte a = 0xff;

		if (ZEND_NUM_ARGS() == 3) {
			if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
		} else if (ZEND_NUM_ARGS() == 4) {
			if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(za);
			a = BYTE_Z_LVAL_PP(za);
		} else {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), a
		);
	} else {
		WRONG_PARAM_COUNT;
	}
	
	if (!fill) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Error adding fill to shape");
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
	zval **zfill, **r, **g, **b, **a;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), 
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);

	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		convert_to_long_ex(a);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);

	} else if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfill) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		
		if (Z_LVAL_PP(zfill) != 0) {
			convert_to_object_ex(zfill);
			fill = getFill(*zfill TSRMLS_CC);
		} else {
			fill = NULL;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	SWFShape_setLeftFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::setleftfill(int arg1 [, int g ,int b [,int a]])
   Sets the right side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting right side fill type. */
PHP_METHOD(swfshape, setRightFill)
{
	zval **zfill, **r, **g, **b, **a;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);

	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		convert_to_long_ex(a);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);
	} else if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfill) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		if (Z_LVAL_PP(zfill) != 0) {
			convert_to_object_ex(zfill);
			fill = getFill(*zfill TSRMLS_CC);
		} else {
			fill = NULL;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	SWFShape_setRightFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::movepento(float x, float y)
   Moves the pen to shape coordinates (x, y) */
PHP_METHOD(swfshape, movePenTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_movePenTo(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::movepen(float x, float y)
   Moves the pen from its current location by vector (x, y) */
PHP_METHOD(swfshape, movePen)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_movePen(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawlineto(float x, float y)
   Draws a line from the current pen position to shape coordinates (x, y) in the current line style */
PHP_METHOD(swfshape, drawLineTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_drawLineTo(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawline(float dx, float dy)
   Draws a line from the current pen position (x, y) to the point (x+dx, y+dy) in the current line style */
PHP_METHOD(swfshape, drawLine)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_drawLine(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawcurveto(float ax, float ay, float bx, float by [, float dx, float dy])
   Draws a curve from the current pen position (x,y) to the point (bx, by) in the current line style, using point (ax, ay) as a control point. Or draws a cubic bezier to point (dx, dy) with control points (ax, ay) and (bx, by) */
PHP_METHOD(swfshape, drawCurveTo)
{
	if (ZEND_NUM_ARGS() == 4) {
		zval **cx, **cy, **ax, **ay;

		if (zend_get_parameters_ex(4, &cx, &cy, &ax, &ay) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(ax);
		convert_to_double_ex(ay);

		SWFShape_drawCurveTo(getShape(getThis() TSRMLS_CC),
			FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(ax), FLOAT_Z_DVAL_PP(ay)
		);
	
	} else if (ZEND_NUM_ARGS() == 6) {
		zval **bx, **by, **cx, **cy, **dx, **dy;

		if (zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(bx);
		convert_to_double_ex(by);
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(dx);
		convert_to_double_ex(dy);

		RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), 
			FLOAT_Z_DVAL_PP(bx), FLOAT_Z_DVAL_PP(by), FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(dx), FLOAT_Z_DVAL_PP(dy))
		);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawcurve(float adx, float ady, float bdx, float bdy [, float cdx, float cdy])
   Draws a curve from the current pen position (x, y) to the point (x+bdx, y+bdy) in the current line style, using point (x+adx, y+ady) as a control point or draws a cubic bezier to point (x+cdx, x+cdy) with control points (x+adx, y+ady) and (x+bdx, y+bdy) */
PHP_METHOD(swfshape, drawCurve)
{
	if (ZEND_NUM_ARGS() == 4) {
		zval **cx, **cy, **ax, **ay;

		if (zend_get_parameters_ex(4, &cx, &cy, &ax, &ay) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(ax);
		convert_to_double_ex(ay);

		SWFShape_drawCurve(getShape(getThis() TSRMLS_CC), 
			FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(ax), FLOAT_Z_DVAL_PP(ay)
		);

	} else if (ZEND_NUM_ARGS() == 6) {
		zval **bx, **by, **cx, **cy, **dx, **dy;

		if (zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(bx);
		convert_to_double_ex(by);
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(dx);
		convert_to_double_ex(dy);

		RETURN_LONG( SWFShape_drawCubic(getShape(getThis() TSRMLS_CC),
			 FLOAT_Z_DVAL_PP(bx), FLOAT_Z_DVAL_PP(by), FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(dx), FLOAT_Z_DVAL_PP(dy))
		);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawglyph(SWFFont font, string character [, int size])
   Draws the first character in the given string into the shape using the glyph definition from the given font */
PHP_METHOD(swfshape, drawGlyph)
{
	zval **font, **c, **zsize;
	int size=0;

	if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &font, &c) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		size = (int)(1024.0f/Ming_getScale());

	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &font, &c, &zsize) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(zsize);
		size = Z_LVAL_PP(zsize);
	}
	convert_to_string_ex(c);
	SWFShape_drawSizedGlyph(getShape(getThis() TSRMLS_CC), getFont(*font TSRMLS_CC), Z_STRVAL_PP(c)[0], size);
}
/* }}} */

/* {{{ proto void swfshape::drawcircle(float r)
   Draws a circle of radius r centered at the current location, in a counter-clockwise fashion */
PHP_METHOD(swfshape, drawCircle)
{
	zval **r;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &r) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	SWFShape_drawCircle(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(r));
}
/* }}} */

/* {{{ proto void swfshape::drawarc(float r, float startAngle, float endAngle)
   Draws an arc of radius r centered at the current location, from angle startAngle to angle endAngle measured clockwise from 12 o'clock */
PHP_METHOD(swfshape, drawArc)
{
	zval **r, **start, **end;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &r, &start, &end) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(start);
	convert_to_double_ex(end);

	/* convert angles to radians, since that's what php uses elsewhere */
	SWFShape_drawArc(getShape(getThis() TSRMLS_CC), FLOAT_Z_DVAL_PP(r), FLOAT_Z_DVAL_PP(start), FLOAT_Z_DVAL_PP(end));
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(float bx, float by, float cx, float cy, float dx, float dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubic)
{
	zval **bx, **by, **cx, **cy, **dx, **dy;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(bx);
	convert_to_double_ex(by);
	convert_to_double_ex(cx);
	convert_to_double_ex(cy);
	convert_to_double_ex(dx);
	convert_to_double_ex(dy);

	RETURN_LONG( SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), 
		FLOAT_Z_DVAL_PP(bx), FLOAT_Z_DVAL_PP(by), FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(dx), FLOAT_Z_DVAL_PP(dy))
	);
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(float bx, float by, float cx, float cy, float dx, float dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubicTo)
{
	zval **bx, **by, **cx, **cy, **dx, **dy;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(bx);
	convert_to_double_ex(by);
	convert_to_double_ex(cx);
	convert_to_double_ex(cy);
	convert_to_double_ex(dx);
	convert_to_double_ex(dy);

	RETURN_LONG( SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC),
		FLOAT_Z_DVAL_PP(bx), FLOAT_Z_DVAL_PP(by), FLOAT_Z_DVAL_PP(cx), FLOAT_Z_DVAL_PP(cy), FLOAT_Z_DVAL_PP(dx), FLOAT_Z_DVAL_PP(dy))
	);
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFSprite");
	}
	return (SWFMovieClip)sprite;
}
/* }}} */

/* {{{ proto object swfsprite::add(object SWFCharacter)
   Adds the character to the sprite, returns a displayitem object */
PHP_METHOD(swfsprite, add)
{
	zval **zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);

	if (Z_OBJCE_PP(zchar) == action_class_entry_ptr) {
		block = (SWFBlock)getAction(*zchar TSRMLS_CC);
	} else {
		block = (SWFBlock)getCharacter(*zchar TSRMLS_CC);
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
	zval **zchar;
	SWFDisplayItem item;
	SWFMovieClip movie = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zchar);
	item = getDisplayItem(*zchar TSRMLS_CC);
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
	zval **label;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &label) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(label);
	SWFMovieClip_labelFrame(getSprite(getThis() TSRMLS_CC), Z_STRVAL_PP(label));
}
/* }}} */

/* {{{ proto void swfsprite::setFrames(int frames)
   Sets the number of frames in this SWFSprite */
PHP_METHOD(swfsprite, setFrames)
{
	zval **frames;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &frames) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frames);
	SWFMovieClip_setNumberOfFrames(sprite, Z_LVAL_PP(frames));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ SWFSoundInstance swfsprite_startsound */

PHP_METHOD(swfsprite, startSound)
{
	zval **zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

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
	zval **zsound;
	SWFSound sound;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	SWFMovieClip_stopSound(sprite, sound);
}
/* }}} */
#endif

static zend_function_entry swfsprite_functions[] = {
	PHP_ME(swfsprite, __construct,  	NULL, 0)
	PHP_ME(swfsprite, add,				NULL, 0)
	PHP_ME(swfsprite, remove,			NULL, 0)
	PHP_ME(swfsprite, nextFrame,		NULL, 0)
	PHP_ME(swfsprite, labelFrame,		NULL, 0)
	PHP_ME(swfsprite, setFrames,		NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfsprite, startSound,		NULL, 0)
	PHP_ME(swfsprite, stopSound,		NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFText
*/
/* {{{ proto void swftext::__construct()
   Creates new SWFText object */
PHP_METHOD(swftext, __construct)
{
	SWFText text = newSWFText2();
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFText");
	}
	return (SWFText)text;
}
/* }}} */

/* {{{ proto void swftext::setFont(object font)
   Sets this SWFText object's current font to given font */
PHP_METHOD(swftext, setFont)
{
	zval **zfont;
	SWFText text = getText(getThis() TSRMLS_CC);
	SWFFont font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfont) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zfont);
	font = getFont(*zfont TSRMLS_CC);
	SWFText_setFont(text, font);
}
/* }}} */

/* {{{ proto void swftext::setHeight(float height)
   Sets this SWFText object's current height to given height */
PHP_METHOD(swftext, setHeight)
{
	zval **height;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	SWFText_setHeight(text, FLOAT_Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftext::setSpacing(float spacing)
   Sets this SWFText object's current letterspacing to given spacing */
PHP_METHOD(swftext, setSpacing)
{
	zval **spacing;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &spacing) == FAILURE) {
 		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(spacing);
	SWFText_setSpacing(text, FLOAT_Z_DVAL_PP(spacing));
}
/* }}} */

/* {{{ proto void swftext::setColor(int r, int g, int b [, int a])
   Sets this SWFText object's current color to the given color */
PHP_METHOD(swftext, setColor)
{
	zval **r, **g, **b, **a;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	if (ZEND_NUM_ARGS() == 4) {
		SWFText_setColor(text, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a));
	} else {
		SWFText_setColor(text, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff);
	}
}
/* }}} */

/* {{{ proto void swftext::moveTo(float x, float y)
   Moves this SWFText object's current pen position to (x, y) in text coordinates */
PHP_METHOD(swftext, moveTo)
{
	zval **x, **y;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFText_setXY(text, FLOAT_Z_DVAL_PP(x), FLOAT_Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swftext::addString(string text)
   Writes the given text into this SWFText object at the current pen position, using the current font, height, spacing, and color */
PHP_METHOD(swftext, addString)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s);
	SWFText_addString(text, Z_STRVAL_PP(s), NULL);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swftext::addUTF8String(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */

PHP_METHOD(swftext, addUTF8String)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(s);

	SWFText_addUTF8String(text, Z_STRVAL_PP(s), NULL);
}

/* }}} */
/* {{{ proto void swftext::addWideString(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */
/*
PHP_METHOD(swftext, addWideString)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(s);

	SWFText_addWideString(text, Z_STRVAL_PP(s), NULL);
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getWidth(string str)
   Calculates the width of the given string in this text objects current font and size */
PHP_METHOD(swftext, getWidth)
{
	zval **zstring;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zstring);
	RETURN_DOUBLE(SWFText_getStringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring)));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto double swftext::getUTF8Width(string)
   calculates the width of the given string in this text objects current font and size */

PHP_METHOD(swftext, getUTF8Width)
{
	zval **zstring;
	int width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFText_getUTF8StringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}

/* }}} */
/* {{{ proto double swftext::getWideWidth(string)
   calculates the width of the given string in this text objects current font and size */
/*
PHP_METHOD(swftext, getWideWidth)
{
	zval **zstring;
	int width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFText_getWideStringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
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
	zval **flags;
	SWFTextField field = newSWFTextField();
	int ret = zend_list_insert(field, le_swftextfieldp);

	object_init_ex(getThis(), textfield_class_entry_ptr);
	add_property_resource(getThis(), "textfield", ret);
	zend_list_addref(ret);

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &flags) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(flags);
		SWFTextField_setFlags(field, Z_LVAL_PP(flags));
	}
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Called object is not an SWFTextField");
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
	zval **font;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &font) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(font);
	SWFTextField_setFont(field, getFontOrFontChar(*font TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swftextfield::setBounds(float width, float height)
   Sets the width and height of this textfield */
PHP_METHOD(swftextfield, setBounds)
{
	zval **width, **height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &width, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(width);
	convert_to_double_ex(height);
	SWFTextField_setBounds(field, FLOAT_Z_DVAL_PP(width), FLOAT_Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftextfield::align(int alignment)
   Sets the alignment of this textfield */
PHP_METHOD(swftextfield, align)
{
	zval **align;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &align) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(align);
	SWFTextField_setAlignment(field, Z_LVAL_PP(align));
}
/* }}} */

/* {{{ proto void swftextfield::setHeight(float height)
   Sets the font height of this textfield */
PHP_METHOD(swftextfield, setHeight)
{
	zval **height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	SWFTextField_setHeight(field, FLOAT_Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftextfield::setLeftMargin(float margin)
   Sets the left margin of this textfield */
PHP_METHOD(swftextfield, setLeftMargin)
{
	zval **margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &margin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(margin);
	SWFTextField_setLeftMargin(field, FLOAT_Z_DVAL_PP(margin));
}
/* }}} */

/* {{{ proto void swftextfield::setRightMargin(float margin)
   Sets the right margin of this textfield */
PHP_METHOD(swftextfield, setRightMargin)
{
	zval **margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &margin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(margin);
	SWFTextField_setRightMargin(field, FLOAT_Z_DVAL_PP(margin));
}
/* }}} */

/* {{{ proto void swftextfield::setMargins(float left, float right)
   Sets both margins of this textfield */
PHP_METHOD(swftextfield, setMargins)
{
	zval **left, **right;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(left);
	convert_to_double_ex(right);
	SWFTextField_setLeftMargin(field, FLOAT_Z_DVAL_PP(left));
	SWFTextField_setRightMargin(field, FLOAT_Z_DVAL_PP(right));
}
/* }}} */

/* {{{ proto void swftextfield::setIndentation(float indentation)
   Sets the indentation of the first line of this textfield */
PHP_METHOD(swftextfield, setIndentation)
{
	zval **indent;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &indent) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(indent);
	SWFTextField_setIndentation(field, FLOAT_Z_DVAL_PP(indent));
}
/* }}} */

/* {{{ proto void swftextfield::setLineSpacing(float space)
   Sets the line spacing of this textfield */
PHP_METHOD(swftextfield, setLineSpacing)
{
	zval **spacing;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &spacing) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(spacing);
	SWFTextField_setLineSpacing(field, FLOAT_Z_DVAL_PP(spacing));
}
/* }}} */

/* {{{ proto void swftextfield::setColor(int r, int g, int b [, int a])
   Sets the color of this textfield */
PHP_METHOD(swftextfield, setColor)
{
	zval **r, **g, **b, **a;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);
	byte alpha = 0xff;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
		alpha = BYTE_Z_LVAL_PP(a);
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);
	SWFTextField_setColor(field, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), (byte)alpha);
}
/* }}} */

/* {{{ proto void swftextfield::setName(string var_name)
   Sets the variable name of this textfield */
PHP_METHOD(swftextfield, setName)
{
	zval **name;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	SWFTextField_setVariableName(field, Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swftextfield::addString(string str)
   Adds the given string to this textfield */
PHP_METHOD(swftextfield, addString)
{
	zval **string;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(string);
	SWFTextField_addString(field, Z_STRVAL_PP(string));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swftextfield::setPadding(float padding)
   Sets the padding of this textfield */
PHP_METHOD(swftextfield, setPadding)
{
	zval **padding;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &padding) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_double_ex(padding);

	SWFTextField_setPadding(field, Z_DVAL_PP(padding));
}
/* }}} */

/* {{{ proto void swftextfield::addChars(string)
   adds characters to a font that will be available within a textfield */
PHP_METHOD(swftextfield, addChars)
{
	zval **zstring;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFTextField_addChars(field, Z_STRVAL_PP(zstring));

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
#ifdef HAVE_NEW_MING
	PHP_ME(swftextfield, setPadding,      NULL, 0)
	PHP_ME(swftextfield, addChars,        NULL, 0)
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

/* {{{ todo PHP_MINIT_FUNCTION(ming)
*/

/* custom error handler propagates ming errors up to php */
static void php_ming_error(const char *msg, ...)
{
	va_list args;
	char *buffer;
	
	TSRMLS_FETCH();

	va_start(args, msg);
	vspprintf(&buffer, 0, msg, args);
	va_end(args);

	php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "%s", buffer);
	efree(buffer);
}

PHP_RINIT_FUNCTION(ming)
{
	/* XXX - this didn't work so well last I tried.. */

	if (Ming_init() != 0) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Error initializing Ming module");
		return FAILURE;
	}
	return SUCCESS;
}

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
	zend_class_entry sprite_class_entry;
	zend_class_entry sound_class_entry;
#ifdef HAVE_NEW_MING
	zend_class_entry fontchar_class_entry;
	zend_class_entry soundinstance_class_entry;
	zend_class_entry videostream_class_entry;
#endif
#ifdef HAVE_SWFPREBUILTCLIP
	zend_class_entry prebuiltclip_class_entry;
#endif

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
	CONSTANT("SWFTEXTFIELD_HASLENGTH",      SWFTEXTFIELD_HASLENGTH);
	CONSTANT("SWFTEXTFIELD_NOEDIT",         SWFTEXTFIELD_NOEDIT);
	CONSTANT("SWFTEXTFIELD_PASSWORD",       SWFTEXTFIELD_PASSWORD);
	CONSTANT("SWFTEXTFIELD_MULTILINE",      SWFTEXTFIELD_MULTILINE);
	CONSTANT("SWFTEXTFIELD_WORDWRAP",       SWFTEXTFIELD_WORDWRAP);
	CONSTANT("SWFTEXTFIELD_DRAWBOX",        SWFTEXTFIELD_DRAWBOX);
	CONSTANT("SWFTEXTFIELD_NOSELECT",       SWFTEXTFIELD_NOSELECT);
	CONSTANT("SWFTEXTFIELD_HTML",           SWFTEXTFIELD_HTML);
	CONSTANT("SWFTEXTFIELD_USEFONT",        SWFTEXTFIELD_USEFONT);
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
	le_swfspritep = zend_register_list_destructors_ex(destroy_SWFSprite_resource, NULL, "SWFSprite", module_number);
	le_swfinputp = zend_register_list_destructors_ex(destroy_SWFInput_resource, NULL, "SWFInput", module_number);

	le_swfsoundp = zend_register_list_destructors_ex(destroy_SWFSound_resource, NULL, "SWFSound", module_number);
#ifdef HAVE_NEW_MING
	le_swffontcharp = zend_register_list_destructors_ex(destroy_SWFFontCharacter_resource, NULL, "SWFFontCharacter", module_number);
	le_swfsoundinstancep = zend_register_list_destructors_ex(NULL, NULL, "SWFSoundInstance", module_number);
	le_swfvideostreamp = zend_register_list_destructors_ex(destroy_SWFVideoStream_resource, NULL, "SWFVideoStream", module_number);
#endif
#ifdef HAVE_SWFPREBUILTCLIP
	le_swfprebuiltclipp = zend_register_list_destructors_ex(destroy_SWFPrebuiltClip_resource, NULL, "SWFPrebuiltClip", module_number);
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
	INIT_CLASS_ENTRY(sprite_class_entry, "SWFSprite", swfsprite_functions);
	INIT_CLASS_ENTRY(sound_class_entry, "SWFSound", swfsound_functions);
#ifdef HAVE_NEW_MING
	INIT_CLASS_ENTRY(fontchar_class_entry, "SWFFontChar", swffontchar_functions);
	INIT_CLASS_ENTRY(soundinstance_class_entry, "SWFSoundInstance", swfsoundinstance_functions);
	INIT_CLASS_ENTRY(videostream_class_entry, "SWFVideoStream", swfvideostream_functions);
#endif
#ifdef HAVE_SWFPREBUILTCLIP
	INIT_CLASS_ENTRY(prebuiltclip_class_entry, "SWFPrebuiltClip", swfprebuiltclip_functions);
#endif

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
	sprite_class_entry_ptr = zend_register_internal_class(&sprite_class_entry TSRMLS_CC);
	sound_class_entry_ptr = zend_register_internal_class(&sound_class_entry TSRMLS_CC);
#ifdef HAVE_NEW_MING
	fontchar_class_entry_ptr = zend_register_internal_class(&fontchar_class_entry TSRMLS_CC);
	soundinstance_class_entry_ptr = zend_register_internal_class(&soundinstance_class_entry TSRMLS_CC);
	videostream_class_entry_ptr = zend_register_internal_class(&videostream_class_entry TSRMLS_CC);
#endif
#ifdef HAVE_SWFPREBUILTCLIP
	prebuiltclip_class_entry_ptr = zend_register_internal_class(&prebuiltclip_class_entry TSRMLS_CC);
#endif

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
