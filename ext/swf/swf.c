/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#include "php.h"

#if HAVE_SWF
#include "swf.h"
#include "ext/standard/info.h"
#include "php_swf.h"

function_entry swf_functions[] = {
	PHP_FE(swf_openfile,		NULL)
	PHP_FE(swf_closefile,		NULL)
	PHP_FE(swf_labelframe,		NULL)
	PHP_FE(swf_showframe,		NULL)
	PHP_FE(swf_setframe,		NULL)
	PHP_FE(swf_getframe,		NULL)
	PHP_FE(swf_mulcolor,		NULL)
	PHP_FE(swf_addcolor,		NULL)
	PHP_FE(swf_placeobject,		NULL)
	PHP_FE(swf_modifyobject,		NULL)
	PHP_FE(swf_removeobject,		NULL)
	PHP_FE(swf_nextid,		NULL)
	PHP_FE(swf_startdoaction,		NULL)
	PHP_FE(swf_enddoaction,		NULL)
	PHP_FE(swf_actiongotoframe,		NULL)
	PHP_FE(swf_actiongeturl,		NULL)
	PHP_FE(swf_actionnextframe,		NULL)
	PHP_FE(swf_actionprevframe,		NULL)
	PHP_FE(swf_actionplay,		NULL)
	PHP_FE(swf_actionstop,		NULL)
	PHP_FE(swf_actiontogglequality,		NULL)
	PHP_FE(swf_actionwaitforframe,		NULL)
	PHP_FE(swf_actionsettarget,		NULL)
	PHP_FE(swf_actiongotolabel,		NULL)
	PHP_FE(swf_defineline,		NULL)
	PHP_FE(swf_definerect,		NULL)
	PHP_FE(swf_startshape,		NULL)
	PHP_FE(swf_shapelinesolid,		NULL)
	PHP_FE(swf_shapefilloff,		NULL)
	PHP_FE(swf_shapefillsolid,		NULL)
	PHP_FE(swf_shapefillbitmapclip,		NULL)
	PHP_FE(swf_shapefillbitmaptile,		NULL)
	PHP_FE(swf_shapemoveto,		NULL)
	PHP_FE(swf_shapelineto,		NULL)
	PHP_FE(swf_shapecurveto,		NULL)
	PHP_FE(swf_shapecurveto3,		NULL)
	PHP_FE(swf_shapearc,		NULL)
	PHP_FE(swf_endshape,		NULL)
	PHP_FE(swf_definefont,		NULL)
	PHP_FE(swf_setfont,		NULL)
	PHP_FE(swf_fontsize,		NULL)
	PHP_FE(swf_fontslant,		NULL)
	PHP_FE(swf_fonttracking,		NULL)
	PHP_FE(swf_getfontinfo,		NULL)
	PHP_FE(swf_definetext,		NULL)
	PHP_FE(swf_textwidth,		NULL)
	PHP_FE(swf_definebitmap,		NULL)
	PHP_FE(swf_getbitmapinfo,		NULL)
	PHP_FE(swf_startsymbol,		NULL)
	PHP_FE(swf_endsymbol,		NULL)
	PHP_FE(swf_startbutton,		NULL)
	PHP_FE(swf_addbuttonrecord,		NULL)
	PHP_FE(swf_oncondition,		NULL)
	PHP_FE(swf_endbutton,		NULL)
	PHP_FE(swf_viewport,		NULL)
	PHP_FE(swf_ortho2,		NULL)
	PHP_FE(swf_perspective,		NULL)
	PHP_FE(swf_polarview,		NULL)
	PHP_FE(swf_lookat,		NULL)
	PHP_FE(swf_pushmatrix,		NULL)
	PHP_FE(swf_popmatrix,		NULL)
	PHP_FE(swf_scale,		NULL)
	PHP_FE(swf_translate,		NULL)
	PHP_FE(swf_rotate,		NULL)
	PHP_FE(swf_posround,		NULL)
	{NULL,NULL,NULL}
};

zend_module_entry swf_module_entry = {
	"swf",
	swf_functions,
	PHP_MINIT(swf),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(swf),
	STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
DLEXPORT zend_module_entry *get_module() { return &swf_module_entry; };
#endif

PHP_MINFO_FUNCTION(swf)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "swf support", "enabled");
	php_info_print_table_end();
}


