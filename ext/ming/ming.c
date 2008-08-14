/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
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
	if (php_check_open_basedir((file) TSRMLS_CC)) {	\
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

/* {{{ proto void ming_setscale(int scale)
   Set scale (?) */
PHP_FUNCTION(ming_setScale)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}

	Ming_setScale(num);
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
#endif

#ifdef HAVE_MING_SETSWFCOMPRESSION
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
	char *script;
	int script_len, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &script, &script_len) == FAILURE) {
		return;
	}
	
	action = compileSWFActionCode(script);

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

static const zend_function_entry swfaction_functions[] = {
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
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|Z", &zfile, &zmask) == FAILURE) {
		return;
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
	if (zend_parse_parameters_none() == FAILURE) {
	    return;
	}
	RETURN_DOUBLE(SWFBitmap_getWidth(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swfbitmap::getHeight()
   Returns the height of this bitmap */
PHP_METHOD(swfbitmap, getHeight)
{
	if (zend_parse_parameters_none() == FAILURE) {
	    return;
	}
	RETURN_DOUBLE(SWFBitmap_getHeight(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

static const zend_function_entry swfbitmap_functions[] = {
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
	zval *zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zchar) == FAILURE) {
		return;
	}

	character = getCharacter(zchar TSRMLS_CC);
	SWFButton_addShape(button, character, SWFBUTTONRECORD_HITSTATE);
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
	SWFButton_addShape(button, character, SWFBUTTONRECORD_OVERSTATE);
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
	SWFButton_addShape(button, character, SWFBUTTONRECORD_UPSTATE);
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
	SWFButton_addShape(button, character, SWFBUTTONRECORD_DOWNSTATE);
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
	SWFButton_addShape(button, character, (byte) flags);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swfbutton::setMenu(int flag)
	enable track as menu button behaviour */

PHP_METHOD(swfbutton, setMenu)
{
	long zflag;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &zflag) == FAILURE) {
		return;
	}

	SWFButton_setMenu(button, zflag);
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
	int key_len;
	char c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
		return;
	}

	if (key_len > 1) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Only one character expected");
	}
	
	c = key[0];
	RETURN_LONG((c&0x7f)<<9);
}
/* }}} */

static const zend_function_entry swfbutton_functions[] = {
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
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_moveTo(getDisplayItem(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::move(float dx, float dy)
   Displaces this SWFDisplayItem by (dx, dy) in movie coordinates */
PHP_METHOD(swfdisplayitem, move)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_move(getDisplayItem(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::scaleTo(float xScale [, float yScale])
   Scales this SWFDisplayItem by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swfdisplayitem, scaleTo)
{
	double x, y;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "d|d", &x, &y) == FAILURE) {
		return;
	}
	
	y = argc == 1 ? x : y;
	
	SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::scale(float xScale, float yScale)
   Multiplies this SWFDisplayItem's current x scale by xScale, its y scale by yScale */
PHP_METHOD(swfdisplayitem, scale)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_scale(getDisplayItem(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotateTo(float degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its original orientation */
PHP_METHOD(swfdisplayitem, rotateTo)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}

	SWFDisplayItem_rotateTo(getDisplayItem(getThis() TSRMLS_CC), (float) degrees);
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotate(float degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its current orientation */
PHP_METHOD(swfdisplayitem, rotate)
{
	double degrees;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &degrees) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_rotate(getDisplayItem(getThis() TSRMLS_CC), (float) degrees);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewXTo(float xSkew)
   Sets this SWFDisplayItem's x skew value to xSkew */
PHP_METHOD(swfdisplayitem, skewXTo)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", x) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewXTo(getDisplayItem(getThis() TSRMLS_CC), (float) x);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewX(float xSkew)
   Adds xSkew to this SWFDisplayItem's x skew value */
PHP_METHOD(swfdisplayitem, skewX)
{
	double x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &x) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewX(getDisplayItem(getThis() TSRMLS_CC), (float) x);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewYTo(float ySkew)
   Sets this SWFDisplayItem's y skew value to ySkew */
PHP_METHOD(swfdisplayitem, skewYTo)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewYTo(getDisplayItem(getThis() TSRMLS_CC), (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewY(float ySkew)
   Adds ySkew to this SWFDisplayItem's y skew value */
PHP_METHOD(swfdisplayitem, skewY)
{
	double y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &y) == FAILURE) {
		return;
	}
	
	SWFDisplayItem_skewY(getDisplayItem(getThis() TSRMLS_CC), (float) y);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMatrix(float a, float b, float c, float d, float x, float y)
   Sets the item's transform matrix */
PHP_METHOD(swfdisplayitem, setMatrix)
{
	double a, b, c, d, x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &a, &b, &c, &d, &x, &y) == FAILURE) {
		return;
	}

	SWFDisplayItem_setMatrix(getDisplayItem(getThis() TSRMLS_CC), (float) a, (float) b, (float) c, (float) d, (float) x, (float) y);
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

	SWFDisplayItem_setRatio(getDisplayItem(getThis() TSRMLS_CC), (float) ratio);
}
/* }}} */

/* {{{ proto void swfdisplayitem::addColor(int r, int g, int b [, int a])
   Sets the add color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 0 */
PHP_METHOD(swfdisplayitem, addColor)
{
	long r, g, b, za;
	int a = 0, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "lll|l", &r, &g, &b, &za) == FAILURE) {
		return;
	}
	
	if (argc > 3) {
		a = za;
	}

	SWFDisplayItem_setColorAdd(getDisplayItem(getThis() TSRMLS_CC), r, g, b, a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::multColor(float r, float g, float b [, float a])
   Sets the multiply color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 1.0 */
PHP_METHOD(swfdisplayitem, multColor)
{
	double r, g, b, za;
	float a = 1.0f;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ddd|d", &r, &g, &b, &za) == FAILURE) { 
		return;
	}
	
	if (argc == 4) {
		a = za;
	}

	SWFDisplayItem_setColorMult(getDisplayItem(getThis() TSRMLS_CC), (float) r, (float) g, (float) b, a);
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	SWFDisplayItem_endMask(item);
}
/* }}} */

/* {{{ swfdisplayitem_getX */

PHP_METHOD(swfdisplayitem, getX)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_x(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getY */

PHP_METHOD(swfdisplayitem, getY)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_y(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getXScale */

PHP_METHOD(swfdisplayitem, getXScale)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_xScale(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getYScale */

PHP_METHOD(swfdisplayitem, getYScale)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_yScale(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getXSkew */

PHP_METHOD(swfdisplayitem, getXSkew)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_xSkew(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getYSkew */

PHP_METHOD(swfdisplayitem, getYSkew)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_ySkew(item);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ swfdisplayitem_getRot */

PHP_METHOD(swfdisplayitem, getRot)
{
	float ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	ret = SWFDisplayItem_get_rot(item);
	RETURN_DOUBLE(ret);
}
/* }}} */
#endif

static const zend_function_entry swfdisplayitem_functions[] = {
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
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	
	SWFFill_moveTo(getFill(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swffill::scaleTo(float xScale [, float yScale])
   Scales this SWFFill by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swffill, scaleTo)
{
	double x, y;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "d|d", &x, &y) == FAILURE) {
		return;
	}
	
	y = argc == 1 ? x : y;
	
	SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), (float) x, (float) y);
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
	
	SWFFill_rotateTo(getFill(getThis() TSRMLS_CC), (float) degrees);
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

	SWFFill_skewXTo(getFill(getThis() TSRMLS_CC), (float) x);
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

	SWFFill_skewYTo(getFill(getThis() TSRMLS_CC), (float) y);
}
/* }}} */
static const zend_function_entry swffill_functions[] = {
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
	char *zstring;
	int zstring_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	SWFFontCharacter_addChars(getFontCharacter(getThis() TSRMLS_CC), zstring);
}
/* }}} */

/* {{{ proto void swffontchar::addChars(string)
   adds characters to a font for exporting font */

PHP_METHOD(swffontchar, addUTF8Chars)
{
	char *zstring;
	int zstring_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	SWFFontCharacter_addUTF8Chars(getFontCharacter(getThis() TSRMLS_CC), zstring);
}
/* }}} */

static const zend_function_entry swffontchar_functions[] = {
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
	char *zfile;
	SWFFont font;
	int zfile_len, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zfile, &zfile_len) == FAILURE) {
		return;
	}

	if (strcmp(zfile + zfile_len - 4, ".fdb") == 0) {
		php_stream * stream;
		FILE * file;
	
		stream = php_stream_open_wrapper(zfile, "rb", REPORT_ERRORS, NULL);

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
		PHP_MING_FILE_CHK(zfile);
		font = (SWFFont)newSWFBrowserFont(zfile);
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
	char *zstring;
	int zstring_len;
	float width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}
	
	width = SWFFont_getStringWidth(getFont(getThis() TSRMLS_CC), zstring);
	RETURN_DOUBLE(width);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto int swffont::getUTF8Width(string)
   Calculates the width of the given string in this font at full height */

PHP_METHOD(swffont, getUTF8Width)
{
	char *zstring;
	int zstring_len;
	float width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	width = SWFFont_getUTF8StringWidth(getFont(getThis() TSRMLS_CC), zstring);

	RETURN_DOUBLE(width);
}

/* }}} */
/* not sure about 0 bytes !!!!!!!!! */
/* {{{ proto int swffont::getWideWidth(string)
   Calculates the width of the given string in this font at full height */
/*
PHP_METHOD(swffont, getWideWidth)
{
	char *zstring;
	int zstring_len;
	float width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	width = SWFFont_getWideStringWidth(getFont(getThis() TSRMLS_CC), zstring);

	RETURN_DOUBLE(width);
}
*/
/* }}} */
#endif

/* {{{ proto float swffont::getAscent()
   Returns the ascent of the font, or 0 if not available */
PHP_METHOD(swffont, getAscent)
{
    if (zend_parse_parameters_none() == FAILURE) {
	    return;
	}
	RETURN_DOUBLE(SWFFont_getAscent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getDescent()
   Returns the descent of the font, or 0 if not available */
PHP_METHOD(swffont, getDescent)
{
    if (zend_parse_parameters_none() == FAILURE) {
	    return;
	}
	RETURN_DOUBLE(SWFFont_getDescent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getLeading()
   Returns the leading of the font, or 0 if not available */
PHP_METHOD(swffont, getLeading)
{
    if (zend_parse_parameters_none() == FAILURE) {
	    return;
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
	char *zstring;
	int zstring_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	SWFFont_addChars(getFont(getThis() TSRMLS_CC), zstring);
}
*/
/* }}} */

/* {{{ proto string swffont::getShape(long code)
   Returns the glyph shape of a char as a text string */
PHP_METHOD(swffont, getShape)
{
	long zcode;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &zcode) == FAILURE) {
		return;
	}

	result = SWFFont_getShape(getFont(getThis() TSRMLS_CC), zcode);
	RETVAL_STRING(result, 1);
	free(result);
}
/* }}} */

#endif

static const zend_function_entry swffont_functions[] = {
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
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	double ratio;
	long r, g, b, za = 0;
	byte a = 0xff;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "dlll|l", &ratio, &r, &g, &b, &za) == FAILURE) {
		return;
	}
	
	if (argc > 4) {
		a = (byte) za;
	}

	SWFGradient_addEntry( getGradient(getThis() TSRMLS_CC), (float) ratio, (byte) r, (byte) g, (byte) b, a);
}
/* }}} */

static const zend_function_entry swfgradient_functions[] = {
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
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object_init_ex(return_value, shape_class_entry_ptr);
	add_property_resource(return_value, "shape", ret);
	zend_list_addref(ret);
}
/* }}} */

static const zend_function_entry swfmorph_functions[] = {
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
/* {{{ proto void swfsound::__construct(mixed file [, int flags ])
   Creates a new SWFSound object from given file */
PHP_METHOD(swfsound, __construct)
{
	zval **zfile;
	long flags = 0;
	SWFSound sound;
	SWFInput input;
	int ret, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Z|l", &zfile, &flags) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}

#ifdef HAVE_NEW_MING
	sound = newSWFSound_fromInput(input, (int) flags);
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

static const zend_function_entry swfsound_functions[] = {
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	SWFSoundInstance_setNoMultiple(inst);
}
/* }}} */

/* {{{ swfsoundinstance_loopinpoint(int point) */

PHP_METHOD(swfsoundinstance, loopInPoint)
{
	long zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &zpoint) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopInPoint(inst, zpoint);
}

/* }}} */
/* {{{ swfsoundinstance_loopoutpoint(int point) */

PHP_METHOD(swfsoundinstance, loopOutPoint)
{
	long zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &zpoint) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopOutPoint(inst, zpoint);
}
/* }}} */

/* {{{ swfsoundinstance_loopcount(point) */

PHP_METHOD(swfsoundinstance, loopCount)
{
	long zcount;

	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &zcount) == FAILURE) {
		return;
	}

	SWFSoundInstance_setLoopCount(inst, zcount);
}
/* }}} */

static const zend_function_entry swfsoundinstance_functions[] = {
	PHP_ME(swfsoundinstance, noMultiple,    NULL, 0)
	PHP_ME(swfsoundinstance, loopInPoint,   NULL, 0)
	PHP_ME(swfsoundinstance, loopOutPoint,  NULL, 0)
	PHP_ME(swfsoundinstance, loopCount,     NULL, 0)
	{ NULL, NULL, NULL }
};

/* {{{ SWFVideoStream */

/* {{{ proto void swfvideostream_init([mixed file])
   Returns a SWVideoStream object */
PHP_METHOD(swfvideostream, __construct)
{
	zval **zfile = NULL;
	SWFVideoStream stream;
	SWFInput input;
	int ret, argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|Z", &zfile) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		stream = newSWFVideoStream();
	} else {
		if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
			convert_to_string_ex(zfile);
			input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
			zend_list_addref(zend_list_insert(input, le_swfinputp));
  		} else{ 
			input = getInput(zfile TSRMLS_CC);		
			stream = newSWFVideoStream_fromInput(input);
		}		
	}
	
	if (stream) {
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
	long x, y;
	SWFVideoStream stream = getVideoStream(getThis() TSRMLS_CC);
	
	if (!stream) {
		 php_error(E_RECOVERABLE_ERROR, "getVideoSTream returned NULL");
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &x, &y) == FAILURE) {
		return;
	}

	SWFVideoStream_setDimension(stream, x, y);
}
/* }}} */

/* {{{ getNumFrames */
PHP_METHOD(swfvideostream, getnumframes) 
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(SWFVideoStream_getNumFrames(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */
		
		
static const zend_function_entry swfvideostream_functions[] = {
	PHP_ME(swfvideostream, 	__construct,	NULL, 0)
	PHP_ME(swfvideostream, setdimension, NULL, 0)
	PHP_ME(swfvideostream, getnumframes, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

#ifdef HAVE_SWFPREBUILTCLIP
/* {{{ SWFPrebuiltClip */
/* {{{ proto void swfprebuiltclip_init([mixed file])
    Returns a SWFPrebuiltClip object */
PHP_METHOD(swfprebuiltclip, __construct)
{
	zval **zfile;
	SWFPrebuiltClip clip;
	SWFInput input;
	int ret;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &zfile) == FAILURE) {
		return;
	}

	/* About void parameter:
	 * 	not sure whether this makes sense
	 *  there would have to be a function to add contents
	 *     clip = newSWFPrebuiltClip();
	 */
			
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
		
	clip = newSWFPrebuiltClip_fromInput(input);
	
	if (clip) {
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
static const zend_function_entry swfprebuiltclip_functions[] = {
	PHP_ME(swfprebuiltclip, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

/* }}} */

/* {{{ SWFMovie
*/
/* {{{ proto void swfmovie::__construct([int version]) 
  Creates swfmovie object according to the passed version */
PHP_METHOD(swfmovie, __construct)
{
	long version;
	SWFMovie movie;
	int ret, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &version) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		movie = newSWFMovie(); /* default version 4 */
	} else {
		movie = newSWFMovieWithVersion(version);
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
	char *label;
	int label_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &label, &label_len) == FAILURE) {
		return;
	}

	SWFMovie_labelFrame(getMovie(getThis() TSRMLS_CC), label);
}
/* }}} */

#ifdef HAVE_SWFMOVIE_NAMEDANCHOR
/* {{{ proto void swfmovie::namedanchor(string name)
*/
PHP_METHOD(swfmovie, namedAnchor)
{
	char *name;
	int name_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}
	
	SWFMovie_namedAnchor(getMovie(getThis() TSRMLS_CC), name);
}
/* }}} */
#endif

/* {{{ proto void swfmovie::protect([ string pasword])
*/
PHP_METHOD(swfmovie, protect)
{
	char *zchar;
	int zchar_len, argc = ZEND_NUM_ARGS();
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|s", &zchar, &zchar_len) == FAILURE) {
		return;
	}

	if (argc == 0) {
		SWFMovie_protect(movie, NULL);
	} else {
		SWFMovie_protect(movie, zchar);
	}
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
	} else {
		block = (SWFBlock) getCharacter(zchar TSRMLS_CC);
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

static void phpByteOutputMethod(byte b, void *data)
{
	TSRMLS_FETCH();

	php_write(&b, 1 TSRMLS_CC);
}

/* {{{ proto int swfmovie::output([int compression])
*/
PHP_METHOD(swfmovie, output)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	int argc = ZEND_NUM_ARGS();
	long zlimit = NULL;
	int limit = -1;	
	int oldval = INT_MIN;
	long out;
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &zlimit) == FAILURE) {
		return;
	}
	
	if (argc > 0) {
		limit = zlimit;

		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING, "compression level must be within 0..9");
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
	int argc = ZEND_NUM_ARGS();
	zval **zlimit = NULL;
	int limit = -1;

	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &zlimit) == FAILURE) {
		return;
	}

	if (argc) {
		limit = zlimit;

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

static void phpStreamOutputMethod(byte b, void * data)
{
	TSRMLS_FETCH();

	php_stream_write((php_stream*)data, &b, 1);
}

/* I'm not sure about the logic here as Ming_setSWFCompression() should be
 * used with current Ming. I'll have to add some tests to the test suite to
 * verify this functionallity before I can say for sure
 */

/* {{{ proto int swfmovie::saveToFile(stream x [, int compression])
*/
PHP_METHOD(swfmovie, saveToFile)
{
	zval *x;
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
	long limit = -1;
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
	long out;
#endif
	int argc = ZEND_NUM_ARGS();
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	php_stream *what;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|l", &x, &limit) == FAILURE) {
		return;
	}
	
	if (argc > 1) {
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
		if (limit < 0 || limit > 9) {
			php_error(E_WARNING, "compression level must be within 0..9");
			RETURN_FALSE;
		}
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression((int) limit);
#endif
	}

	ZEND_FETCH_RESOURCE(what, php_stream *, &x, -1, "File-Handle", php_file_le_stream());

#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	out = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, what);
	if (oldval >= -1 && oldval <= 9) {
		Ming_setSWFCompression(oldval);
	}
	RETURN_LONG(out);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	RETURN_LONG(SWFMovie_output(movie, &phpStreamOutputMethod, what, (int) limit));
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
	long limit = -1;
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long retval;
	php_stream *stream;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Z|l", &x, &limit) == FAILURE) {
		return;
	}

	if (argc > 1) {
#if defined(HAVE_MING_ZLIB) || defined(HAVE_NEW_MING)
		if (limit < 0 || limit > 9) {
			php_error(E_WARNING, "compression level must be within 0..9");
			RETURN_FALSE;
		}
#endif
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
	}
		  
	if (Z_TYPE_PP(x) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(stream, php_stream *, x, -1, "File-Handle", php_file_le_stream());

#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
		RETURN_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream, (int) limit));
#else
		RETVAL_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream));
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
    	if (oldval >= -1 && oldval <=9) {
			Ming_setSWFCompression(oldval);
		}
