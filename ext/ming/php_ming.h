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

#ifndef _PHP_MING_H
#define _PHP_MING_H

#if HAVE_MING

extern zend_module_entry ming_module_entry;
#define ming_module_ptr &ming_module_entry

PHP_RINIT_FUNCTION(ming);
PHP_MINIT_FUNCTION(ming);
PHP_MINFO_FUNCTION(ming);

PHP_METHOD(swfbitmap, init);
PHP_METHOD(swfbitmap, getWidth);
PHP_METHOD(swfbitmap, getHeight);

PHP_METHOD(swfvideostream, init);
PHP_METHOD(swfvideostream, setdimention);
PHP_METHOD(swfvideostream, getnumframes);

PHP_METHOD(swfprebuiltclip, init);

PHP_METHOD(swffill, init);
PHP_METHOD(swffill, moveTo);
PHP_METHOD(swffill, scaleTo);
PHP_METHOD(swffill, rotateTo);
PHP_METHOD(swffill, skewXTo);
PHP_METHOD(swffill, skewYTo);

PHP_METHOD(swfgradient, init);
PHP_METHOD(swfgradient, addEntry);

PHP_METHOD(swfshape, init);
PHP_METHOD(swfshape, addfill);
PHP_METHOD(swfshape, setrightfill);
PHP_METHOD(swfshape, setleftfill);
PHP_METHOD(swfshape, setline);
PHP_METHOD(swfshape, movepento);
PHP_METHOD(swfshape, movepen);
PHP_METHOD(swfshape, drawlineto);
PHP_METHOD(swfshape, drawline);
PHP_METHOD(swfshape, drawcurveto);
PHP_METHOD(swfshape, drawcurve);
PHP_METHOD(swfshape, drawglyph);
PHP_METHOD(swfshape, drawarc);
PHP_METHOD(swfshape, drawcircle);
PHP_METHOD(swfshape, drawcubic);
PHP_METHOD(swfshape, drawcubicto);

/* SWFMovie */

PHP_METHOD(swfmovie, init);
PHP_METHOD(swfmovie, output);
PHP_METHOD(swfmovie, saveToFile);
PHP_METHOD(swfmovie, save);
PHP_METHOD(swfmovie, add);
PHP_METHOD(swfmovie, remove);
PHP_METHOD(swfmovie, nextFrame);
PHP_METHOD(swfmovie, labelFrame);
#ifdef HAVE_SWFMOVIE_NAMEDANCHOR
PHP_METHOD(swfmovie, namedAnchor);
#endif
PHP_METHOD(swfmovie, setBackground);
PHP_METHOD(swfmovie, setRate);
PHP_METHOD(swfmovie, setDimension);
PHP_METHOD(swfmovie, setFrames);

#ifdef HAVE_NEW_MING
PHP_METHOD(swfmovie, protect);
PHP_METHOD(swfmovie, streamMp3);
PHP_METHOD(swfmovie, addExport);
PHP_METHOD(swfmovie, writeExports);
PHP_METHOD(swfmovie, startSound);
PHP_METHOD(swfmovie, stopSound);
PHP_METHOD(swfmovie, importChar);
PHP_METHOD(swfmovie, importFont);
PHP_METHOD(swfmovie, addFont);
# endif

/* SWFSprint */

PHP_METHOD(swfsprite, init);
PHP_METHOD(swfsprite, add);
PHP_METHOD(swfsprite, remove);
PHP_METHOD(swfsprite, nextFrame);
PHP_METHOD(swfsprite, labelFrame);
PHP_METHOD(swfsprite, setFrames);

#ifdef HAVE_NEW_MING
PHP_METHOD(swfsprite, startSound);
PHP_METHOD(swfsprite, stopSound);
#endif

/* SWFFont */

PHP_METHOD(swffont, init);
PHP_METHOD(swffont, getWidth);

#ifdef HAVE_NEW_MING
PHP_METHOD(swffont, getUTF8Width);
/*PHP_METHOD(swffont, getWideWidth);*/
#endif

PHP_METHOD(swffont, getAscent);
PHP_METHOD(swffont, getDescent);
PHP_METHOD(swffont, getLeading);

#ifdef HAVE_NEW_MING
/*PHP_METHOD(swffont, addChars);*/
PHP_METHOD(swffont, getShape);
#endif

PHP_METHOD(swffont, getLeading);

/* SWFText */

PHP_METHOD(swftext, init);
PHP_METHOD(swftext, setFont);
PHP_METHOD(swftext, setHeight);
PHP_METHOD(swftext, setSpacing);
PHP_METHOD(swftext, setColor);
PHP_METHOD(swftext, moveTo);
PHP_METHOD(swftext, addString);

#ifdef HAVE_NEW_MING
PHP_METHOD(swftext, addUTF8String);
/*PHP_METHOD(swftext, addWideString);*/
#endif

PHP_METHOD(swftext, getWidth);

