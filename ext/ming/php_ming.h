/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: dave@opaque.net                                              |
   +----------------------------------------------------------------------+
*/

#ifndef _PHP_MING_H
#define _PHP_MING_H

#if HAVE_MING

extern zend_module_entry ming_module_entry;
#define ming_module_ptr &ming_module_entry

#include <ming.h>

PHP_RINIT_FUNCTION(ming);
PHP_MINIT_FUNCTION(ming);
PHP_MINFO_FUNCTION(ming);

PHP_FUNCTION(swfbitmap_init);
PHP_FUNCTION(swfbitmap_getWidth);
PHP_FUNCTION(swfbitmap_getHeight);

PHP_FUNCTION(swffill_init);
PHP_FUNCTION(swffill_moveTo);
PHP_FUNCTION(swffill_scaleTo);
PHP_FUNCTION(swffill_rotateTo);
PHP_FUNCTION(swffill_skewXTo);
PHP_FUNCTION(swffill_skewYTo);

PHP_FUNCTION(swfgradient_init);
PHP_FUNCTION(swfgradient_addEntry);

PHP_FUNCTION(swfshape_init);
PHP_FUNCTION(swfshape_addfill);
PHP_FUNCTION(swfshape_setrightfill);
PHP_FUNCTION(swfshape_setleftfill);
PHP_FUNCTION(swfshape_setline);
PHP_FUNCTION(swfshape_movepento);
PHP_FUNCTION(swfshape_movepen);
PHP_FUNCTION(swfshape_drawlineto);
PHP_FUNCTION(swfshape_drawline);
PHP_FUNCTION(swfshape_drawcurveto);
PHP_FUNCTION(swfshape_drawcurve);
PHP_FUNCTION(swfshape_drawglyph);
PHP_FUNCTION(swfshape_drawarc);
PHP_FUNCTION(swfshape_drawcircle);
PHP_FUNCTION(swfshape_drawcubic);
PHP_FUNCTION(swfshape_drawcubicto);

PHP_FUNCTION(swfmovie_init);
PHP_FUNCTION(swfmovie_output);
PHP_FUNCTION(swfmovie_saveToFile);
PHP_FUNCTION(swfmovie_save);
PHP_FUNCTION(swfmovie_add);
PHP_FUNCTION(swfmovie_remove);
PHP_FUNCTION(swfmovie_nextFrame);
PHP_FUNCTION(swfmovie_labelFrame);
PHP_FUNCTION(swfmovie_setBackground);
PHP_FUNCTION(swfmovie_setRate);
PHP_FUNCTION(swfmovie_setDimension);
PHP_FUNCTION(swfmovie_setFrames);
PHP_FUNCTION(swfmovie_streamMp3);

PHP_FUNCTION(swfsprite_init);
PHP_FUNCTION(swfsprite_add);
PHP_FUNCTION(swfsprite_remove);
PHP_FUNCTION(swfsprite_nextFrame);
PHP_FUNCTION(swfsprite_labelFrame);
PHP_FUNCTION(swfsprite_setFrames);

PHP_FUNCTION(swffont_init);
PHP_FUNCTION(swffont_getWidth);
PHP_FUNCTION(swffont_getAscent);
PHP_FUNCTION(swffont_getDescent);
PHP_FUNCTION(swffont_getLeading);

PHP_FUNCTION(swftext_init);
PHP_FUNCTION(swftext_setFont);
PHP_FUNCTION(swftext_setHeight);
PHP_FUNCTION(swftext_setSpacing);
PHP_FUNCTION(swftext_setColor);
PHP_FUNCTION(swftext_moveTo);
PHP_FUNCTION(swftext_addString);
PHP_FUNCTION(swftext_getWidth);
PHP_FUNCTION(swftext_getAscent);
PHP_FUNCTION(swftext_getDescent);
PHP_FUNCTION(swftext_getLeading);

PHP_FUNCTION(swftextfield_init);
PHP_FUNCTION(swftextfield_setFont);
PHP_FUNCTION(swftextfield_setBounds);
PHP_FUNCTION(swftextfield_align);
PHP_FUNCTION(swftextfield_setHeight);
PHP_FUNCTION(swftextfield_setLeftMargin);
PHP_FUNCTION(swftextfield_setRightMargin);
PHP_FUNCTION(swftextfield_setMargins);
PHP_FUNCTION(swftextfield_setIndentation);
PHP_FUNCTION(swftextfield_setLineSpacing);
PHP_FUNCTION(swftextfield_setColor);
PHP_FUNCTION(swftextfield_setName);
PHP_FUNCTION(swftextfield_addString);

PHP_FUNCTION(swfdisplayitem_move);
PHP_FUNCTION(swfdisplayitem_moveTo);
PHP_FUNCTION(swfdisplayitem_scale);
PHP_FUNCTION(swfdisplayitem_scaleTo);
PHP_FUNCTION(swfdisplayitem_rotate);
PHP_FUNCTION(swfdisplayitem_rotateTo);
PHP_FUNCTION(swfdisplayitem_skewX);
PHP_FUNCTION(swfdisplayitem_skewXTo);
PHP_FUNCTION(swfdisplayitem_skewY);
PHP_FUNCTION(swfdisplayitem_skewYTo);
PHP_FUNCTION(swfdisplayitem_setMatrix);
PHP_FUNCTION(swfdisplayitem_setDepth);
PHP_FUNCTION(swfdisplayitem_setRatio);
PHP_FUNCTION(swfdisplayitem_addColor);
PHP_FUNCTION(swfdisplayitem_multColor);
PHP_FUNCTION(swfdisplayitem_setName);
PHP_FUNCTION(swfdisplayitem_addAction);

PHP_FUNCTION(swfbutton_init);
PHP_FUNCTION(swfbutton_setHit);
PHP_FUNCTION(swfbutton_setOver);
PHP_FUNCTION(swfbutton_setUp);
PHP_FUNCTION(swfbutton_setDown);
PHP_FUNCTION(swfbutton_setAction);
PHP_FUNCTION(swfbutton_addShape);
PHP_FUNCTION(swfbutton_addAction);

PHP_FUNCTION(swfbutton_keypress);

PHP_FUNCTION(swfaction_init);

PHP_FUNCTION(swfmorph_init);
PHP_FUNCTION(swfmorph_getShape1);
PHP_FUNCTION(swfmorph_getShape2);

PHP_FUNCTION(ming_setCubicThreshold);
PHP_FUNCTION(ming_setScale);
PHP_FUNCTION(ming_useSWFVersion);

#else
#define ming_module_ptr NULL
#endif /* HAVE_MING */
#define phpext_ming_ptr ming_module_ptr
#endif  /* _PHP_MING_H */