#endif
		return;
#endif
	}

	convert_to_string_ex(x);
	stream = php_stream_open_wrapper(Z_STRVAL_PP(x), "wb", REPORT_ERRORS, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}
	
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream, (int) limit);
#else
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream);
#endif
	php_stream_close(stream);
#if defined(HAVE_MING_ZLIB) && !defined(HAVE_NEW_MING)
    if (oldval >= -1 && oldval <= 9) {
		Ming_setSWFCompression(oldval);
	}
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

	SWFMovie_setRate(movie, (float) rate);
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

	SWFMovie_setDimension(movie, (float) x, (float) y);
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
/* {{{ proto int swfmovie::streamMP3(mixed file [, float skip])
   Sets sound stream of the SWF movie. The parameter can be stream or string. Retuens the number of frames. */
PHP_METHOD(swfmovie, streamMP3)
{
	zval **zfile;
	double zskip = 0;
	SWFSoundStream sound;
	SWFInput input;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Z|d", &zfile, &zskip) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
	
	sound = newSWFSoundStream_fromInput(input);
	SWFMovie_setSoundStreamAt(movie, sound, (float) zskip);
	RETURN_LONG(SWFSoundStream_getFrames(sound));
}
/* }}} */

/* {{{ swfmovie_addexport */