#ifdef HAVE_NEW_MING
PHP_METHOD(swftext, getUTF8Width);
/*PHP_METHOD(swftext, getWideWidth);*/
#endif

PHP_METHOD(swftext, getAscent);
PHP_METHOD(swftext, getDescent);
PHP_METHOD(swftext, getLeading);

/* SWFTextField */

PHP_METHOD(swftextfield, init);
PHP_METHOD(swftextfield, setFont);
PHP_METHOD(swftextfield, setBounds);
PHP_METHOD(swftextfield, align);
PHP_METHOD(swftextfield, setHeight);
PHP_METHOD(swftextfield, setLeftMargin);
PHP_METHOD(swftextfield, setRightMargin);
PHP_METHOD(swftextfield, setMargins);
PHP_METHOD(swftextfield, setIndentation);
PHP_METHOD(swftextfield, setLineSpacing);
PHP_METHOD(swftextfield, setColor);
PHP_METHOD(swftextfield, setName);
PHP_METHOD(swftextfield, addString);
PHP_METHOD(swftextfield, setPadding);
PHP_METHOD(swftextfield, addChars);

/* SWFFontChar */

PHP_METHOD(swffontchar, addChars);
PHP_METHOD(swffontchar, addUTF8Chars);

/* SWFDisplayItem */

PHP_METHOD(swfdisplayitem, move);
PHP_METHOD(swfdisplayitem, moveTo);
PHP_METHOD(swfdisplayitem, scale);
PHP_METHOD(swfdisplayitem, scaleTo);
PHP_METHOD(swfdisplayitem, rotate);
PHP_METHOD(swfdisplayitem, rotateTo);
PHP_METHOD(swfdisplayitem, skewX);
PHP_METHOD(swfdisplayitem, skewXTo);
PHP_METHOD(swfdisplayitem, skewY);
PHP_METHOD(swfdisplayitem, skewYTo);
PHP_METHOD(swfdisplayitem, setMatrix);
PHP_METHOD(swfdisplayitem, setDepth);
PHP_METHOD(swfdisplayitem, setRatio);
PHP_METHOD(swfdisplayitem, addColor);
PHP_METHOD(swfdisplayitem, multColor);
PHP_METHOD(swfdisplayitem, setName);
PHP_METHOD(swfdisplayitem, addAction);

#ifdef HAVE_NEW_MING
PHP_METHOD(swfdisplayitem, remove);
PHP_METHOD(swfdisplayitem, setMaskLevel);
PHP_METHOD(swfdisplayitem, endMask);
PHP_METHOD(swfdisplayitem, getX);
PHP_METHOD(swfdisplayitem, getY);
PHP_METHOD(swfdisplayitem, getXScale);
PHP_METHOD(swfdisplayitem, getYScale);
PHP_METHOD(swfdisplayitem, getXSkew);
PHP_METHOD(swfdisplayitem, getYSkew);
PHP_METHOD(swfdisplayitem, getRot);
#endif

/* SWFButton */

PHP_METHOD(swfbutton, init);
PHP_METHOD(swfbutton, setHit);
PHP_METHOD(swfbutton, setOver);
PHP_METHOD(swfbutton, setUp);
PHP_METHOD(swfbutton, setDown);
PHP_METHOD(swfbutton, setAction);
PHP_METHOD(swfbutton, addShape);

#ifdef HAVE_NEW_MING
PHP_METHOD(swfbutton, setMenu);
#endif

PHP_METHOD(swfbutton, addAction);

#ifdef HAVE_NEW_MING
PHP_METHOD(swfbutton, addSound);
#endif

/* SWFAction */

PHP_METHOD(swfaction, init);

/* SWFMorph */

PHP_METHOD(swfmorph, init);
PHP_METHOD(swfmorph, getShape1);
PHP_METHOD(swfmorph, getShape2);

/* SWFSound */

PHP_METHOD(swfsound, init);

/* SWFSoundInstance */

#ifdef HAVE_NEW_MING
PHP_METHOD(swfsoundinstance, noMultiple);
PHP_METHOD(swfsoundinstance, loopInPoint);
PHP_METHOD(swfsoundinstance, loopOutPoint);
PHP_METHOD(swfsoundinstance, loopCount);
#endif

/* SWF Utils */

PHP_FUNCTION(ming_keypress);
PHP_FUNCTION(ming_setCubicThreshold);
PHP_FUNCTION(ming_setScale);
PHP_FUNCTION(ming_useSWFVersion);

#ifdef HAVE_NEW_MING
PHP_FUNCTION(ming_useConstants);
#endif
#ifdef HAVE_MING_SETSWFCOMPRESSION
PHP_FUNCTION(ming_setSWFCompression);
#endif

#else
#define ming_module_ptr NULL
#endif /* HAVE_MING */
#define phpext_ming_ptr ming_module_ptr
#endif  /* _PHP_MING_H */