PHP_MINIT_FUNCTION(swf)
{
	REGISTER_LONG_CONSTANT("TYPE_PUSHBUTTON", TYPE_PUSHBUTTON, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPE_MENUBUTTON", TYPE_MENUBUTTON, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSHitTest", BSHitTest, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSDown", BSDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSOver", BSOver, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSUp", BSUp, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverDowntoIdle", OverDowntoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IdletoOverDown", IdletoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OutDowntoIdle", OutDowntoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OutDowntoOverDown", OutDowntoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverDowntoOutDown", OverDowntoOutDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverUptoOverDown", OverUptoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverUptoIdle", OverUptoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IdletoOverUp", IdletoOverUp, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ButtonEnter", ButtonEnter, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ButtonExit", ButtonExit, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MenuEnter", MenuEnter, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MenuExit", MenuExit, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_FUNCTION(swf_openfile)
{
	zval **name, **sizeX, **sizeY, **frameRate, **r, **g, **b;
	if (ARG_COUNT(ht) != 7 ||
	    zend_get_parameters_ex(7, &name, &sizeX, &sizeY, &frameRate, &r, &g, &b) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(name);
	convert_to_double_ex(sizeX);
	convert_to_double_ex(sizeY);
	convert_to_double_ex(frameRate);
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	
	swf_openfile((*name)->value.str.val, (float)(*sizeX)->value.dval, (float)(*sizeY)->value.dval,
	       		 (float)(*frameRate)->value.dval, (float)(*r)->value.dval, (float)(*g)->value.dval, (float)(*b)->value.dval);
}

PHP_FUNCTION(swf_closefile)
{
	swf_closefile();
}

PHP_FUNCTION(swf_labelframe)
{
	zval **name;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &name) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	swf_labelframe((*name)->value.str.val);
}

PHP_FUNCTION(swf_showframe)
{
	swf_showframe();
}

PHP_FUNCTION(swf_setframe)
{
	zval **frameno;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &frameno) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frameno);
	
	swf_setframe((*frameno)->value.lval);
}

PHP_FUNCTION(swf_getframe)
{
	RETURN_LONG(swf_getframe());
}

void col_swf(INTERNAL_FUNCTION_PARAMETERS, int opt) {
	zval **r, **g, **b, **a;
	if (ARG_COUNT(ht) != 4 ||
	    zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	if (opt) {
		swf_addcolor((float)(*r)->value.dval, (float)(*g)->value.dval, (float)(*b)->value.dval, (float)(*a)->value.dval);
	} else {
		swf_mulcolor((float)(*r)->value.dval, (float)(*g)->value.dval, (float)(*b)->value.dval, (float)(*a)->value.dval);
	}
}

PHP_FUNCTION(swf_mulcolor)
{
	col_swf(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(swf_addcolor)
{
	col_swf(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(swf_placeobject)
{
	zval **objid, **depth;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &objid, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_long_ex(depth);
	swf_placeobject((*objid)->value.lval, (*depth)->value.lval);
}

PHP_FUNCTION(swf_modifyobject)
{
	zval **depth, **how;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &depth, &how) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(depth);
	convert_to_long_ex(how);
	
	swf_modifyobject((*depth)->value.lval, (*how)->value.lval);
}

PHP_FUNCTION(swf_removeobject)
{
	zval **depth;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(depth);
	
	swf_removeobject((*depth)->value.lval);
}

PHP_FUNCTION(swf_nextid)
{
	swf_nextid();
}

PHP_FUNCTION(swf_startdoaction)
{
	swf_startdoaction();
}

PHP_FUNCTION(swf_enddoaction)
{
	swf_enddoaction();
}

PHP_FUNCTION(swf_actiongotoframe)
{
	zval **frameno;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &frameno) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frameno);

	swf_actionGotoFrame((*frameno)->value.lval);
}

PHP_FUNCTION(swf_actiongeturl)
{
	zval **url, **target;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &url, &target) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(url);
	convert_to_string_ex(target);
	
	swf_actionGetURL((*url)->value.str.val, (*target)->value.str.val);
}

PHP_FUNCTION(swf_actionnextframe)
{
	swf_actionNextFrame();
}

PHP_FUNCTION(swf_actionprevframe)
{
	swf_actionPrevFrame();
}

PHP_FUNCTION(swf_actionplay)
{
	swf_actionPlay();
}

PHP_FUNCTION(swf_actionstop)
{
	swf_actionStop();
}

PHP_FUNCTION(swf_actiontogglequality)
{
	swf_actionToggleQuality();
}

PHP_FUNCTION(swf_actionwaitforframe)
{
	zval **frame, **skipcount;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &frame, &skipcount) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(frame);
	convert_to_long_ex(skipcount);
	swf_actionWaitForFrame((*frame)->value.lval, (*skipcount)->value.lval);
}