PHP_METHOD(swfmovie, addExport)
{
	zval *zchar;
	char *zname;
	int zname_len;
	SWFBlock block;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &zchar, &zname, &zname_len) == FAILURE) {
		return;
	}

	block = (SWFBlock)getCharacter(zchar TSRMLS_CC);

	SWFMovie_addExport(movie, block, zname);
}

/* }}} */

/* {{{ swfmovie_writeexports */
			
PHP_METHOD(swfmovie, writeExports)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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

	if (item != NULL) {
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

/* {{{ void swfmovie_importChar */

PHP_METHOD(swfmovie, importChar)
{
	SWFMovie movie;
	SWFCharacter res;
	int ret;
	char *libswf, *name;
	int libswf_len, name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &libswf, &libswf_len, &name, &name_len) == FAILURE) {
		return;
	}
	
	movie = getMovie(getThis() TSRMLS_CC);
	res = SWFMovie_importCharacter(movie, libswf, name);

	if (res != NULL) {
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
	char *libswf, *name;
	int libswf_len, name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &libswf, &libswf_len, &name, &name_len) == FAILURE) {
		return;
	}
	
	movie = getMovie(getThis() TSRMLS_CC);
	PHP_MING_FILE_CHK(libswf);
	res = SWFMovie_importFont(movie, libswf, name);

	if (res != NULL) {
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

	if (res != NULL)	{
		/* try and create a fontchar object */
    	ret = zend_list_insert(res, le_swffontcharp);
		object_init_ex(return_value, fontchar_class_entry_ptr);
		add_property_resource(return_value, "fontcharacter", ret);
		zend_list_addref(ret);
	}	
}
/* }}} */
#endif