PHP_FUNCTION(swf_actionsettarget)
{
	zval **target;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &target) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(target);
	swf_actionSetTarget((*target)->value.str.val);
}

PHP_FUNCTION(swf_actiongotolabel)
{
	zval **label;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &label) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(label);
	swf_actionGoToLabel((*label)->value.str.val);
}

void php_swf_define(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **objid, **x1, **y1, **x2, **y2, **width;
	if (ARG_COUNT(ht) != 6 ||
	    zend_get_parameters_ex(6, &objid, &x1, &y1, &x2, &y2, &width) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(objid);
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	convert_to_double_ex(width);
	
	if (opt) {
		swf_defineline((*objid)->value.lval, (float)(*x1)->value.dval, (float)(*y1)->value.dval,
	 	               (float)(*x2)->value.dval, (float)(*y2)->value.dval, (float)(*width)->value.dval);
	} else {
		swf_definerect((*objid)->value.lval, (float)(*x1)->value.dval, (float)(*y1)->value.dval,
	 	               (float)(*x2)->value.dval, (float)(*y2)->value.dval, (float)(*width)->value.dval);
	}
}

PHP_FUNCTION(swf_defineline)
{
	php_swf_define(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(swf_definerect)
{
	php_swf_define(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}


PHP_FUNCTION(swf_startshape)
{
	zval **objid;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &objid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	swf_startshape((*objid)->value.lval);
}

PHP_FUNCTION(swf_shapelinesolid)
{
	zval **r, **g, **b, **a, **width;
	if (ARG_COUNT(ht) != 5 ||
	    zend_get_parameters_ex(5, &r, &g, &b, &a, &width) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	convert_to_double_ex(width);
	swf_shapelinesolid((float)(*r)->value.dval, (float)(*g)->value.dval, (float)(*b)->value.dval, (float)(*a)->value.dval,
				   (float)(*width)->value.dval);
}

PHP_FUNCTION(swf_shapefilloff)
{
	swf_shapefilloff();
}

PHP_FUNCTION(swf_shapefillsolid)
{
	zval **r, **g, **b, **a;
	if (ARG_COUNT(ht) != 4 ||
	    zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	
	swf_shapefillsolid((float)(*r)->value.dval, (float)(*g)->value.dval, (float)(*b)->value.dval, (float)(*a)->value.dval);
}

void php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **bitmapid;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &bitmapid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(bitmapid);
	
	if (opt) {
		swf_shapefillbitmapclip((*bitmapid)->value.lval);
	} else {
		swf_shapefillbitmaptile((*bitmapid)->value.lval);
	}
}


PHP_FUNCTION(swf_shapefillbitmapclip)
{
	php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(swf_shapefillbitmaptile)
{
	php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

void php_swf_shape(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **x, **y;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &x, &y) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	
	if (opt) {
		swf_shapemoveto((float)(*x)->value.dval, (float)(*y)->value.dval);
	} else {
		swf_shapelineto((float)(*x)->value.dval, (float)(*y)->value.dval);
	}
}

PHP_FUNCTION(swf_shapemoveto)
{
	php_swf_shape(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(swf_shapelineto)
{
	php_swf_shape(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(swf_shapecurveto)
{
	zval **x1, **y1, **x2, **y2;
	if (ARG_COUNT(ht) != 4 ||
	    zend_get_parameters_ex(4, &x1, &y1, &x2, &y2) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	
	swf_shapecurveto((float)(*x1)->value.dval, (float)(*y1)->value.dval, (float)(*x2)->value.dval, (float)(*y2)->value.dval);
}


PHP_FUNCTION(swf_shapecurveto3)
{
	zval **x1, **y1, **x2, **y2, **x3, **y3;
	if (ARG_COUNT(ht) != 6 ||
	    zend_get_parameters_ex(6, &x1, &y1, &x2, &y2, &x3, &y3) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	convert_to_double_ex(x3);
	convert_to_double_ex(y3);
	
	swf_shapecurveto3((float)(*x1)->value.dval, (float)(*y1)->value.dval, (float)(*x2)->value.dval, (float)(*y2)->value.dval,
				  (float)(*x3)->value.dval, (float)(*y3)->value.dval);
}

PHP_FUNCTION(swf_shapearc)
{
	zval **x, **y, **r, **ang1, **ang2;
	if (ARG_COUNT(ht) != 5 ||
	    zend_get_parameters_ex(5, &x, &y, &r, &ang1, &ang2) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(r);
	convert_to_double_ex(ang1);
	convert_to_double_ex(ang2);
	
	swf_shapearc((float)(*x)->value.dval, (float)(*y)->value.dval, (float)(*r)->value.dval, (float)(*ang1)->value.dval,
	             (float)(*ang2)->value.dval);
}

PHP_FUNCTION(swf_endshape)
{
	swf_endshape();
}

PHP_FUNCTION(swf_definefont)
{
	zval **fontid, **name;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &fontid, &name) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fontid);
	convert_to_string_ex(name);
	
	swf_definefont((*fontid)->value.lval, (*name)->value.str.val);
}

PHP_FUNCTION(swf_setfont)
{
	zval **fontid;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &fontid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fontid);
	swf_setfont((*fontid)->value.lval);
}

PHP_FUNCTION(swf_fontsize)
{
	zval **height;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &height) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	
	swf_fontsize((float)(*height)->value.dval);
}

PHP_FUNCTION(swf_fontslant)
{
	zval **slant;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &slant) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(slant);
	
	swf_fontslant((float)(*slant)->value.dval);
}

PHP_FUNCTION(swf_fonttracking)
{
	zval **track;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &track) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(track);
	swf_fonttracking((float)(*track)->value.dval);
}

PHP_FUNCTION(swf_getfontinfo)
{
	float A_height, x_height;
	swf_getfontinfo(&A_height, &x_height);
	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "Cannot initialize return value from swf_getfontinfo");
		RETURN_FALSE;
	}
	add_assoc_double(return_value, "Aheight", A_height);
	add_assoc_double(return_value, "xheight", x_height);
}

PHP_FUNCTION(swf_definetext)
{
	zval **objid, **str, **docCenter;
	if (ARG_COUNT(ht) != 3 ||
	    zend_get_parameters_ex(3, &objid, &str, &docCenter) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_string_ex(str);
	convert_to_long_ex(docCenter);
	
	swf_definetext((*objid)->value.lval, (*str)->value.str.val, (*docCenter)->value.lval);
}
	
PHP_FUNCTION(swf_textwidth)
{
	zval **str;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &str) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	RETURN_DOUBLE((double)swf_textwidth((*str)->value.str.val));
}

PHP_FUNCTION(swf_definebitmap)
{
	zval **objid, **imgname;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &objid, &imgname) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_string_ex(imgname);
	
	swf_definebitmap((*objid)->value.lval, (*imgname)->value.str.val);
}

PHP_FUNCTION(swf_getbitmapinfo)
{
	zval **bitmapid;
	int size, width, height;
	
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &bitmapid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(bitmapid);
	
	size = swf_getbitmapinfo((*bitmapid)->value.lval, &width, &height);
	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "Cannot initialize return value from swf_getbitmapinfo");
		RETURN_FALSE;
	}
	
	add_assoc_long(return_value, "size", size);
	add_assoc_long(return_value, "width", width);
	add_assoc_long(return_value, "height", height);
}

PHP_FUNCTION(swf_startsymbol)
{
	zval **objid;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &objid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	
	swf_startsymbol((*objid)->value.lval);
}

PHP_FUNCTION(swf_endsymbol)
{
	swf_endsymbol();
}

PHP_FUNCTION(swf_startbutton)
{
	zval **objid, **type;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &objid, &type) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_long_ex(type);
	
	swf_startbutton((*objid)->value.lval, (*type)->value.lval); /* TYPE_MENUBUTTON, TYPE_PUSHBUTTON */
}

PHP_FUNCTION(swf_addbuttonrecord)
{
	zval **state, **objid, **depth;
	if (ARG_COUNT(ht) != 3 ||
	    zend_get_parameters_ex(3, &state, &objid, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(state);
	convert_to_long_ex(objid);
	convert_to_long_ex(depth);
	
	swf_addbuttonrecord((*state)->value.lval, (*objid)->value.lval, (*depth)->value.lval);
}

PHP_FUNCTION(swf_oncondition)
{
	zval **transitions;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &transitions) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(transitions);
	
	swf_oncondition((*transitions)->value.lval);
}

PHP_FUNCTION(swf_endbutton)
{
	swf_endbutton();
}

void php_swf_geo_same(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **arg1, **arg2, **arg3, **arg4;
	if (ARG_COUNT(ht) != 4 ||
	    zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(arg1);
	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);

	if (opt == 0) {
		swf_viewport((*arg1)->value.dval, (*arg2)->value.dval, (*arg3)->value.dval,
		             (*arg4)->value.dval);
	} else if (opt == 1) {
		swf_ortho2((*arg1)->value.dval, (*arg2)->value.dval, (*arg3)->value.dval,
		           (*arg4)->value.dval);
	} else if (opt == 2) {
		swf_polarview((*arg1)->value.dval, (*arg2)->value.dval, (*arg3)->value.dval,
		              (*arg4)->value.dval);
	} else if (opt == 3) {
		swf_perspective((*arg1)->value.dval, (*arg2)->value.dval, (*arg3)->value.dval,
		                (*arg4)->value.dval);
	}
}

PHP_FUNCTION(swf_viewport)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(swf_ortho2)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(swf_polarview)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}

PHP_FUNCTION(swf_perspective)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}