static const zend_function_entry swfmovie_functions[] = {
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
	long w, r = 0, g = 0, b = 0, a = 0xff;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "l|llll", &w, &r, &g, &b, &a) == FAILURE) {
		return;
	}
	
	if (argc == 1) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC), 0, 0, 0, 0, 0);
	} else if (argc > 3) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC),	(unsigned short) w, (byte) r, (byte) g, (byte) b, (byte) a);		
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto object swfshape::addfill(mixed arg1 [, int arg2 [, int b [, int a]]])
   Returns a fill object, for use with swfshape_setleftfill and swfshape_setrightfill. If 1 or 2 parameter(s) is (are) passed first should be object (from gradient class) and the second int (flags). Gradient fill is performed. If 3 or 4 parameters are passed : r, g, b [, a]. Solid fill is performed. */
PHP_METHOD(swfshape, addFill)
{
	zval **arg1;
	long arg2 = 0, g = 0, b = 0;
	SWFFill fill=NULL;
	int ret, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Z|lll", &arg1, &arg2, &g, &b) == FAILURE) {
		return;
	}
	
	if (argc == 1 || argc == 2) {
		/* it's a gradient or bitmap */		
		unsigned char flags = 0;

		convert_to_object_ex(arg1);
		
		if (argc == 2) {
			flags = (unsigned char) arg2;
		}

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
	} else if (argc == 3 || argc == 4) {
		/* it's a solid fill */
		byte a = 0xff;

		if (argc == 4) {
			a = (byte) b;
		}

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte) arg2, (byte) g, (byte) b, a);
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

/* {{{ proto void swfshape::setleftfill(int r [, int g ,int b [,int a]])
   Sets the left side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting left side fill type. */
PHP_METHOD(swfshape, setLeftFill)
{
	zval **r;
	long g, b, a = 0xff;
	SWFFill fill;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Z|lll",  &r, &g, &b, &a) == FAILURE) {
		return;
	}
		
	convert_to_long_ex(r);

	if (argc == 1) {	
		if (Z_LVAL_PP(r) != 0) {
			convert_to_object_ex(r);

			fill = getFill(*r TSRMLS_CC);
		} else {
			fill = NULL;
		}
	} else if (argc == 4) {
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte) Z_LVAL_PP(r), (byte) g, (byte) b, (byte) a);
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
	zval **zfill;
	long g = 0, b = 0, a = 0xff;
	SWFFill fill;
	int argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "Z|lll", &zfill, &g, &b, &a) == FAILURE) {
		return;
	}

	if (argc == 1) {
		if (Z_LVAL_PP(zfill) != 0) {
			convert_to_object_ex(zfill);
			fill = getFill(*zfill TSRMLS_CC);
		} else {
			fill = NULL;
		}		
	} else if (argc == 3 || argc == 4) {
		convert_to_long_ex(zfill);
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), (byte) Z_LVAL_PP(zfill), (byte) g, (byte) b, (byte) a);
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
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}
	
	SWFShape_movePenTo(getShape(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfshape::movepen(float x, float y)
   Moves the pen from its current location by vector (x, y) */
PHP_METHOD(swfshape, movePen)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_movePen(getShape(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfshape::drawlineto(float x, float y)
   Draws a line from the current pen position to shape coordinates (x, y) in the current line style */
PHP_METHOD(swfshape, drawLineTo)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_drawLineTo(getShape(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfshape::drawline(float dx, float dy)
   Draws a line from the current pen position (x, y) to the point (x+dx, y+dy) in the current line style */
PHP_METHOD(swfshape, drawLine)
{
	double x, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
		return;
	}

	SWFShape_drawLine(getShape(getThis() TSRMLS_CC), (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swfshape::drawcurveto(float ax, float ay, float bx, float by [, float dx, float dy])
   Draws a curve from the current pen position (x,y) to the point (bx, by) in the current line style, using point (ax, ay) as a control point. Or draws a cubic bezier to point (dx, dy) with control points (ax, ay) and (bx, by) */
PHP_METHOD(swfshape, drawCurveTo)
{
	double a1, a2, a3, a4, a5, a6;
	int argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "dddd|dd", &a1, &a2, &a3, &a4, &a5, &a6) == FAILURE) {
		return;
	}
	
	if (argc == 4) {
		SWFShape_drawCurveTo(getShape(getThis() TSRMLS_CC), (float) a1, (float) a2, (float) a3, (float) a4);
	} else if (argc == 6) {
		RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), (float) a1, (float) a2, (float) a3, (float) a4, (float) a5, (float) a6));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawcurve(float adx, float ady, float bdx, float bdy [, float cdx, float cdy])
   Draws a curve from the current pen position (x, y) to the point (x+bdx, y+bdy) in the current line style, using point (x+adx, y+ady) as a control point or draws a cubic bezier to point (x+cdx, x+cdy) with control points (x+adx, y+ady) and (x+bdx, y+bdy) */
PHP_METHOD(swfshape, drawCurve)
{
	double a1, a2, a3, a4, a5, a6;
	int argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "dddd|dd", &a1, &a2, &a3, &a4, &a5, &a6) == FAILURE) {
		return;
	}
	
	if (argc == 4) {
		SWFShape_drawCurve(getShape(getThis() TSRMLS_CC), (float) a1, (float) a2, (float) a3, (float) a4);
	} else if (ZEND_NUM_ARGS() == 6) {
		RETURN_LONG(SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), (float) a1, (float) a2, (float) a3, (float) a4, (float) a5, (float) a6));
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
	long zsize;
	int c_len, argc = ZEND_NUM_ARGS();
	int size = 1024.0f / Ming_getScale();

	if (zend_parse_parameters(argc TSRMLS_CC, "os|l", &font, &c, &c_len, &zsize) == FAILURE) {
		return;
	}
	
	if (argc == 3) {
		size = zsize;
	}
	
	SWFShape_drawSizedGlyph(getShape(getThis() TSRMLS_CC), getFont(font TSRMLS_CC), c[0], size);
}
/* }}} */

/* {{{ proto void swfshape::drawcircle(float r)
   Draws a circle of radius r centered at the current location, in a counter-clockwise fashion */
PHP_METHOD(swfshape, drawCircle)
{
	double r;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &r) == FAILURE) {
		return;
	}

	SWFShape_drawCircle(getShape(getThis() TSRMLS_CC), (float) r);
}
/* }}} */