PHP_FUNCTION(swf_lookat)
{
	zval **vx, **vy, **vz, **px, **py, **pz, **twist;
	if (ARG_COUNT(ht) != 7 ||
	    zend_get_parameters_ex(7, &vx, &vy, &vz, &px, &py, &pz, &twist) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(vx);
	convert_to_double_ex(vy);
	convert_to_double_ex(vz);
	convert_to_double_ex(px);
	convert_to_double_ex(py);
	convert_to_double_ex(pz);
	convert_to_double_ex(twist);
	
	swf_lookat((*vx)->value.dval, (*vy)->value.dval, (*vz)->value.dval,
	           (*px)->value.dval, (*py)->value.dval, (*pz)->value.dval, (*twist)->value.dval);
}

PHP_FUNCTION(swf_pushmatrix)
{
	swf_pushmatrix();
}

PHP_FUNCTION(swf_popmatrix)
{
	swf_popmatrix();
}

PHP_FUNCTION(swf_scale)
{
	zval **x, **y, **z;
	if (ARG_COUNT(ht) != 3 ||
	    zend_get_parameters_ex(3, &x, &y, &z) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(z);
	
	swf_scale((*x)->value.dval, (*y)->value.dval, (*z)->value.dval);
}

PHP_FUNCTION(swf_translate)
{
	zval **x, **y, **z;
	if (ARG_COUNT(ht) != 3 ||
	    zend_get_parameters_ex(3, &x, &y, &z) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(z);
	
	swf_translate((*x)->value.dval, (*y)->value.dval, (*z)->value.dval);
}

PHP_FUNCTION(swf_rotate)
{
	zval **angle, **axis;
	if (ARG_COUNT(ht) != 2 ||
	    zend_get_parameters_ex(2, &angle, &axis) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(angle);
	convert_to_string_ex(axis);
	
	swf_rotate((*angle)->value.dval, (*axis)->value.str.val);
}

PHP_FUNCTION(swf_posround)
{
	zval **doit;
	if (ARG_COUNT(ht) != 1 ||
	    zend_get_parameters_ex(1, &doit) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(doit);
	
	swf_posround((*doit)->value.lval);
}

#endif