/* {{{ proto void swfshape::drawarc(float r, float startAngle, float endAngle)
   Draws an arc of radius r centered at the current location, from angle startAngle to angle endAngle measured clockwise from 12 o'clock */
PHP_METHOD(swfshape, drawArc)
{
	double r, start, end;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ddd", &r, &start, &end) == FAILURE) {
		return;
	}

	/* convert angles to radians, since that's what php uses elsewhere */
	SWFShape_drawArc(getShape(getThis() TSRMLS_CC), (float) r, (float) start, (float) end);
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(float bx, float by, float cx, float cy, float dx, float dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubic)
{
	double bx, by, cx, cy, dx, dy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		return;
	}

	RETURN_LONG(SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), (float) bx, (float) by, (float) cx, (float) cy, (float) dx, (float) dy));
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(float bx, float by, float cx, float cy, float dx, float dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubicTo)
{
	double bx, by, cx, cy, dx, dy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddddd", &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		return;
	}

	RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), (float) bx, (float) by, (float) cx, (float) cy, (float) dx, (float) dy));
}
/* }}} */

static const zend_function_entry swfshape_functions[] = {
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
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	SWFMovieClip_nextFrame(getSprite(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfsprite::labelFrame(string label)
   Labels frame */
PHP_METHOD(swfsprite, labelFrame)
{
	char *label;
	int label_len;

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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &frames) == FAILURE) {
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

	if (item != NULL) {
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
#endif

static const zend_function_entry swfsprite_functions[] = {
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
	long height;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &height) == FAILURE) {
		return;
	}
	
	SWFText_setHeight(text, (float) height);
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

	SWFText_setSpacing(text, (float) spacing);
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

	SWFText_setColor(text, (byte) r, (byte) g, (byte) b, (byte) a);
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

	SWFText_setXY(text, (float) x, (float) y);
}
/* }}} */

/* {{{ proto void swftext::addString(string text)
   Writes the given text into this SWFText object at the current pen position, using the current font, height, spacing, and color */
PHP_METHOD(swftext, addString)
{
	char *s;
	int s_len;
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
	int s_len;
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
	int s_len;
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
	char *zstring;
	int zstring_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	RETURN_DOUBLE(SWFText_getStringWidth(getText(getThis() TSRMLS_CC), zstring));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto double swftext::getUTF8Width(string)
   calculates the width of the given string in this text objects current font and size */

PHP_METHOD(swftext, getUTF8Width)
{
	char *zstring;
	int zstring_len, width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	width = SWFText_getUTF8StringWidth(getText(getThis() TSRMLS_CC), zstring);

	RETURN_DOUBLE(width);
}

/* }}} */
/* {{{ proto double swftext::getWideWidth(string)
   calculates the width of the given string in this text objects current font and size */
/*
PHP_METHOD(swftext, getWideWidth)
{
	char *zstring;
	int zstring_len, width;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	width = SWFText_getWideStringWidth(getText(getThis() TSRMLS_CC), zstring);

	RETURN_DOUBLE(width);
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getAscent()
   Returns the ascent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getAscent)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_DOUBLE(SWFText_getAscent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getDescent()
   Returns the descent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getDescent)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
  RETURN_DOUBLE(SWFText_getDescent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getLeading()
   Returns the leading of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getLeading)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
  RETURN_DOUBLE(SWFText_getLeading(getText(getThis() TSRMLS_CC)));
}
/* }}} */

static const zend_function_entry swftext_functions[] = {
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
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &flags) == FAILURE) {
		return;
	}

	object_init_ex(getThis(), textfield_class_entry_ptr);
	add_property_resource(getThis(), "textfield", ret);
	zend_list_addref(ret);

	if (argc) {
		SWFTextField_setFlags(field, flags);
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
	zval *font;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &font) == FAILURE) {
		return;
	}

	SWFTextField_setFont(field, getFontOrFontChar(font TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swftextfield::setBounds(float width, float height)
   Sets the width and height of this textfield */
PHP_METHOD(swftextfield, setBounds)
{
	double width, height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &width, &height) == FAILURE) {
		return;
	}

	SWFTextField_setBounds(field, (float) width, (float) height);
}
/* }}} */

/* {{{ proto void swftextfield::align(int alignment)
   Sets the alignment of this textfield */
PHP_METHOD(swftextfield, align)
{
	long align;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &align) == FAILURE) {
		return;
	}

	SWFTextField_setAlignment(field, align);
}
/* }}} */

/* {{{ proto void swftextfield::setHeight(float height)
   Sets the font height of this textfield */
PHP_METHOD(swftextfield, setHeight)
{
	double height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &height) == FAILURE) {
		return;
	}

	SWFTextField_setHeight(field, (float) height);
}
/* }}} */

/* {{{ proto void swftextfield::setLeftMargin(float margin)
   Sets the left margin of this textfield */
PHP_METHOD(swftextfield, setLeftMargin)
{
	double margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &margin) == FAILURE) {
		return;
	}

	SWFTextField_setLeftMargin(field, (float) margin);
}
/* }}} */

/* {{{ proto void swftextfield::setRightMargin(float margin)
   Sets the right margin of this textfield */
PHP_METHOD(swftextfield, setRightMargin)
{
	double margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &margin) == FAILURE) {
		return;
	}

	SWFTextField_setRightMargin(field, margin);
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

	SWFTextField_setLeftMargin(field, (float) left);
	SWFTextField_setRightMargin(field, (float) right);
}
/* }}} */

/* {{{ proto void swftextfield::setIndentation(float indentation)
   Sets the indentation of the first line of this textfield */
PHP_METHOD(swftextfield, setIndentation)
{
	double indent;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &indent) == FAILURE) {
		return;
	}

	SWFTextField_setIndentation(field, (float) indent);
}
/* }}} */

/* {{{ proto void swftextfield::setLineSpacing(float space)
   Sets the line spacing of this textfield */
PHP_METHOD(swftextfield, setLineSpacing)
{
	double spacing;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &spacing) == FAILURE) {
		return;
	}

	SWFTextField_setLineSpacing(field, (float) spacing);
}
/* }}} */

/* {{{ proto void swftextfield::setColor(int r, int g, int b [, int a])
   Sets the color of this textfield */
PHP_METHOD(swftextfield, setColor)
{
	long r, g, b, a = 0xff;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &r, &g, &b, &a) == FAILURE) {
		return;
	}
	
	SWFTextField_setColor(field, (byte) r, (byte) g, (byte) b, (byte) a);
}
/* }}} */

/* {{{ proto void swftextfield::setName(string var_name)
   Sets the variable name of this textfield */
PHP_METHOD(swftextfield, setName)
{
	char *name;
	int name_len;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	SWFTextField_setVariableName(field, name);
}
/* }}} */

/* {{{ proto void swftextfield::addString(string str)
   Adds the given string to this textfield */
PHP_METHOD(swftextfield, addString)
{
	char *string;
	int string_len;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	SWFTextField_addString(field, string);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swftextfield::setPadding(float padding)
   Sets the padding of this textfield */
PHP_METHOD(swftextfield, setPadding)
{
	double padding;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &padding) == FAILURE) {
		return;
	}

	SWFTextField_setPadding(field, padding);
}
/* }}} */

/* {{{ proto void swftextfield::addChars(string)
   adds characters to a font that will be available within a textfield */
PHP_METHOD(swftextfield, addChars)
{
	char *zstring;
	int zstring_len;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zstring, &zstring_len) == FAILURE) {
		return;
	}

	SWFTextField_addChars(field, zstring);
}
/* }}} */
#endif

static const zend_function_entry swftextfield_functions[] = {
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
